//
// Created by robert on 25.05.2020.
//

#ifndef KEYREPO_KEYIOINTERFACES_H
#define KEYREPO_KEYIOINTERFACES_H
#include <memory>
#include <stdio.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/evp.h>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <stdlib.h>

// to be changed to template
// and to abstract interface inheritance
// currently (may 2020) I am working on the most effective solution
class RsaKeyFileIOInterface{
private:
    FILE* getFileStructFromPath(std::string, std::string);
    RSA* readPrivateKeyFromFpAndClose(FILE**);
    RSA* readPublicKeyFromFpAndClose(FILE**);
    void printFile(std::string filepath);
public:
    RSA* readPublicKeyFromFile(std::string filepath);
    RSA* readPrivateKeyFromFile(std::string filepath);
    void writePublicKeyToFile(std::string filepath, std::string modes, RSA*);
    void writePrivateKeyToFile(std::string filepath, std::string modes, RSA*);
    std::string readMessageFromFile(std::string filepath);
    void writeToFile(std::string filepath, std::string data);
};

#endif
