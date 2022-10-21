#ifndef ADSCLIENT_ADSSYMBOL_H
#define ADSCLIENT_ADSSYMBOL_H

#include <string>
#include <map>
#include <set>

#include "AdsClient.h"

/**
 * Wrapper for remote ADS symbol.
 *
 * Any handle acquisition and release is done automatically.
 *
 * Take care with integer types. These PLC types correspond to the following C++ types:
 * SINT (8 bit)     -> char
 * INT (16 bit)     -> short
 * DINT (32 bit)    -> int
 * LINT (64 bit)    -> long
 */
template <typename T>
class AdsSymbol {
public:

    using VariableHandle = AdsClient::VariableHandle;
    using NotificationHandle = AdsClient::NotificationHandle;

    struct Index {
        unsigned long group;
        unsigned long offset;
    };

    typedef void (*Callback)(const T&);

    /**
     * Make symbol by name.
     *
     * @param client Active TwinCAT connection
     * @param name Remote variable name
     * @param use_handle If true, rely on variable handle instead of offsets
     */
    explicit AdsSymbol(AdsClient& client, const std::string& name, bool use_handle = false);

    /**
     * Make symbol through indices.
     *
     * @param client
     * @param index_group
     * @param index_offset
     */
    AdsSymbol(AdsClient& client,
              const unsigned long& index_group,
              const unsigned long& index_offset);

    ~AdsSymbol();

    /**
     * Get current ADS value and return it.
     *
     * Last value is also bufferend locally.
     *
     * @return
     */
    const T& read();

    /**
     * Write a new value to ADS.
     *
     * This will also update the local buffer.
     *
     * @param new_value
     * @return True on success
     */
    bool write(const T& new_value);

    /**
     * Write the edited buffered value.
     *
     * Use in combination with `value()`. Useful for very large
     * variables that you don't want to copy first.
     *
     * @return True on success
     */
    bool write();

    /**
     * Get last known buffered value.
     *
     * @return
     */
    const T& value() const;

    /**
     * Get editable reference to buffered value.
     *
     * Use in combination with `write()`.
     *
     * @return
     */
    T& value();

    /**
     * Create notification for symbol.
     *
     * See `AdsClient::registerDeviceNotification`.
     * The local buffer will also be updated on each call.
     *
     * @param user_callback
     * @return
     */
    unsigned long registerDeviceNotification(Callback user_callback,
                                             unsigned long user_handle = 0,
                                             AdsNotificationAttrib* attrib = nullptr);
    /**
     * Release a notification
     *
     * @param handle
     * @return
     */
    bool clearNotification(unsigned long handle);

protected:

    /**
     * Callback that will be used for every notification.
     *
     * It will route back to the desired user callback
     *
     * @param ams_addr
     * @param notification
     * @param user_handle
     */
    static void notificationCallback(AmsAddr* ams_addr,
                                     AdsNotificationHeader* notification,
                                     unsigned long user_handle);

    /**
     * Track user callbacks by their notification handles.
     *
     * Each notification handle is unique, also across variables and functions. TwinCAT increments
     * the number and resets it at startup.
     */
    static std::map<NotificationHandle, Callback> user_callbacks;

    AdsClient* m_client; ///< Pointer to active TwinCAT connection

    std::string m_name; ///< Remote variable name

    bool m_use_handle; ///< If true, `index_` is a variable handle
    Index m_index; ///< Variable indices

    T m_value; ///< Last known remote value

    std::set<NotificationHandle> m_notification_handles; ///< Notification handles for this symbol
};

// Load implementation of this template class (cannot be compiled like regular .cpp)
#include "AdsSymbol_impl.h"

#endif //ADSCLIENT_ADSSYMBOL_H
