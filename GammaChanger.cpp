#include <iostream>
#include <sstream>

#include <Windows.h>
#include <Wingdi.h>

int strToInt(const char *str) {
	std::stringstream ss(str);

	int out;
	ss >> out;

	if (ss.fail() || out > 255 || out < 0) {
		std::cout << "Invalid arguments!" << std::endl;
		return 0;
	}

	return out;
}

HDC getPrimaryMonitorDC() {
	HMONITOR monitor = MonitorFromPoint({ 0, 0 }, MONITOR_DEFAULTTOPRIMARY);

	MONITORINFOEX info;
	info.cbSize = sizeof(MONITORINFOEX);
	GetMonitorInfo(monitor, &info);

	return CreateDC(info.szDevice, NULL, NULL, 0);
}

void setGamma(HDC dc, int gamma) {
	WORD ramp[3][256];

	for (int i = 0; i < 256; ++i) {
		int value = i * (256 + gamma);

		if (value > 65535) {
			value = 65535;
		}

		ramp[0][i] = value;
		ramp[1][i] = value;
		ramp[2][i] = value;
	}

	SetDeviceGammaRamp(dc, &ramp);
}

int main(int argc, char **argv) {
	HDC dc = getPrimaryMonitorDC();

	WORD oldRamp[3][256];
	if (GetDeviceGammaRamp(dc, &oldRamp) != TRUE) {
		std::cerr << "Failed to get the old gamma ramp!" << std::endl;
		std::cin.get();
		exit(1);
	}

	int gamma = 0;
	if (argc == 2) {
		gamma = strToInt(argv[1]);
	}
	else {
		std::cout << "Usage:\n"
			<< "GammaChanger <gamma>\n"
			<< "The gamma value must be in between 0 and 255." << std::endl;
		std::cin.get();
		return 0;
	}

	setGamma(dc, gamma);
	std::cout << "Gamma ramp set, press enter to exit." << std::endl;
	std::cin.get();
	SetDeviceGammaRamp(dc, &oldRamp);

	DeleteDC(dc);

	return 0;
}