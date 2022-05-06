#ifndef ADSCLIENT_ADSCLIENT_IMP_H
#define ADSCLIENT_ADSCLIENT_IMP_H

#include "AdsClient.h"
#include "AdsSymbol.h"

template<typename T>
AdsSymbol<T> AdsClient::getSymbolByName(const std::string& name) {
    return AdsSymbol<T>(*this, name);
}

#endif //ADSCLIENT_ADSCLIENT_IMP_H
