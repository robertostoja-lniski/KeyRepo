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

#define AES_256_KEY_SIZE 32
#define AES_BLOCK_SIZE 16

struct Config{
    const char* key;
    const char* iv;
};

struct OpenSSLInterface {
    virtual std::string sign(std::string toSign) = 0;
    virtual bool checkSignature(std::string hash, std::string msg) = 0;
};
struct OpenSSLHandler {
    std::string sign(RSA* r, std::string toSign);
    std::shared_ptr<RSA> createKey(int ketLen);
    bool checkSignature(RSA* r, std::string hash, std::string msg);
    //  void sign(std::string toSign) = 0;

    void encrypt(std::shared_ptr<Config> config, FILE *ifp, FILE *ofp);
    void decrypt(std::shared_ptr<Config> config, FILE *ifp, FILE *ofp);
    void encrypt_or_decrypt(std::shared_ptr<Config> config, FILE *ifp, FILE *ofp, int mode);

    OpenSSLHandler() = default;
};


#endif //KEYREPO_OPENSSLHANDLER_H
