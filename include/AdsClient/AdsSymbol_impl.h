#include <cassert>

#include "AdsClient/AdsSymbol.h"

template<typename T>
std::map<AdsClient::NotificationHandle, typename AdsSymbol<T>::Callback> AdsSymbol<T>::user_callbacks = {};

template<typename T>
AdsSymbol<T>::AdsSymbol(AdsClient& client, const std::string& name, bool use_handle) :
    m_client(&client),
    m_name(name),
    m_use_handle(use_handle) {

    m_index.group = m_index.offset = 0;

    if (use_handle) {
        m_index.group = ADSIGRP_SYM_VALBYHND;
        m_index.offset = m_client->getVariableByName(m_name);
    } else {
        const auto info = client.getVariableInfo(name);
        m_index.group = info.iGroup;
        m_index.offset = info.iOffs;
    }
}

template<typename T>
AdsSymbol<T>::AdsSymbol(AdsClient& client,
                        const unsigned long& index_group,
                        const unsigned long& index_offset) :
        m_client(&client),
        m_name(""),
        m_use_handle(false),
        m_index(index_group, index_offset) {
}

template<typename T>
AdsSymbol<T>::~AdsSymbol<T>() {

    while (!m_notification_handles.empty()) {
        clearNotification(*m_notification_handles.begin());
        // Entries in the set will be removed, so use while loop instead of iterators
    }

    if (m_use_handle) {
        m_client->releaseVariableHandle(m_index.offset);
    }
}

template<typename T>
const T& AdsSymbol<T>::read() {

    if (m_use_handle) {
        m_client->read_by_handle(
                m_index.offset,
                (void*) &m_value,
                sizeof(m_value)
        );
    } else {
        m_client->read(
                m_index.group,
                m_index.offset,
                (void*) &m_value,
                sizeof(m_value)
        );
    }

    return value();
}

template<typename T>
bool AdsSymbol<T>::write(const T& new_value) {

    bool result;

    if (m_use_handle) {
        result = m_client->write_by_handle(m_index.offset,
                                           (void*) &new_value,
                                           sizeof(new_value));
    } else {
        result = m_client->write(m_index.group,
                                 m_index.offset,
                                 (void*) &new_value,
                                 sizeof(new_value));
    }


    if (result) {
        m_value = new_value;
    }

    return result;
}

template<typename T>
bool AdsSymbol<T>::write() {
    return write(m_value);
}

template<typename T>
const T& AdsSymbol<T>::value() const {
    return m_value;
}

template<typename T>
T& AdsSymbol<T>::value() {
    return m_value;
}

template<typename T>
unsigned long AdsSymbol<T>::registerDeviceNotification(Callback user_callback,
                                                       unsigned long user_handle,
                                                       AdsNotificationAttrib* attrib) {

    auto noti_handle = m_client->registerNotification(m_index.group,
                                                      m_index.offset,
                                                      &notificationCallback,
                                                      sizeof(T),
                                                      user_handle,
                                                      attrib);

    if (noti_handle == 0) {
        return 0;
    }

    m_notification_handles.insert(noti_handle);
    user_callbacks[noti_handle] = user_callback;

    return noti_handle;
}

template<typename T>
void AdsSymbol<T>::notificationCallback(AmsAddr* ams_addr,
                                        AdsNotificationHeader* notification,
                                        unsigned long user_handle) {

    assert(sizeof(T) == notification->cbSampleSize);

    T* new_value_ptr = (T*)(notification->data);
    user_callbacks[notification->hNotification](*new_value_ptr);
}

template<typename T>
bool AdsSymbol<T>::clearNotification(unsigned long handle) {
    bool result = m_client->clearNotification(handle);
    erase_safe(m_notification_handles, handle);
    erase_safe(user_callbacks, handle);
    return result;
}
