#include <iostream>
#include <chrono>
#include <thread>

#include "AdsClient/AdsClient.h"

AdsClient::AdsClient(unsigned short port, const std::array<unsigned char, 6>& ams_address) {

    ads_port_ = AdsPortOpen();
    connected_ = true;

    p_address_ = &address_; // Rely on custom pointer type

    if (ams_address[0] == 0) {
        long error = AdsGetLocalAddress(p_address_);

        if (error) {
            std::cerr << "Error in AdsGetLocalAddress: " << error << std::endl;
        }
    } else {

        for (int i = 0; i < 6; i++) {
            p_address_->netId.b[i] = ams_address[i];
        }
    }

    p_address_->port = port;
}

AdsClient::~AdsClient() {
    close();
}

void AdsClient::close() {

    if (!connected_) {
        return;
    }

    while (!variable_handles_.empty()) {
        releaseVariableHandle(variable_handles_.back());
    }

    while (!notification_handles_.empty()) {
        clearNotification(notification_handles_.back());
    }

    long error = AdsPortClose();
    if (error) {
        std::cerr << "Error in AdsPortClose: " << error << std::endl;
    }

    connected_ = false;
}

/**
 * Do not pass string name by reference, because we need a non-const pointer.
 */
unsigned long AdsClient::getVariableByName(std::string name) {

    void* name_ptr = &name[0];

    unsigned long handle;
    long error = AdsSyncReadWriteReq(p_address_,
                                     ADSIGRP_SYM_HNDBYNAME,
                                     0,
                                     sizeof(handle),
                                     &handle,
                                     (unsigned long)name.size(),
                                     name_ptr);

    if (error) {
        std::cerr << "Error in getVariableByName: " << error << std::endl;
        return 0;
    }

    variable_handles_.push_back(handle);

    return variable_handles_.back();
}

bool AdsClient::releaseVariableHandle(unsigned long handle) {

    long error = AdsSyncWriteReqEx(ads_port_,
                                   p_address_,
                                   ADSIGRP_SYM_RELEASEHND,
                                   0,
                                   sizeof(handle),
                                   &handle);

    variable_handles_.erase(
            std::remove(variable_handles_.begin(),
                        variable_handles_.end(),
                        handle), variable_handles_.end()
    ); // Remove regardless - If release fails we cannot do anything else

    if (error) {
        std::cerr << "Error in releaseVariableHandle: " << error << std::endl;
        return false;
    }
    return true;
}

bool AdsClient::read(unsigned long handle, void* buffer, int num_bytes) {

    unsigned long bytes_read;

    long error = AdsSyncReadReqEx2(ads_port_,
                                   p_address_,
                                   ADSIGRP_SYM_VALBYHND,
                                   handle,
                                   num_bytes,
                                   buffer,
                                   &bytes_read);

    if (error) {
        std::cerr << "Error in read: " << error << std::endl;
        return false;
    }
    if (bytes_read != num_bytes) {
        std::cerr << "Error while reading from variable: requested " << num_bytes
                  << " bytes, but received " << bytes_read << " bytes" << std::endl;
    }
    return true;
}

bool AdsClient::write(unsigned long handle, void* data, int num_bytes) {

    long error = AdsSyncWriteReqEx(ads_port_,
                                   p_address_,
                                   ADSIGRP_SYM_VALBYHND,
                                   handle,
                                   num_bytes,
                                   data);

    if (error) {
        std::cerr << "Error in write: " << error << std::endl;
        return false;
    }
    return true;
}

unsigned long AdsClient::registerNotification(unsigned long handle,
                                              PAdsNotificationFuncEx callback,
                                              unsigned long var_length,
                                              unsigned long user_handle,
                                              AdsNotificationAttrib* attrib) {

    if (user_handle == 0) {
        user_handle = USER_HANDLE++;
    }

    unsigned long noti_handle = 0;

    AdsNotificationAttrib attrib_default;
    if (!attrib) {
        attrib_default.cbLength = var_length;
        attrib_default.nTransMode = ADSTRANS_SERVERONCHA; // Callback on remote value change
        attrib_default.nMaxDelay = 0;
        attrib_default.nCycleTime = 10; // In 100 ns

        attrib = &attrib_default;
    }

    long error = AdsSyncAddDeviceNotificationReqEx(ads_port_,
                                                   p_address_,
                                                   ADSIGRP_SYM_VALBYHND,
                                                   handle,
                                                   attrib,
                                                   callback,
                                                   user_handle,
                                                   &noti_handle);

    if (error) {
        std::cerr << "Error in registerNotification: " << error << std::endl;
        return 0;
    }

    notification_handles_.push_back(noti_handle); // add hUser to list
    return noti_handle;
}

bool AdsClient::clearNotification(unsigned long noti_handle) {

    long error = AdsSyncDelDeviceNotificationReqEx(ads_port_, p_address_, noti_handle);

    notification_handles_.erase(
            std::remove(notification_handles_.begin(),
                        notification_handles_.end(),
                        noti_handle), notification_handles_.end()
    ); // Remove anyway - if release fails we cannot do anything else

    if (error) {
        std::cerr << "Error in clearNotification: " << error << std::endl;
        return false;
    }

    return true;
}

unsigned long AdsClient::USER_HANDLE = 1;
