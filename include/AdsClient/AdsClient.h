#ifndef SAGITTAL_BALANCE_BRIDGE_ADSCLIENT_H
#define SAGITTAL_BALANCE_BRIDGE_ADSCLIENT_H

#include <array>
#include <string>
#include <vector>

#ifdef WIN32
#include <windows.h>
#endif

#include <TcAdsDef.h>
#include <TcAdsAPI.h>

/**
 * Class to wrap around ADS communication
 */
class AdsClient {

public:

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
     * @param name
     * @return
     */
    unsigned long getVariableByName(std::string name);

    /**
     * @brief Unregister a variable handle
     * @param handle
     * @return
     */
    bool releaseVariableHandle(unsigned long handle);

    /**
     * Read data from variable handle
     *
     * @param handle
     * @param[out] buffer
     * @param num_bytes
     * @return
     */
    bool read(unsigned long handle, void *buffer, int num_bytes);

    /**
     * Write data by variable handle
     *
     * @param handle
     * @param data
     * @param num_bytes
     * @return
     */
    bool write(unsigned long handle, void *data, int num_bytes);

    /**
     * Attach a function callback to a remote value change inside TwinCAT.
     *
     * @param handle
     * @param callback
     * @param var_length    Size of the watched variable - ignored when using
     *                      custom attribute
     * @param user_handle   User-defined handle, attached to callback (default:
     *                      handled automatically)
     * @param attrib        Notification attributes (optional)
     * @return Notification handle
     */
    unsigned long registerNotification(unsigned long handle,
                                       PAdsNotificationFuncEx callback,
                                       unsigned long var_length = 8,
                                       unsigned long user_handle = 0,
                                       AdsNotificationAttrib* attrib = nullptr);

    /**
     * Free a notification callback
     *
     * @param noti_handle
     * @return
     */
    bool clearNotification(unsigned long noti_handle);

    /**
     * Disconnect
     */
    void close();

protected:
    long ads_port_; ///< Twincat port
    AmsAddr address_; ///< AMS address
    PAmsAddr p_address_; ///< Pointer to AMS address

    bool connected_; ///< Keep track if still connected to ADS

    std::vector<unsigned long> variable_handles_; ///< List of variable handles
    std::vector<unsigned long> notification_handles_; ///< List of notification handles

    static unsigned long USER_HANDLE; ///< Increasing handle for notifications
};


#endif //SAGITTAL_BALANCE_BRIDGE_ADSCLIENT_H
