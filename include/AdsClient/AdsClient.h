#ifndef SAGITTAL_BALANCE_BRIDGE_ADSCLIENT_H
#define SAGITTAL_BALANCE_BRIDGE_ADSCLIENT_H

#include <array>
#include <string>
#include <set>
#include <map>

#ifdef WIN32
#include <Windows.h>
#endif

#include <TcAdsDef.h>
#include <TcAdsAPI.h>

// Symbol forward declaration
template <typename T>
class AdsSymbol;

/**
 * Class to wrap around ADS communication.
 *
 * Note: the underlying ADS library almost never uses `const` for it's argument when
 * it would be appropriate. So we do not use it often either.
 */
class AdsClient {

public:

    using VariableHandle = unsigned long;
    using NotificationHandle = unsigned long;

    /**
     * Map of error codes to human-readable error messages.
     *
     * See https://infosys.beckhoff.com/content/1033/tcplclib_tc2_utilities/374277003.html?id=8315955496832679590.
     */
    static const std::map<unsigned int, std::string> ads_error_message;

    /**
     * @param port Port inside ADS connection
     * @param ams_address Target IP / hostname (default: local)
     */
    explicit AdsClient(unsigned short port = AMSPORT_R0_PLC_TC3, const std::array<unsigned char,6>& ams_address = {0});

    /**
     * Destructor
     */
    ~AdsClient();

    /**
     * @brief Register variable handle by name
     *
     * The handle should be released again before the connection is closed. This is not done
     * automatically by this connection class.
     *
     * @param name
     * @return
     */
    VariableHandle getVariableByName(std::string name);

    /**
     * Get variable information based on it's name.
     *
     * @param name
     * @return
     */
    AdsSymbolEntry getVariableInfo(std::string name);

    /**
     * @brief Unregister a variable handle
     * @param handle
     * @return
     */
    bool releaseVariableHandle(VariableHandle handle);

    /**
     * Read from remote using address.
     *
     * @param index_group
     * @param index_offset
     * @param buffer
     * @param num_bytes
     * @return
     */
    bool read(const unsigned long& index_group,
              const unsigned long& index_offset,
              void* buffer,
              const unsigned long& num_bytes);

    /**
     * Write data by address.
     *
     * @param handle
     * @param data
     * @param num_bytes
     * @return
     */
    bool write(const unsigned long& index_group,
               const unsigned long& index_offset,
               void *data,
               const unsigned long& num_bytes);

    /**
     * Read data from variable handle
     *
     * @param handle
     * @param[out] buffer
     * @param num_bytes
     * @return
     */
    bool read_by_handle(const VariableHandle& handle,
                        void* buffer,
                        const unsigned long& num_bytes);

    /**
     * Write data by variable handle
     *
     * @param handle
     * @param data
     * @param num_bytes
     * @return
     */
    bool write_by_handle(const VariableHandle& handle,
                         void *data,
                         const unsigned long& num_bytes);

    /**
     * Attach a function callback to a remote value change inside TwinCAT.
     *
     * It is recommended to use `AdsSymbol::registerNotification` instead.
     * Note: the callback will fire immediately upon creating the notfication,
     * regardless of the value.
     *
     * @param callback      User callback function
     * @param var_length    Size of the watched variable - ignored when using
     *                      custom attribute
     * @param user_handle   User-defined handle, sent to callback
     * @param attrib        Notification attributes (optional)
     * @return Notification handle
     */
    //{
    /**
     * @param handle        Variable handle
     */
    NotificationHandle registerNotification(const VariableHandle& handle,
                                            PAdsNotificationFuncEx callback,
                                            const unsigned long& var_length = 8,
                                            const unsigned long& user_handle = 0,
                                            AdsNotificationAttrib* attrib = nullptr);
    /**
     * @param index_group    Variable group index
     * @param index_offset   Variable index offset
     */
    NotificationHandle registerNotification(const unsigned long& index_group,
                                            const unsigned long& index_offset,
                                            PAdsNotificationFuncEx callback,
                                            const unsigned long& var_length = 8,
                                            const unsigned long& user_handle = 0,
                                            AdsNotificationAttrib* attrib = nullptr);
    //}

    /**
     * Free a notification callback
     *
     * @param handle
     * @return
     */
    bool clearNotification(NotificationHandle handle);

    /**
     * Disconnect
     */
    void close();

    /**
     * Get Symbol instance.
     *
     * @tparam T Type of the remote variable
     * @param name
     * @return
     */
    template<typename T>
    AdsSymbol<T> getSymbolByName(const std::string& name);

    /**
     * Turn an error code into a message.
     *
     * @param error
     * @return
     */
    static std::string getAdsErrorMessage(const unsigned long& error);

protected:
    long m_ads_port; ///< Twincat port
    AmsAddr m_address; ///< AMS address
    PAmsAddr m_p_address; ///< Pointer to AMS address

    bool m_connected; ///< Keep track if still connected to ADS

    std::set<NotificationHandle> m_notification_handles; ///< List of notification handles
};

template<typename T, typename Item>
void erase_safe(T& list, const Item& item);

// For template implementation:
#include "AdsClient_imp.h"

#endif //SAGITTAL_BALANCE_BRIDGE_ADSCLIENT_H
