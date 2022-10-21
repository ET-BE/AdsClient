#ifndef ADSCLIENT_ADSCLIENT_IMP_H
#define ADSCLIENT_ADSCLIENT_IMP_H

#include "AdsClient.h"
#include "AdsSymbol.h"

template<typename T>
AdsSymbol<T> AdsClient::getSymbolByName(const std::string& name) {
    return AdsSymbol<T>(*this, name);
}

template<typename T, typename Item>
void erase_safe(T& list, const Item& item) {
    auto it = list.find(item);
    if (it != list.end()) {
        list.erase(it); // If found, remove it
    }
    // Otherwise, do nothing
}

#endif //ADSCLIENT_ADSCLIENT_IMP_H
