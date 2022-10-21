#include <cassert>

#include "AdsClient/AdsSymbol.h"

template<typename T>
std::map<ulong, typename AdsSymbol<T>::Callback> AdsSymbol<T>::user_callbacks_ = {};

template<typename T>
AdsSymbol<T>::AdsSymbol(AdsClient& client, const std::string& name, bool use_handle) :
    client_(&client),
    name_(name),
    use_handle_(use_handle) {

    index_.group = index_.offset = 0;

    if (use_handle) {
        index_.group = ADSIGRP_SYM_VALBYHND;
        index_.offset = client_->getVariableByName(name_);
    } else {
        const auto info = client.getVariableInfo(name);
        index_.group = info.iGroup;
        index_.offset = info.iOffs;
    }
}

template<typename T>
AdsSymbol<T>::AdsSymbol(AdsClient& client,
                        const unsigned long& index_group,
                        const unsigned long& index_offset) :
        client_(&client),
        name_(""),
        use_handle_(false),
        index_(index_group, index_offset) {
}

template<typename T>
AdsSymbol<T>::~AdsSymbol<T>() {

    while (!notification_handles_.empty()) {
        clearNotification(*notification_handles_.begin());
        // Entries in the set will be removed, so use while loop instead of iterators
    }

    if (use_handle_) {
        client_->releaseVariableHandle(index_.offset);
    }
}

template<typename T>
const T& AdsSymbol<T>::read() {

    if (use_handle_) {
        client_->read_by_handle(
                index_.offset,
                (void*) &value_,
                sizeof(value_)
        );
    } else {
        client_->read(
                index_.group,
                index_.offset,
                (void*) &value_,
                sizeof(value_)
        );
    }

    return value();
}

template<typename T>
bool AdsSymbol<T>::write(const T& new_value) {

    bool result;

    if (use_handle_) {
        result = client_->write_by_handle(index_.offset,
                                          (void*) &new_value,
                                          sizeof(new_value));
    } else {
        result = client_->write(index_.group,
                                index_.offset,
                                (void*) &new_value,
                                sizeof(new_value));
    }


    if (result) {
        value_ = new_value;
    }

    return result;
}

template<typename T>
bool AdsSymbol<T>::write() {
    return write(value_);
}

template<typename T>
const T& AdsSymbol<T>::value() const {
    return value_;
}

template<typename T>
T& AdsSymbol<T>::value() {
    return value_;
}

template<typename T>
unsigned long AdsSymbol<T>::registerDeviceNotification(Callback user_callback,
                                                       unsigned long user_handle,
                                                       AdsNotificationAttrib* attrib) {

    auto noti_handle = client_->registerNotification(index_.group,
                                                     index_.offset,
                                                     &notificationCallback,
                                                     sizeof(T),
                                                     user_handle,
                                                     attrib);

    if (noti_handle == 0) {
        return 0;
    }

    notification_handles_.insert(noti_handle);
    user_callbacks_[noti_handle] = user_callback;

    return noti_handle;
}

template<typename T>
void AdsSymbol<T>::notificationCallback(AmsAddr* ams_addr, AdsNotificationHeader* notification, ulong user_handle) {

    assert(sizeof(T) == notification->cbSampleSize);

    T* new_value_ptr = (T*)(notification->data);
    user_callbacks_[notification->hNotification](*new_value_ptr);
}

template<typename T>
bool AdsSymbol<T>::clearNotification(unsigned long noti_handle) {
    bool result = client_->clearNotification(noti_handle);
    notification_handles_.erase(noti_handle);
    user_callbacks_.erase(noti_handle);
    return result;
}
