//Code form Nieks (Nicholas) Beckers
#pragma once

#ifdef WIN32
#include <iostream>
#include <windows.h>
#include <conio.h>
#include <vector>
#include <algorithm>

#include "TcAdsDef.h"
#include "TcAdsApi.h"


class tcAdsClient
{
	public:
		tcAdsClient(unsigned short port);
		~tcAdsClient() {}
		long nErr;
		unsigned long getVariableHandle(char* szVarIn, int numBytes);
		void releaseVariableHandle(unsigned long hVar);
		unsigned long registerTCAdsDeviceNotification(unsigned long lhUser, unsigned long vhUser, PAdsNotificationFuncEx callback, unsigned long cbLength = 8);
		void unregisterTCAdsDeviceNotification(unsigned long hNotification);
		void read(unsigned long lHdlVar, void *pData, int numBytes);
		void write(unsigned long lHdlVar, void *pData, int numBytes);
		void disconnect();
	protected:
		AmsAddr _addr;
		PAmsAddr _pAddr = &_addr;
		long _nPort;
		unsigned long _pcbReturn;

		// params for event-driven interactions with ADS
		std::vector<unsigned long> _hNotifications, _hVariables;

};
#endif

