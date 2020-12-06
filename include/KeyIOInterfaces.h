//
// Created by robert on 25.05.2020.
//

#ifndef KEYREPO_KEYIOINTERFACES_H
#define KEYREPO_KEYIOINTERFACES_H
#include <memory>
#include <stdio.h>


#if __APPLE__
#include "openssl/rsa.h"
#include "openssl/pem.h"
#include "openssl/evp.h"
#include "openssl/ossl_typ.h"
#include <experimental/filesystem>
#else
#include <boost/filesystem.hpp>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/evp.h>
#endif

#include <stdio.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <stdlib.h>
#include "KernelEmulation.h"
// to be changed to template
// and to abstract interface inheritance
// currently (may 2020) I am working on the most effective solution
class RsaKeyFileIOInterface {
private:
    RSA* readPublicKeyFromFpAndClose(FILE**);
    void printFile(std::string filepath);
public:
    RsaKeyFileIOInterface() = default;

    FILE* getFileStructFromPath(std::string, std::string);
    RSA* readPublicKeyFromFile(std::string filepath);
    RSA* readPrivateKeyFromFile(std::string filepath);
    void writePublicKeyToFile(std::string filepath, std::string modes, RSA*, bool overwrite);
    void writePrivateKeyToFile(std::string filepath, std::string modes, RSA*, bool overwrite);
    std::string readFromFile(std::string filepath);
    void writeToFile(std::string filepath, std::string data, bool overwrite);

    void removePrivateKey(std::string privateKeyPath);
    void removePublicKey(std::string publicKeyPath);

    int getKeyMode(std::string privateKeyPath);
    void changeKeyMode(std::string privateKeyPath, int changedMode);

    int getCurrentKeyNum() {
        return getCurrentKeyNumFromEmulation();
    }

    void throwIfOverwriteForbidden(std::string filepath, bool overwrite);
    void throwIfCannotRemoveFile(std::string filepath);

    std::string getPrivateKey(std::string filepathWithPrvKeyId);

};

#endif
