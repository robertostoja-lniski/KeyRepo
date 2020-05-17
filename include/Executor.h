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
    void createKey(const std::string& algorithm, int ketLen, const std::string& pubKeyPath, const std::string& prvKeyIdPath);
    void sign(const unsigned char* Msg, size_t MsgLen, unsigned char** EncMsg, size_t* MsgLenEnc);

public:
    Executor(std::shared_ptr<Parser> parser) : parser(parser) {}
    void execute();
};


#endif //KEYREPO_EXECUTOR_H
