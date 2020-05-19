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
    std::shared_ptr<Parser> parser;
    size_t currentlyEncryptedMsgLen {0};
    char** currentlyEncryptedMsg;
    void createKey(const std::string& algorithm, int ketLen, const std::string& pubKeyPath, const std::string& prvKeyIdPath);
    void sign(const unsigned char* Msg, size_t MsgLen, unsigned char** EncMsg, size_t* MsgLenEnc);
    bool checkSignature(unsigned char *data, size_t dataSize, const char *originalData, size_t originalDataSize);
public:
    Executor(std::shared_ptr<Parser> parser) : parser(parser) {}
    ~Executor() { free(currentlyEncryptedMsg); }
    void execute();
};


#endif //KEYREPO_EXECUTOR_H
