#include <iostream>
#include <fstream>
#include <stdexcept>

#include <Windows.h>
#include <Wingdi.h>

#include "GammaRamp.hpp"

GammaRamp::GammaRamp() {
	HMONITOR monitor = MonitorFromPoint({ 0, 0 }, MONITOR_DEFAULTTOPRIMARY);
	MONITORINFOEX info;
	info.cbSize = sizeof(MONITORINFOEX);
	GetMonitorInfo(monitor, &info);
	m_dc = CreateDC(info.szDevice, NULL, NULL, 0);

	if (m_dc == NULL) {
		throw std::runtime_error("GammaRamp(): Failed to get the primary monitor DC!");
	}
}

GammaRamp::~GammaRamp() {
	DeleteDC(m_dc);
}

void GammaRamp::loadGammaRamp(const std::string &filename) {
	std::fstream file(filename, std::ios::in);
	if (!file.is_open()) {
		throw std::runtime_error("loadGammaRamp: Failed to open gamma ramp file!");
	}

	WORD ramp[3][256];
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 256; ++j) {
			file >> std::skipws >> ramp[i][j];
		}
	}

	SetDeviceGammaRamp(m_dc, &ramp);

	file.close();
}

void GammaRamp::saveGammaRamp(const std::string &filename) {
	WORD ramp[3][256];

	if (GetDeviceGammaRamp(m_dc, &ramp) != TRUE) {
		throw std::runtime_error("saveGammaRamp: Failed to get the old gamma ramp!");
	}

	std::fstream file(filename, std::ios::out | std::ios::trunc);
	if (!file.is_open()) {
		throw std::runtime_error("saveGammaRamp: Failed to open gamma ramp file!");
	}

	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 256; ++j) {
			file << ramp[i][j] << " ";
		}
	}

	file.close();
}

void GammaRamp::setGammaRamp(double gamma) {
	WORD ramp[3][256];

	for (int i = 0; i < 256; ++i) {
		double value = std::pow(i / 256.0, 1.0 / gamma) * 65535.0 + 0.5;

		if (value > 65535) {
			value = 65535;
		}

		ramp[0][i] = static_cast<int> (value);
		ramp[1][i] = static_cast<int> (value);
		ramp[2][i] = static_cast<int> (value);
	}

	SetDeviceGammaRamp(m_dc, &ramp);
}