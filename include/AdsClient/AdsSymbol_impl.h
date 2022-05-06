#include "AdsClient/AdsSymbol.h"

template <typename T>
AdsSymbol<T>::AdsSymbol(AdsClient& client, const std::string& name) {
    client_ = &client;
    name_ = name;

    handle_ = client_->getVariableByName(name_);
}

template <typename T>
const T& AdsSymbol<T>::read() {

    client_->read(handle_,
                  (void*)&value_,
                  sizeof(value_));

    return value();
}

template <typename T>
bool AdsSymbol<T>::write(const T& new_value) {

    auto result = client_->write(handle_,
                                 (void*)&new_value,
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
