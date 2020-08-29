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
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/evp.h>
#include <stdio.h>
#include <ostream>
#include <fstream>
#include <stdlib.h>

enum class CallResult {
    NOT_DEFINED,
    SIGNATURE_THE_SAME,
    SIGNATURE_NOT_THE_SAME,
    WRITE_PRV_KEY_FAIL,
    WRITE_PUB_KEY_FAIL,
    WRITE_KEYS_SUCCESS,
    NO_PRV_KEY_TO_REMOVE,
    NO_PUB_KEY_TO_REMOVE,
    KEY_REMOVE_SUCCESS,
    TRIED_TO_READ_NOT_EXISTING_PRV_KEY,
    SIGN_SUCCESS,
};

class Executor {
private:
    // methods for crypto handler

    std::shared_ptr<Parser> parser;
    std::shared_ptr<RsaKeyFileIOInterface> interface;
    std::unique_ptr<OpenSSLHandler> openSSLHandler;
    CallResult result {CallResult::NOT_DEFINED};


public:
    Executor(std::shared_ptr<Parser> parser) : parser(parser) {
        interface = std::make_shared<RsaKeyFileIOInterface>();
        openSSLHandler = std::make_unique<OpenSSLHandler>();
        ERR_load_CRYPTO_strings();
        OpenSSL_add_all_algorithms();
        OpenSSL_add_all_ciphers();
        OpenSSL_add_all_digests();
    }

    CallResult getResult() {
        return result;
    }

    std::shared_ptr<RsaKeyFileIOInterface> getCurrentInterface() {
        return interface;
    }
//    ~Executor() { free(currentlyEncryptedMsg); }
    void execute();
};


#endif //KEYREPO_EXECUTOR_H
