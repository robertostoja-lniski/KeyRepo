//
// Created by robert on 17.05.2020.
//

#ifndef KEYREPO_EXECUTOR_H
#define KEYREPO_EXECUTOR_H

#include <memory>
#include "Parser.h"
#include "Statement.h"
#include "KeyIOInterfaces.h"
#include <stdio.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/evp.h>
#include <stdio.h>
#include <ostream>
#include <fstream>
#include <stdlib.h>
class Executor {
private:
    // methods for crypto handler
    bool checkSignature( RSA*, std::string hash, std::string msg);
    std::string sign( RSA* rsa, std::string toSign);
    void createKey(const std::string& algorithm, int ketLen, const std::string& pubKeyPath, const std::string& prvKeyIdPath);
    void assignRsaKeyToPtr(size_t keyLen, RSA**);

    std::shared_ptr<Parser> parser;
    std::unique_ptr<RsaKeyFileIOInterface> interface;


public:
    Executor(std::shared_ptr<Parser> parser) : parser(parser) {
        interface = std::make_unique<RsaKeyFileIOInterface>();
        ERR_load_CRYPTO_strings();
        OpenSSL_add_all_algorithms();
        OpenSSL_add_all_ciphers();
        OpenSSL_add_all_digests();
    }
//    ~Executor() { free(currentlyEncryptedMsg); }
    void execute();
};


#endif //KEYREPO_EXECUTOR_H
