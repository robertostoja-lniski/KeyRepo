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
#include <boost/any.hpp>
// to be changed to template
// and to abstract interface inheritance
// currently (may 2020) I am working on the most effective solution

extern "C" {
    #include "KernelEmulation.h"
};


//class ConfigurationManager {
//protected:
//    virtual boost::any getOptionProtected(const std::string& name) = 0;
//public:
//    template<typename T> T getOption(const std::string& name) {
//        return boost::any_cast<T>(getOptionProtected(name));
//    }
//};

class KeyFileIOInterface {
protected:
    virtual boost::any protectedReadPublicKeyFromFile(std::string ) = 0;
    virtual boost::any protectedReadPrivateKeyFromFile(std::string ) = 0;
    virtual void protectedWritePublicKeyToFile(std::string , std::string , boost::any, bool ) = 0;
    virtual void protectedWritePrivateKeyToFile(std::string , std::string , boost::any, bool ) = 0;

public:

    template<typename T> T readPublicKeyFromFile(const std::string& filename) {
        return boost::any_cast<T>(protectedReadPublicKeyFromFile(filename));
    }

    template<typename T> T readPrivateKeyFromFile(const std::string& filename) {
        return boost::any_cast<T>(protectedReadPrivateKeyFromFile(filename));
    }

    template<typename T> T writePublicKeyToFile(std::string filepath , std::string modes, T algorithm, bool overwrite) {
        protectedWritePublicKeyToFile(filepath, modes, boost::any_cast<T>(algorithm), overwrite);
    }

    template<typename T> T writePrivateKeyToFile(std::string filepath, std::string modes, T algorithm, bool overwrite) {
        protectedWritePrivateKeyToFile(filepath, modes, boost::any_cast<T>(algorithm), overwrite);
    }

    virtual std::string readFromFile(std::string ) = 0;
    virtual void writeToFile(std::string , std::string , bool ) = 0;

    virtual void removePrivateKey(std::string ) = 0;
    virtual void removePublicKey(std::string ) = 0;

    virtual int getKeyMode(std::string ) = 0;
    virtual void changeKeyMode(std::string , int ) = 0;

    virtual int getCurrentKeyNum() = 0;

    virtual void throwIfOverwriteForbidden(std::string , bool ) = 0;
    virtual void throwIfCannotRemoveFile(std::string ) = 0;

    virtual std::string getPrivateKey(std::string ) = 0;

    virtual FILE* getFileStructFromPath(std::string, std::string) = 0;

};

class KeyPartitionIOInterface : public KeyFileIOInterface {
protected:
    void printFile(std::string filepath);
    boost::any protectedReadPublicKeyFromFile(std::string );
    boost::any protectedReadPrivateKeyFromFile(std::string );
    void protectedWritePublicKeyToFile(std::string , std::string , boost::any, bool );
    void protectedWritePrivateKeyToFile(std::string , std::string , boost::any, bool );

public:
    KeyPartitionIOInterface() = default;

    template<typename T>
    T readPublicKeyFromFile(std::string filepath) {
        return boost::any_cast<T>(protectedReadPublicKeyFromFile(filepath));
    }

    template<typename T>
    T readPrivateKeyFromFile(std::string filepath) {
        return boost::any_cast<T>(protectedReadPrivateKeyFromFile(filepath));
    }

    template<typename T>
    void writePublicKeyToFile(std::string filepath, std::string modes, T algorithm, bool overwrite) {
        protectedWritePublicKeyToFile(filepath, modes, boost::any_cast<T>(algorithm), overwrite);
    }

    template<typename T>
    void writePrivateKeyToFile(std::string filepath, std::string modes, T algorithm, bool overwrite) {
        protectedWritePrivateKeyToFile(filepath, modes, boost::any_cast<T>(algorithm), overwrite);
    }

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

    FILE* getFileStructFromPath(std::string, std::string);
};


#endif
