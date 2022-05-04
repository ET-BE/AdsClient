#include "tcAdsClient.h"
#ifdef WIN32
//--------------------------------------------------------------
tcAdsClient::tcAdsClient(unsigned short port)
{
	// Open communication port on the ADS router
	_nPort = AdsPortOpenEx();
	nErr = AdsGetLocalAddressEx(_nPort, _pAddr);
	if (nErr) {
		std::cerr << "[error] (class tcAdsClient) Error AdsGetLocalAddressEx: " << nErr << std::endl;
		std::cerr << "[error] (class tcAdsClient) TC ADS client not running, please start TwinCAT and restart this app" << std::endl;
	}

	// Select Port: TwinCAT 3 PLC
	_pAddr->port = port;
}

//--------------------------------------------------------------
unsigned long tcAdsClient::getVariableHandle(char* szVarIn, int numBytes)
{
	unsigned long lHdlVar;
	// Fetch handle for the PLC variable 
	nErr = AdsSyncReadWriteReqEx2(
				_nPort, 
				_pAddr, 
				ADSIGRP_SYM_HNDBYNAME, 
				0x0, 
				sizeof(lHdlVar), 
				&lHdlVar, 
				numBytes, 
				szVarIn, 
				&_pcbReturn);

	if (nErr) {
		std::cerr << "[error] (class tcAdsClient) Error AdsSyncReadWriteReqEx2: " << nErr << " - variable " << szVarIn << " not found." << std::endl;
		return -1;
	} 
	else {
		_hVariables.push_back(lHdlVar);
		return lHdlVar;
	}
}

//--------------------------------------------------------------
void tcAdsClient::releaseVariableHandle(unsigned long hVar)
{
	// release handle
	nErr = AdsSyncWriteReqEx(
				_nPort, 
				_pAddr, 
				ADSIGRP_SYM_RELEASEHND, 
				0, 
				sizeof(hVar), 
				&hVar);

	if (nErr) std::cerr << "[error] (class tcAdsClient) Error AdsSyncWriteReq: " << nErr << std::endl;
	_hVariables.erase(remove(_hVariables.begin(), _hVariables.end(), hVar), _hVariables.end()); // remove _hVariables from list
}

//--------------------------------------------------------------
unsigned long tcAdsClient::registerTCAdsDeviceNotification(unsigned long lhVar, unsigned long lhUser, PAdsNotificationFuncEx callback, unsigned long cbLength)
{

	// set the attributes of the notification
	AdsNotificationAttrib  _adsNotificationAttrib;
	_adsNotificationAttrib.cbLength = cbLength;
	_adsNotificationAttrib.nTransMode = ADSTRANS_SERVERONCHA;
	//_adsNotificationAttrib.nTransMode = ADSTRANS_SERVERCYCLE;
	_adsNotificationAttrib.nMaxDelay = 0;
	_adsNotificationAttrib.nCycleTime = 10000000; // 0.5sec 

	// initiate the transmission of the PLC-variable 
	unsigned long hNotification;
	nErr = AdsSyncAddDeviceNotificationReqEx(
				_nPort,
				_pAddr,
				ADSIGRP_SYM_VALBYHND,
				lhVar,
				&_adsNotificationAttrib,
				callback,
				lhUser,
				&hNotification);

	if (nErr) std::cerr << "[error] (class tcAdsClient) Error AdsSyncAddDeviceNotificationReq: " << nErr << std::endl;
	std::cout << "[verbose] (class tcAdsClient) Registered notification: " << hNotification << std::endl;

	_hNotifications.push_back(hNotification); // add hUser to list
	return hNotification;
}

void tcAdsClient::unregisterTCAdsDeviceNotification(unsigned long hNotification)
{
	// finish the transmission of the PLC-variable 
	nErr = AdsSyncDelDeviceNotificationReqEx(_nPort, _pAddr, hNotification);
	if (nErr) std::cerr << "[error] (class tcAdsClient) Error AdsSyncDelDeviceNotificationReq: " << nErr << '\n';
	
	_hNotifications.erase(remove(_hNotifications.begin(), _hNotifications.end(), hNotification), _hNotifications.end()); // remove hNotification from list
}

//--------------------------------------------------------------
void tcAdsClient::read(unsigned long lHdlVar, void *pData, int numBytes)
{
	// Read values of the PLC variables (by handle)
	nErr = AdsSyncReadReqEx2(
				_nPort, 
				_pAddr, 
				ADSIGRP_SYM_VALBYHND, 
				lHdlVar, 
				numBytes, 
				pData, 
				&_pcbReturn);

	//if (nErr) cerr << "Error: AdsSyncReadReqEx2: " << nErr << '\n';
}

//--------------------------------------------------------------
void tcAdsClient::write(unsigned long lHdlVar, void *pData, int numBytes)
{
	// Write to ADS (bytes).
	nErr = AdsSyncWriteReqEx(
				_nPort, // Port
				_pAddr, // Address
				ADSIGRP_SYM_VALBYHND, // IndexGroup 
				lHdlVar, // IndexOffset
				numBytes, // Size of data
				pData);

	//if (nErr) cerr << "Error: AdsSyncWriteReqEx: " << nErr << '\n';
}

//--------------------------------------------------------------
void tcAdsClient::disconnect()
{
	// unregister notifications
	while (_hNotifications.size() > 0) {
		unregisterTCAdsDeviceNotification(_hNotifications.back());
	}

	// unregister variable handles
	while (_hVariables.size() > 0) {
		releaseVariableHandle(_hVariables.back());
	}

	// close port
	nErr = AdsPortCloseEx(_nPort);
	if (nErr) std::cerr << "[error] (class tcAdsClient) Error AdsPortCloseEx: " << nErr << " on nPort: " << _nPort << std::endl;
}
#endif



