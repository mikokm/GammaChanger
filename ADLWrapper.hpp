#ifndef ADLWRAPPER_HPP
#define ADLWRAPPER_HPP

#include <Windows.h>

class ADLWrapper {
public:
	ADLWrapper();
	~ADLWrapper();
	
	bool setSaturation(int saturation);

private:
	bool loadDriver();
	void releaseDriver();

	HINSTANCE m_hdll;
};


#endif