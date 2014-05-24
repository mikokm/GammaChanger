#include <iostream>
#include <sstream>
#include <cmath>

#include <Windows.h>
#include <Wingdi.h>

double strToDouble(const char *str) {
	std::stringstream ss(str);

	double out;
	ss >> out;

	if (ss.fail()) {
		std::cout << "Invalid argument type: " << str << std::endl;
		exit(1);
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

void setGamma(HDC dc, double gamma) {
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

	SetDeviceGammaRamp(dc, &ramp);
}

int main(int argc, char **argv) {
	HDC dc = getPrimaryMonitorDC();

	WORD oldRamp[3][256];
	if (GetDeviceGammaRamp(dc, &oldRamp) != TRUE) {
		std::cerr << "Failed to get the old gamma ramp!" << std::endl;
		std::cin.get();
		return 1;
	}

	double gamma = 0.0;

	if (argc == 2) {
		gamma = strToDouble(argv[1]);
	}

	if (argc != 2 || gamma > 3.0 || gamma < 0.1) {
		std::cout << "Usage:\n"
			<< "GammaChanger <gamma>\n"
			<< "The gamma value must be in between 0.1 and 3.0." << std::endl;
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
