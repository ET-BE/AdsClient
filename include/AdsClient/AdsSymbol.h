#ifndef ADSCLIENT_ADSSYMBOL_H
#define ADSCLIENT_ADSSYMBOL_H

#include <string>

#include "AdsClient.h"

/**
 * Wrapper for remote ADS symbol.
 */
template <typename T>
class AdsSymbol {
public:

    /**
     *
     * @param client Active TwinCAT connection
     * @param name Remote variable name
     */
    explicit AdsSymbol(AdsClient& client,
                       const std::string& name);

    ~AdsSymbol() = default;

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
     * @return
     */
    bool write(const T& new_value);

    /**
     * Get last known buffered value.
     *
     * @return
     */
    const T& value() const;

protected:
    AdsClient* client_; ///< Pointer to active TwinCAT connection

    std::string name_; ///< Remote variable name

    unsigned long handle_; ///< Variable handle

    T value_; ///< Last known remote value
};

// Load implementation of this template class (cannot be compiled like regular .cpp)
#include "AdsSymbol_impl.h"

#endif //ADSCLIENT_ADSSYMBOL_H
