#include <AdsClient/AdsClient.h>

const std::map<unsigned int, std::string> AdsClient::ads_error_message = {
        {ADSERR_DEVICE_ERROR, "{General device error."},
        {ADSERR_DEVICE_SRVNOTSUPP, "Service is not supported by the server."},
        {ADSERR_DEVICE_INVALIDGRP, "Invalid index group."},
        {ADSERR_DEVICE_INVALIDOFFSET, "Invalid index offset."},
        {ADSERR_DEVICE_INVALIDACCESS, "Reading or writing not permitted."},
        {ADSERR_DEVICE_INVALIDSIZE, "Parameter size not correct."},
        {ADSERR_DEVICE_INVALIDDATA, "Invalid data values."},
        {ADSERR_DEVICE_NOTREADY, "Device is not ready to operate."},
        {ADSERR_DEVICE_BUSY, "Device is busy."},
        {ADSERR_DEVICE_INVALIDCONTEXT, "Invalid operating system context. This can result from use of ADS blocks in different tasks. It may be possible to resolve this through multitasking synchronization in the PLC."},
        {ADSERR_DEVICE_NOMEMORY, "Insufficient memory."},
        {ADSERR_DEVICE_INVALIDPARM, "Invalid parameter values."},
        {ADSERR_DEVICE_NOTFOUND, "Not found (files, ...)."},
        {ADSERR_DEVICE_SYNTAX, "Syntax error in file or command."},
        {ADSERR_DEVICE_INCOMPATIBLE, "Objects do not match."},
        {ADSERR_DEVICE_EXISTS, "Object already exists."},
        {ADSERR_DEVICE_SYMBOLNOTFOUND, "Symbol not found."},
        {ADSERR_DEVICE_SYMBOLVERSIONINVALID, "Invalid symbol version. This can occur due to an online change. Create a new handle."},
        {ADSERR_DEVICE_INVALIDSTATE, "Device (server) is in invalid state."},
        {ADSERR_DEVICE_TRANSMODENOTSUPP, "AdsTransMode not supported."},
        {ADSERR_DEVICE_NOTIFYHNDINVALID, "Notification handle is invalid."},
        {ADSERR_DEVICE_CLIENTUNKNOWN, "Notification client not registered."},
        {ADSERR_DEVICE_NOMOREHDLS, "No further handle available."},
        {ADSERR_DEVICE_INVALIDWATCHSIZE, "Notification size too large."},
        {ADSERR_DEVICE_NOTINIT, "Device not initialized."},
        {ADSERR_DEVICE_TIMEOUT, "Device has a timeout."},
        {ADSERR_DEVICE_NOINTERFACE, "Interface query failed."},
        {ADSERR_DEVICE_INVALIDINTERFACE, "Wrong interface requested."},
        {ADSERR_DEVICE_INVALIDCLSID, "Class ID is invalid."},
        {ADSERR_DEVICE_INVALIDOBJID, "Object ID is invalid."},
        {ADSERR_DEVICE_PENDING, "Request pending."},
        {ADSERR_DEVICE_ABORTED, "Request is aborted."},
        {ADSERR_DEVICE_WARNING, "Signal warning."},
        {ADSERR_DEVICE_INVALIDARRAYIDX, "Invalid array index."},
        {ADSERR_DEVICE_SYMBOLNOTACTIVE, "Symbol not active."},
        {ADSERR_DEVICE_ACCESSDENIED, "Access denied."},
        {ADSERR_DEVICE_LICENSENOTFOUND, "Missing license."},
        {ADSERR_DEVICE_LICENSEEXPIRED, "License expired."},
        {ADSERR_DEVICE_LICENSEEXCEEDED, "License exceeded."},
        {ADSERR_DEVICE_LICENSEINVALID, "Invalid license."},
        {ADSERR_DEVICE_LICENSESYSTEMID, "License problem: System ID is invalid."},
        {ADSERR_DEVICE_LICENSENOTIMELIMIT, "License not limited in time."},
        {ADSERR_DEVICE_LICENSEFUTUREISSUE, "Licensing problem: time in the future."},
        {ADSERR_DEVICE_LICENSETIMETOLONG, "License period too long."},
        {ADSERR_DEVICE_EXCEPTION, "Exception at system startup."},
        {ADSERR_DEVICE_LICENSEDUPLICATED, "License file read twice."},
        {ADSERR_DEVICE_SIGNATUREINVALID, "Invalid signature."},
        {ADSERR_DEVICE_CERTIFICATEINVALID, "Invalid certificate."},
        {ADSERR_CLIENT_ERROR, "Client error."},
        {ADSERR_CLIENT_INVALIDPARM, "Service contains an invalid parameter."},
        {ADSERR_CLIENT_LISTEMPTY, "Polling list is empty."},
        {ADSERR_CLIENT_VARUSED, "Var connection already in use."},
        {ADSERR_CLIENT_DUPLINVOKEID, "The called ID is already in use."},
        {ADSERR_CLIENT_SYNCTIMEOUT, "Timeout has occurred – the remote terminal is not responding in the specified ADS timeout. The route setting of the remote terminal may be configured incorrectly."},
        {ADSERR_CLIENT_W32ERROR, "Error in Win32 subsystem."},
        {ADSERR_CLIENT_TIMEOUTINVALID, "Invalid client timeout value."},
        {ADSERR_CLIENT_PORTNOTOPEN, "Port not open."},
        {ADSERR_CLIENT_NOAMSADDR, "No AMS address."},
        {ADSERR_CLIENT_SYNCINTERNAL, "Internal error in Ads sync."},
        {ADSERR_CLIENT_ADDHASH, "Hash table overflow."},
        {ADSERR_CLIENT_REMOVEHASH, "Key not found in the table."},
        {ADSERR_CLIENT_NOMORESYM, "No symbols in the cache."},
        {ADSERR_CLIENT_SYNCRESINVALID, "Invalid response received."},
        {ADSERR_CLIENT_SYNCPORTLOCKED, "Sync Port is locked."},
};
