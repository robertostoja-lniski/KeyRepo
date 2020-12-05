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

class ModHandler {
private:
    std::vector<std::string> possibleFlags = {
            "r", "w", "rw", "wr"
    };
    std::vector<std::string> possiblePrefixes = {
            " ", "g", "o"
    };
    std::vector<std::string> possibleConcatOperators = {
            "+"
    };

public:
    ModHandler() = default;
    bool isFlagInRightFormat(std::string checkedFlag) {

        for( auto possibleFlag : possibleFlags ) {
            for( auto possiblePrefix : possiblePrefixes ) {
                for( auto possibleConcatOperator : possibleConcatOperators ) {

                    auto currentPotentialFlag = possiblePrefix + possibleConcatOperator + possibleFlag;
                    if(checkedFlag == currentPotentialFlag) {
                        return true;
                    }
                }
            }
        }

        return false;
    }
};

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

    static void printHelp() {
        std::cout << "key_repo - Tool used for key storage\n";
        std::cout << "USAGE:\n";
        std::cout << "\tKEY MANAGEMENT:\n";
        std::cout << "\t\tkey_repo create-key         path_to_private_key_id  path_to_public_key  key_length key_type [overwrite]\n";
        std::cout << "\t\tkey_repo get-private-key    path_to_private_key_id  path_to_file_with_key_value             [overwrite]\n";
        std::cout << "\t\tkey_repo delete-key         path_to_private_key_id  path_to_public_key\n";
        std::cout << "\tKEY OPERATIONS:\n";
        std::cout << "\t\tkey_repo sign               path_to_private_key     path_to_file_to_be_signed path_to_signature       [overwrite]\n";
        std::cout << "\t\tkey_repo check-signature    path_to_public_key      path_to_signed_file       path_to_signature\n";
        std::cout << "\t\tfor encryption and decryption AES of 256 size is currently used\n";
        std::cout << "\t\tkey_repo encrypt-file       path_to_key             path_to_iv                path_to_file              path_to_encrypted_file  [overwrite]\n";
        std::cout << "\t\tkey_repo decrypt-file       path_to_key             path_to_iv                path_to_encrypted_file    path_to_decrypted_file  [overwrite]\n";
    }

    std::shared_ptr<RsaKeyFileIOInterface> getCurrentInterface() {
        return interface;
    }
//    ~Executor() { free(currentlyEncryptedMsg); }
    std::string execute();
};


#endif //KEYREPO_EXECUTOR_H
