#include <iostream>
#include <thread>

#include <AdsClient/AdsClient.h>

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
        clearNotification(*notification_handles_.begin());
        // Entries in the set will be removed, so use while loop instead of iterators
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

AdsSymbolEntry AdsClient::getVariableInfo(std::string name) {

    void* name_ptr = &name[0];

    AdsSymbolEntry info;

    long error = AdsSyncReadWriteReq(p_address_,
                                     ADSIGRP_SYM_INFOBYNAMEEX,
                                     0,
                                     sizeof(info),
                                     &info,
                                     (unsigned long)name.size(),
                                     name_ptr);

    if (error) {
        std::cerr << "Error in getVariableInfo: " << getAdsErrorMessage(error) << std::endl;
    }

    return info;
}

bool AdsClient::releaseVariableHandle(VariableHandle handle) {

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

bool AdsClient::read(const unsigned long& index_group,
                     const unsigned long& index_offset,
                     void* buffer,
                     const unsigned long& num_bytes) {

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

bool AdsClient::write(const unsigned long& index_group,
                      const unsigned long& index_offset,
                      void* data,
                      const unsigned long& num_bytes) {

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

bool AdsClient::read_by_handle(const VariableHandle& handle,
                               void* buffer,
                               const unsigned long& num_bytes) {

    return read(ADSIGRP_SYM_VALBYHND,
                handle,
                buffer,
                num_bytes);
}

bool AdsClient::write_by_handle(const VariableHandle& handle,
                                void* data,
                                const unsigned long& num_bytes) {

    return write(ADSIGRP_SYM_VALBYHND,
                 handle,
                 data,
                 num_bytes);
}

unsigned long AdsClient::registerNotification(const VariableHandle& handle,
                                              PAdsNotificationFuncEx callback,
                                              const unsigned long& var_length,
                                              const unsigned long& user_handle,
                                              AdsNotificationAttrib* attrib) {

    return registerNotification(ADSIGRP_SYM_VALBYHND,
                                handle,
                                callback,
                                var_length,
                                user_handle,
                                attrib);
}

unsigned long AdsClient::registerNotification(const unsigned long& index_group,
                                              const unsigned long& index_offset,
                                              PAdsNotificationFuncEx callback,
                                              const unsigned long& var_length,
                                              const unsigned long& user_handle,
                                              AdsNotificationAttrib* attrib) {

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
                                                   index_group,
                                                   index_offset,
                                                   attrib,
                                                   callback,
                                                   user_handle,
                                                   &noti_handle);

    if (error) {
        std::cerr << "Error in registerNotification: " << getAdsErrorMessage(error) << std::endl;
        return 0;
    }

    notification_handles_.insert(noti_handle); // add handle to list
    return noti_handle;
}

bool AdsClient::clearNotification(NotificationHandle handle) {

    long error = AdsSyncDelDeviceNotificationReqEx(ads_port_, p_address_, handle);

    notification_handles_.erase(handle);

    // Remove anyway - if release fails we cannot do anything else

    if (error) {
        std::cerr << "Error in clearNotification: " << getAdsErrorMessage(error) << std::endl;
        return false;
    }

    return true;
}

/**
 * Cannot return a const reference because of the temporary fallback string.
 */
std::string AdsClient::getAdsErrorMessage(const unsigned long& error) {

    auto pos = ads_error_message.find(error);

    if (pos == ads_error_message.end()) {
        return "Unknown error (" + std::to_string(error) + ")";
    }

    return pos->second;
}
