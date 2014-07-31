#ifndef ADLWRAPPER_HPP
#define ADLWRAPPER_HPP

#include <Windows.h>

class ADLWrapper {
public:
	ADLWrapper();
	~ADLWrapper();
	
	bool setSaturdation(int saturdation);

private:
	bool loadDriver();
	void releaseDriver();

	HINSTANCE m_hdll;
};


#endif