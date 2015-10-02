#include <iostream>
#include <sstream>

#include "ADLWrapper.hpp"
#include "GammaRamp.hpp"

namespace {
	double parseGamma(const char *str) {
		std::stringstream ss(str);
		double gamma;
		ss >> gamma;

		if (ss.fail()) {
			return 0;
		}

		return gamma;
	}

	void printUsage() {
		std::cout << "Usage:\n"
			<< "GammaChanger load <filename>" << std::endl
			<< "GammaChanger save <filename>" << std::endl
			<< "GammaChanger set <gamma>" << std::endl;
		exit(0);
	}
}

int main(int argc, char **argv) {
	if (argc != 3) {
		printUsage();
	}

	std::string mode(argv[1]);

	try {
		ADLWrapper adl;
		GammaRamp ramp;

		if (mode == "load") {
			std::cout << "Loading the saved gamma ramp." << std::endl;
			ramp.loadGammaRamp(std::string(argv[2]));
			adl.setSaturation(100);
		}
		else if (mode == "save") {
			std::cout << "Saving the current gamma ramp." << std::endl;
			ramp.saveGammaRamp(std::string(argv[2]));
		}
		else if (mode == "set") {
			double gamma = parseGamma(argv[2]);
			if (gamma > 3.0 || gamma < 0.1) {
				std::cerr << "The gamma value must be between 0.1 and 3.0." << std::endl;
				return 1;
			}

			std::cout << "Setting a new gamma ramp." << std::endl;
			ramp.setGammaRamp(gamma);
			adl.setSaturation(200);
		}
		else {
			std::cerr << "Invalid mode specified!" << std::endl;
			return 1;
		}
	}
	catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
	}

	return 0;
}
