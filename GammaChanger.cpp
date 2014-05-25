#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>

#include <Windows.h>
#include <Wingdi.h>
#include <Shlobj.h>

double strToDouble(const char *str) {
	std::stringstream ss(str);

	double out;
	ss >> out;

	if (ss.fail()) {
		std::cerr << "Invalid argument type: " << str << std::endl;
		exit(1);
	}

	return out;
}

void checkFileOpen(const std::fstream &file, const std::wstring &filename) {
	if (!file.is_open()) {
		std::wcerr << "Cannot open the file: " << filename << std::endl;
		std::cin.get();
		exit(1);
	}
}

HDC getPrimaryMonitorDC() {
	HMONITOR monitor = MonitorFromPoint({ 0, 0 }, MONITOR_DEFAULTTOPRIMARY);

	MONITORINFOEX info;
	info.cbSize = sizeof(MONITORINFOEX);
	GetMonitorInfo(monitor, &info);

	return CreateDC(info.szDevice, NULL, NULL, 0);
}

std::wstring getDataFilePath() {
	wchar_t *path = 0;
	HRESULT hr = SHGetKnownFolderPath(FOLDERID_LocalAppData, NULL, NULL, &path);
	std::wstringstream ss;
	ss << path << "\\GammaChanger.dat";
	CoTaskMemFree(path);

	return ss.str();
}

void loadGammaRamp(HDC dc) {
	std::wstring filename = getDataFilePath();
	std::fstream file(filename, std::ios::in);
	checkFileOpen(file, filename);

	WORD ramp[3][256];
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 256; ++j) {
			file >> std::skipws >> ramp[i][j];
		}
	}

	SetDeviceGammaRamp(dc, &ramp);

	file.close();
}

void saveGammaRamp(HDC dc) {
	WORD ramp[3][256];

	if (GetDeviceGammaRamp(dc, &ramp) != TRUE) {
		std::cerr << "Failed to get the old gamma ramp!" << std::endl;
		std::cin.get();
		exit(1);
	}

	std::wstring filename = getDataFilePath();
	std::fstream file(filename, std::ios::out | std::ios::trunc);
	checkFileOpen(file, filename);

	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 256; ++j) {
			file << ramp[i][j] << " ";
		}
	}

	file.close();
}

void setGammaRamp(HDC dc, double gamma) {
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
	double gamma = 0.0;
	std::string mode;

	if (argc > 1) {
		mode = std::string(argv[1]);
	}

	if (argc > 2) {
		gamma = strToDouble(argv[2]);
	}

	if (argc == 3 && (gamma > 3.0 || gamma < 0.1)) {
		std::cerr << "The gamma value must be in between 0.1 and 3.0." << std::endl;
		std::cin.get();
		return 1;
	}

	if (argc != 2 && argc != 3) {
		std::cout << "Usage:\n"
			<< "GammaChanger <gamma>"
			<< std::endl;
		std::cin.get();
		return 1;
	}

	HDC dc = getPrimaryMonitorDC();

	if (dc == NULL) {
		std::cerr << "Failed to get the monitor DC!" << std::endl;
		return 1;
	}

	if (mode == "load") {
		std::cout << "Loading the saved gamma ramp." << std::endl;
		loadGammaRamp(dc);
	}
	else if (mode == "save") {
		std::cout << "Saving the current gamma ramp." << std::endl;
		saveGammaRamp(dc);
	}
	else if (mode == "set") {
		std::cout << "Setting a new gamma ramp." << std::endl;
		setGammaRamp(dc, gamma);
	}
	else {
		std::cerr << "Invalid mode: " << mode << std::endl;
		std::cin.get();
	}

	DeleteDC(dc);

	return 0;
}
