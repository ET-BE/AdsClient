#ifndef ADSCLIENT_ADSSYMBOL_H
#define ADSCLIENT_ADSSYMBOL_H

#include <string>

#include "AdsClient.h"

/**
 * Wrapper for remote ADS symbol.
 *
 * Any handle acquisition and release is done automatically.
 */
template <typename T>
class AdsSymbol {
public:

    struct Index {
        unsigned long group;
        unsigned long offset;
    };

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

protected:
    AdsClient* client_; ///< Pointer to active TwinCAT connection

    std::string name_; ///< Remote variable name

    unsigned long handle_; ///< Variable handle
    Index index_; ///< Variable indices

    T value_; ///< Last known remote value
};

// Load implementation of this template class (cannot be compiled like regular .cpp)
#include "AdsSymbol_impl.h"

#endif //ADSCLIENT_ADSSYMBOL_H
