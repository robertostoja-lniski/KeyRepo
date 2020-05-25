//
// Created by robert on 17.05.2020.
//

#ifndef KEYREPO_EXECUTOR_H
#define KEYREPO_EXECUTOR_H

#include <memory>
#include "Parser.h"
#include "Statement.h"
#include <stdio.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/evp.h>
#include <stdio.h>
#include <stdlib.h>
class Executor {
private:
    bool checkSignature( RSA*, std::string hash, std::string msg);
    std::string sign( RSA* rsa, std::string toSign);
    std::shared_ptr<Parser> parser;
    size_t currentlyEncryptedMsgLen {0};
    unsigned char* encMessage;
    size_t encMessageLength;
    std::string encryptedMessage;
    void createKey(const std::string& algorithm, int ketLen, const std::string& pubKeyPath, const std::string& prvKeyIdPath);
    void printFile(std::string);
    FILE* getFileStructFromPath(std::string, std::string);
    RSA* readPrivateKeyFromFpAndClose(FILE**);
    RSA* readPublicKeyFromFpAndClose(FILE**);
    RSA* readPublicKeyFromFile(std::string filepath);
    RSA* readPrivateKeyFromFile(std::string filepath);
    void writePublicKeyToFile(std::string filepath, std::string modes, RSA*);
    void writePrivateKeyToFile(std::string filepath, std::string modes, RSA*);
    void assignRsaKeyToPtr(size_t keyLen, RSA**);
    // temporary solution
    std::string readMessageFromFile(std::string filepath);

public:
    Executor(std::shared_ptr<Parser> parser) : parser(parser) {
        ERR_load_CRYPTO_strings();
        OpenSSL_add_all_algorithms();
        OpenSSL_add_all_ciphers();
        OpenSSL_add_all_digests();
    }
//    ~Executor() { free(currentlyEncryptedMsg); }
    void execute();
};


#endif //KEYREPO_EXECUTOR_H
