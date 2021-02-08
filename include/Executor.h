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
extern "C" {
    #include "KeyRepoSyscallWrapper.h"
};

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

enum class SetAction {
    ADD = 0,
    TAKE = 1,
    OVERWRITE_WITH_VALUE = 2,
};

struct ModeSetter {
    int value;
    SetAction action;
};

class ModHandler {

public:
    ModHandler() = default;

    bool isFlagInRightFormat(int checkedFlag) {

        if(checkedFlag > 777) {
            return false;
        }

        int digit = checkedFlag % 10;
        int decDigit = (checkedFlag / 10) % 10;
        int hundredDigit = (checkedFlag / 100) % 10;

        return digit <= 7 && decDigit <= 7 && hundredDigit <= 7;
    }

    ModeSetter stringToModeSetter(std::string flags) {

        // basic edge cases
        if(flags.size() > 7) {
            throw std::runtime_error("ModeHandler: Wrong string flag format");
        }

        if(flags == "000") {
            return {0, SetAction::OVERWRITE_WITH_VALUE};
        }

        int isUser = 0;
        int isGroup = 0;
        int isOther = 0;
        int isRead = 0;
        int isWrite = 0;
        SetAction operation = SetAction::ADD;

        auto found_add = flags.find("+");
        auto found_sub = flags.find("-");
        if(found_add == found_sub && found_add != std::string::npos) {
            throw std::runtime_error("ModeHandler: Wrong string flag format");
        }

        if(found_sub != std::string::npos) {
            operation = SetAction::TAKE;
        }

        auto found = std::min(found_add, found_sub);

        if(found == flags.size() - 1) {
            // only + passed as argument
            throw std::runtime_error("ModeHandler: Wrong string flag format");
        }

        if (found!=std::string::npos && found != 0) {

            for (int i = 0; i < found; i ++) {
                if(flags[i] == 'g') {
                    isGroup++;
                } else if(flags[i] == 'o'){
                    isOther++;
                } else if(flags[i] == 'u') {
                    isUser++;
                } else if(flags[i] == 'a') {
                    isGroup++;
                    isUser++;
                    isOther++;
                } else {
                    throw std::runtime_error("ModeHandler: Wrong string flag format");
                }

                if(isGroup > 1 || isOther > 1 || isUser > 1) {
                    throw std::runtime_error("ModeHandler: Wrong string flag format");
                }
            }

            for(int i = found + 1; i < flags.size(); i++) {
                if(flags[i] == 'r') {
                    isRead++;
                } else if(flags[i] == 'w'){
                    isWrite++;
                } else {
                    throw std::runtime_error("ModeHandler: Wrong string flag format");
                }

                if(isRead > 1 || isWrite > 1) {
                    throw std::runtime_error("ModeHandler: Wrong string flag format");
                }
            }

        } else if (found == 0){

            isUser = 1;
            for(int i = 1; i < flags.size(); i++) {
                if(flags[i] == 'r') {
                    isRead++;
                } else if(flags[i] == 'w'){
                    isWrite++;
                } else {
                    throw std::runtime_error("ModeHandler: Wrong string flag format");
                }
            }

            if(isRead > 1 || isWrite > 1) {
                throw std::runtime_error("ModeHandler: Wrong string flag format");
            }

        } else {

            int intFlags;
            std::istringstream ss(flags);
            ss >> intFlags;
            if (intFlags <= 0 || !isFlagInRightFormat(intFlags)) {
                throw std::runtime_error("ModeHandler: Wrong string flag format");
            }
            return {intFlags, SetAction::OVERWRITE_WITH_VALUE};
        }

        int singleResult = 0;

        if(isRead) {
            singleResult += 4;
        }

        if(isWrite) {
            singleResult += 2;
        }

        int result = 0;

        if(isUser) {
            result += singleResult * 100;
        }

        if(isGroup) {
            result += singleResult * 10;
        }

        if(isOther) {
            result += singleResult;
        }

        return {result, operation};
    }
    int modeSetterToInt(ModeSetter modeSetter, int currentValue) {
        auto action = (int)modeSetter.action;
        if(action == (int)SetAction::OVERWRITE_WITH_VALUE) {
            return modeSetter.value;
        }

        int otherDigit = currentValue % 10;
        int groupDigit = (currentValue / 10) % 10;
        int userDigit = (currentValue / 100) % 10;

        int takeValue = modeSetter.value;
        int takeOtherDigit = takeValue % 10;
        int takeGroupDigit = (takeValue / 10) % 10;
        int takeUserDigit = (takeValue / 100) % 10;

        if(action == (int)SetAction::TAKE) {
            return std::max(0, otherDigit - takeOtherDigit)  +
                std::max(0, groupDigit - takeGroupDigit) * 10 +
                std::max(0, userDigit - takeUserDigit) * 100;
        }

        if(action == (int)SetAction::ADD) {
            return std::min(6, otherDigit + takeOtherDigit) +
                std::min(6,groupDigit + takeGroupDigit) * 10 +
                std::min(6,userDigit + takeUserDigit) * 100;
        }

        throw std::runtime_error("ModeHandler: Unknown action");
    }
    std::string parseIntModesToString(int modes) {
        if(modes <= 7) {
            return "00" + std::to_string(modes);
        }

        if(modes <= 77) {
            return "0" + std::to_string(modes);
        }

        return std::to_string(modes);
    }
};

class Executor {
private:
    // methods for crypto handler
    std::shared_ptr<Parser> parser;
    std::shared_ptr<KeyFileIOInterface> interface;
    std::unique_ptr<OpenSSLHandler> openSSLHandler;

public:
    Executor(std::shared_ptr<Parser> parser) : parser(parser) {
        interface = std::make_shared<KeyPartitionIOInterface>();
        openSSLHandler = std::make_unique<OpenSSLHandler>();
        ERR_load_CRYPTO_strings();
        OpenSSL_add_all_algorithms();
        OpenSSL_add_all_ciphers();
        OpenSSL_add_all_digests();
    }

    static void printHelp() {
        std::cout << "\nkey_repo - Tool used for key storage\n";
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

    std::shared_ptr<KeyFileIOInterface> getCurrentInterface() {
        return interface;
    }
    std::string execute();
};


#endif //KEYREPO_EXECUTOR_H
