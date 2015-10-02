#include "ADLWrapper.hpp"

#include <tchar.h>
#include "adl_sdk.h"

#include <iostream>
#include <stdexcept>

namespace {
	void* __stdcall ADL_Main_Memory_Alloc(int size) {
		return reinterpret_cast<void*> (new char[size]);
	}

	void __stdcall ADL_Main_Memory_Free(void** buffer) {
		if (NULL != *buffer) {
			delete[] * buffer;
			*buffer = NULL;
		}
	}

	template<typename T> T getProcAddW(HINSTANCE module, LPCSTR str) {
		T t = reinterpret_cast<T> (GetProcAddress(module, str));
		if (t == NULL) {
			std::cerr << "Failed to load module: " << str << std::endl;
			throw std::runtime_error("Failed to load symbol!");
		}

		return t;
	}
}

namespace ADL {
	typedef int(*ADL_MAIN_CONTROL_CREATE)(ADL_MAIN_MALLOC_CALLBACK, int);
	typedef int(*ADL_MAIN_CONTROL_DESTROY)();
	typedef int(*ADL_ADAPTER_NUMBEROFADAPTERS_GET) (int*);
	typedef int(*ADL_ADAPTER_ADAPTERINFO_GET) (LPAdapterInfo, int);
	typedef int(*ADL_DISPLAY_COLORCAPS_GET) (int, int, int *, int *);
	typedef int(*ADL_DISPLAY_COLOR_GET) (int, int, int, int *, int *, int *, int *, int *);
	typedef int(*ADL_DISPLAY_COLOR_SET) (int, int, int, int);
	typedef int(*ADL_DISPLAY_DISPLAYINFO_GET) (int, int *, ADLDisplayInfo **, int);
	typedef int(*ADL_ADAPTER_PRIMARY_GET) (int*);

	ADL_MAIN_CONTROL_CREATE			 ADL_Main_Control_Create;
	ADL_MAIN_CONTROL_DESTROY		 ADL_Main_Control_Destroy;
	ADL_ADAPTER_ADAPTERINFO_GET		 ADL_Adapter_AdapterInfo_Get;
	ADL_ADAPTER_NUMBEROFADAPTERS_GET ADL_Adapter_NumberOfAdapters_Get;
	ADL_ADAPTER_PRIMARY_GET			 ADL_Adapter_Primary_Get;
	ADL_DISPLAY_COLOR_GET			 ADL_Display_Color_Get;
	ADL_DISPLAY_COLOR_SET			 ADL_Display_Color_Set;
	ADL_DISPLAY_DISPLAYINFO_GET		 ADL_Display_DisplayInfo_Get;
}

using namespace ADL;

ADLWrapper::ADLWrapper() {
	loadDriver();
}

ADLWrapper::~ADLWrapper() {
	releaseDriver();
}


bool ADLWrapper::loadDriver() {
	m_hdll = LoadLibrary("atiadlxx.dll");
	if (m_hdll == NULL) {
		std::cerr << "Failed to load 64bit AMD driver library!" << std::endl;

		m_hdll = LoadLibrary("atiadlxy.dll");
		if (m_hdll == NULL) {
			std::cerr << "Failed to load 32bit AMD driver library!" << std::endl;
			return false;
		}
	}

	try {
		ADL_Main_Control_Create = ::getProcAddW<ADL_MAIN_CONTROL_CREATE>(m_hdll, "ADL_Main_Control_Create");
		ADL_Main_Control_Destroy = ::getProcAddW<ADL_MAIN_CONTROL_DESTROY>(m_hdll, "ADL_Main_Control_Destroy");
		ADL_Adapter_NumberOfAdapters_Get = ::getProcAddW<ADL_ADAPTER_NUMBEROFADAPTERS_GET>(m_hdll, "ADL_Adapter_NumberOfAdapters_Get");
		ADL_Adapter_AdapterInfo_Get = ::getProcAddW<ADL_ADAPTER_ADAPTERINFO_GET>(m_hdll, "ADL_Adapter_AdapterInfo_Get");
		ADL_Display_Color_Get = ::getProcAddW<ADL_DISPLAY_COLOR_GET>(m_hdll, "ADL_Display_Color_Get");
		ADL_Display_Color_Set = ::getProcAddW<ADL_DISPLAY_COLOR_SET>(m_hdll, "ADL_Display_Color_Set");
		ADL_Display_DisplayInfo_Get = ::getProcAddW<ADL_DISPLAY_DISPLAYINFO_GET>(m_hdll, "ADL_Display_DisplayInfo_Get");
		ADL_Adapter_Primary_Get = ::getProcAddW<ADL_ADAPTER_PRIMARY_GET>(m_hdll, "ADL_Adapter_Primary_Get");
	}
	catch (std::exception&) {
		return false;
	}

	if (ADL_Main_Control_Create(ADL_Main_Memory_Alloc, 1) != ADL_OK) {
		std::cerr << "Failed to create ADL main control!" << std::endl;
		return false;
	}

	return true;
}

void ADLWrapper::releaseDriver() {
	ADL_Main_Control_Destroy();
	FreeLibrary(m_hdll);
}

bool ADLWrapper::setSaturation(int saturation) {
	int adapterCount;

	if (ADL_Adapter_NumberOfAdapters_Get(&adapterCount) != ADL_OK) {
		std::cerr << "Failed to get the display adapter count!" << std::endl;
		return false;
	}

	if (adapterCount <= 0) {
		std::cerr << "The display Adapter count is not a positive integer!" << std::endl;
		return false;
	}

	AdapterInfo *adapterInfo = new AdapterInfo[adapterCount];
	ADL_Adapter_AdapterInfo_Get(adapterInfo, sizeof(AdapterInfo) * adapterCount);

	int primary;
	if (ADL_Adapter_Primary_Get(&primary) != ADL_OK) {
		std::cerr << "Cannot retrieve the primary adapter!" << std::endl;
		return false;
	}

	int index = -1;
	for (int i = 0; i < adapterCount; ++i) {
		if (!adapterInfo[i].iAdapterIndex == primary) {
			index = i;
			break;
		}
	}

	if (index == -1) {
		std::cerr << "Failed to get the display adapter index!" << std::endl;
		return false;
	}

	int displayCount;
	ADLDisplayInfo *displayInfo = NULL;

	if (ADL_Display_DisplayInfo_Get(primary, &displayCount, &displayInfo, 0) != ADL_OK) {
		std::cerr << "Failed to get display info!" << std::endl;
		return false;
	}

	int dpyIndex = displayInfo[0].displayID.iDisplayLogicalIndex;

	int cur, def, min, max, step;
	if (ADL_OK == ADL_Display_Color_Get(primary, dpyIndex, ADL_DISPLAY_COLOR_SATURATION, &cur, &def, &min, &max, &step)) {
		std::cout << "Current saturation: " << cur << std::endl;
	}

	if (saturation < min || saturation > max) {
		throw std::runtime_error("Invalid saturation value!");
	}

	ADL_Display_Color_Set(primary, displayInfo[0].displayID.iDisplayLogicalIndex, ADL_DISPLAY_COLOR_SATURATION, saturation);
	std::cout << "Setting saturation to: " << saturation << std::endl;

	ADL_Main_Memory_Free(reinterpret_cast<void**>(&displayInfo));
	delete[] adapterInfo;

	return true;
}