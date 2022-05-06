#include <iostream>
#include <thread>

#include <AdsClient/AdsClient.h>

unsigned long AdsClient::USER_HANDLE = 1;

AdsClient::AdsClient(unsigned short port, const std::array<unsigned char, 6>& ams_address) {

    ads_port_ = AdsPortOpen();
    connected_ = true;

    p_address_ = &address_; // Rely on custom pointer type

    if (ams_address[0] == 0) {
        long error = AdsGetLocalAddress(p_address_);

        if (error) {
            std::cerr << "Error in AdsGetLocalAddress: " << getAdsErrorMessage(error) << std::endl;
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

    while (!notification_handles_.empty()) {
        clearNotification(notification_handles_.back());
    }

    long error = AdsPortClose();
    if (error) {
        std::cerr << "Error in AdsPortClose: " << getAdsErrorMessage(error) << std::endl;
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
        std::cerr << "Error in getVariableByName: " << getAdsErrorMessage(error) << std::endl;
        return 0;
    }

    return handle;
}

bool AdsClient::releaseVariableHandle(ulong handle) {

    long error = AdsSyncWriteReqEx(ads_port_,
                                   p_address_,
                                   ADSIGRP_SYM_RELEASEHND,
                                   0,
                                   sizeof(handle),
                                   &handle);

    if (error) {
        std::cerr << "Error in releaseVariableHandle: " << getAdsErrorMessage(error) << std::endl;
        return false;
    }
    return true;
}

bool AdsClient::read(ulong_ref index_group, ulong_ref index_offset, void* buffer, ulong_ref num_bytes) {

    unsigned long bytes_read;

    long error = AdsSyncReadReqEx2(ads_port_,
                                   p_address_,
                                   index_group,
                                   index_offset,
                                   num_bytes,
                                   buffer,
                                   &bytes_read);

    if (error) {
        std::cerr << "Error in read: " << getAdsErrorMessage(error) << std::endl;
        return false;
    }
    if (bytes_read != num_bytes) {
        std::cerr << "Error while reading from variable: requested " << num_bytes
                  << " bytes, but received " << bytes_read << " bytes" << std::endl;
    }
    return true;
}

bool AdsClient::write(ulong_ref index_group, ulong_ref index_offset, void* data, ulong_ref num_bytes) {

    long error = AdsSyncWriteReqEx(ads_port_,
                                   p_address_,
                                   index_group,
                                   index_offset,
                                   num_bytes,
                                   data);

    if (error) {
        std::cerr << "Error in write: " << getAdsErrorMessage(error) << std::endl;
        return false;
    }
    return true;
}

bool AdsClient::read_by_handle(ulong_ref handle, void* buffer, ulong_ref num_bytes) {

    unsigned long bytes_read;

    long error = AdsSyncReadReqEx2(ads_port_,
                                   p_address_,
                                   ADSIGRP_SYM_VALBYHND,
                                   handle,
                                   num_bytes,
                                   buffer,
                                   &bytes_read);

    if (error) {
        std::cerr << "Error in read_by_handle: " << getAdsErrorMessage(error) << std::endl;
        return false;
    }
    if (bytes_read != num_bytes) {
        std::cerr << "Error while reading from variable: requested " << num_bytes
                  << " bytes, but received " << bytes_read << " bytes" << std::endl;
    }
    return true;
}

bool AdsClient::write_by_handle(ulong_ref handle, void* data, ulong_ref num_bytes) {

    long error = AdsSyncWriteReqEx(ads_port_,
                                   p_address_,
                                   ADSIGRP_SYM_VALBYHND,
                                   handle,
                                   num_bytes,
                                   data);

    if (error) {
        std::cerr << "Error in write_by_handle: " << getAdsErrorMessage(error) << std::endl;
        return false;
    }
    return true;
}

unsigned long AdsClient::registerNotification(ulong_ref handle,
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
        std::cerr << "Error in registerNotification: " << getAdsErrorMessage(error) << std::endl;
        return 0;
    }

    notification_handles_.push_back(noti_handle); // add hUser to list
    return noti_handle;
}

bool AdsClient::clearNotification(ulong noti_handle) {

    long error = AdsSyncDelDeviceNotificationReqEx(ads_port_, p_address_, noti_handle);

    notification_handles_.erase(
            std::remove(notification_handles_.begin(),
                        notification_handles_.end(),
                        noti_handle), notification_handles_.end()
    ); // Remove anyway - if release fails we cannot do anything else

    if (error) {
        std::cerr << "Error in clearNotification: " << getAdsErrorMessage(error) << std::endl;
        return false;
    }

    return true;
}

std::string AdsClient::getAdsErrorMessage(unsigned long error) {

    auto pos = ads_error_message.find(error);

    if (pos == ads_error_message.end()) {
        return "Unknown error (" + std::to_string(error) + ")";
    }

    return pos->second;
}
