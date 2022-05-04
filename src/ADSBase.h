#ifndef ADSBASE_h
#define ADSBASE_h

#include <vector>
#include <cmath>
#include <iostream>
#include <map>
#ifdef WIN32
#include <tcAdsClient.h>

template <typename T>
class ADSBase
{
public:
	ADSBase();
	~ADSBase();
	void connect(const int& port);
	void disconnect();
	
	void send(std::vector<T>& data, const std::string& varname);

	std::vector<T> receive(const int& length, const std::string& varname);

protected:
	tcAdsClient* client_;
	std::map<std::string, unsigned long> varNameMap_;
};

#include <ADSBase.cpp>
#endif
#endif