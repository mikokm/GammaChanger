#ifndef GAMMARAMP_HPP
#define GAMMARAMP_HPP

#include "Windows.h"

class GammaRamp {
public:
	GammaRamp();
	~GammaRamp();

	void loadGammaRamp(const std::string &filename);
	void saveGammaRamp(const std::string &filename);

	void setGammaRamp(double gamma);

private:
	HDC m_dc;
};


#endif