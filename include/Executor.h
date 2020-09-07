//
// Created by robert on 17.05.2020.
//

#ifndef KEYREPO_EXECUTOR_H
#define KEYREPO_EXECUTOR_H

#include <memory>
#include "Parser.h"
#include "Statement.h"
#include "KeyIOInterfaces.h"
#include "OpenSSLHandler.h"
#include "KernelEmulation.h"
#include <stdio.h>

#if __APPLE__
#include "openssl/rsa.h"
#include "openssl/pem.h"
#include "openssl/evp.h"
#else
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/evp.h>
#endif


#include <stdio.h>
#include <ostream>
#include <fstream>
#include <stdlib.h>

class Executor {
private:
    // methods for crypto handler
    std::shared_ptr<Parser> parser;
    std::shared_ptr<RsaKeyFileIOInterface> interface;
    std::unique_ptr<OpenSSLHandler> openSSLHandler;

public:
    Executor(std::shared_ptr<Parser> parser) : parser(parser) {
        interface = std::make_shared<RsaKeyFileIOInterface>();
        openSSLHandler = std::make_unique<OpenSSLHandler>();
        ERR_load_CRYPTO_strings();
        OpenSSL_add_all_algorithms();
        OpenSSL_add_all_ciphers();
        OpenSSL_add_all_digests();
    }

    std::shared_ptr<RsaKeyFileIOInterface> getCurrentInterface() {
        return interface;
    }
//    ~Executor() { free(currentlyEncryptedMsg); }
    std::string execute();
};


#endif //KEYREPO_EXECUTOR_H
