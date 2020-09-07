//
// Created by robert on 26.05.2020.
//

#ifndef KEYREPO_OPENSSLHANDLER_H
#define KEYREPO_OPENSSLHANDLER_H

#include <memory>
#if __APPLE__
#include "openssl/rsa.h"
#include "openssl/pem.h"
#include "openssl/evp.h"
#include "openssl/ossl_typ.h"
#else
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/evp.h>
#endif
#include <iostream>
struct OpenSSLInterface {
    virtual std::string sign(std::string toSign) = 0;
//  void sign(std::string toSign) = 0;
    virtual bool checkSignature(std::string hash, std::string msg) = 0;
};
struct OpenSSLHandler {
    std::string sign(RSA* r, std::string toSign);
    std::shared_ptr<RSA> createKey(int ketLen, const std::string& pubKeyPath, const std::string& prvKeyIdPath);
    bool checkSignature(RSA* r, std::string hash, std::string msg);
    OpenSSLHandler() = default;
};


#endif //KEYREPO_OPENSSLHANDLER_H
