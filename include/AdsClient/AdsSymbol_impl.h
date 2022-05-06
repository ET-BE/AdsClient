#include "AdsClient/AdsSymbol.h"

template <typename T>
AdsSymbol<T>::AdsSymbol(AdsClient& client, const std::string& name) {
    client_ = &client;
    name_ = name;

    handle_ = client_->getVariableByName(name_);

    index_.group = index_.offset = 0;
}

template <typename T>
AdsSymbol<T>::AdsSymbol(AdsClient& client,
                        const unsigned long& index_group,
                        const unsigned long& index_offset) {
    client_ = &client;
    name_ = "";

    index_.group = index_group;
    index_.offset = index_offset;

    handle_ = 0;
}

template <typename T>
const T& AdsSymbol<T>::read() {

    if (handle_) {
        client_->read_by_handle(handle_, (void*) &value_, sizeof(value_));
    } else {
        client_->read(index_.group, index_.offset, (void*) &value_, sizeof(value_));
    }

    return value();
}

template <typename T>
bool AdsSymbol<T>::write(const T& new_value) {

    auto result = client_->write_by_handle(handle_,
                                           (void*) &new_value,
                                           sizeof(new_value));

    if (result) {
        value_ = new_value;
    }

    return result;
}

template <typename T>
const T& AdsSymbol<T>::value() const {
    return value_;
}

template <typename T>
AdsSymbol<T>::~AdsSymbol<T>() {

    if (handle_) {
        client_->releaseVariableHandle(handle_);
    }
}
