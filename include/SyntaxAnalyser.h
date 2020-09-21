//
// Created by robert on 17.05.2020.
//

#ifndef KEYREPO_SYNTAXANALYSER_H
#define KEYREPO_SYNTAXANALYSER_H

#include <string>
#include "Token.h"
#include <memory>
#include <functional>
#include <map>
#include <iostream>

class SyntaxAnalyser {

private:
    class InputIterator {
    private:
        int currentIdx {0};
        int endIdx {0};
        char** currentArgs {nullptr};

    public:
        InputIterator(int endIdx, char** argv) : endIdx(endIdx), currentArgs(argv) {}
        std::string operator*() {
            if(currentIdx >= endIdx) {
                throw std::runtime_error("Tried to access end value");
            }
            return currentArgs[currentIdx];
        }
        void operator++(int) {
            if(currentIdx == endIdx) {
                throw std::runtime_error("Iterator out of bound");
            }
            currentIdx++;
        }
        int begin() { return 0; }
        int end() { return endIdx; }
    };
    InputIterator it;
    std::string getNextAndRead();
    void printHelp() {
        std::cout << "key_repo - Tool used for key storage\n";
        std::cout << "USAGE:\n";
        std::cout << "\tKEY MANAGEMENT:\n";
        std::cout << "\t\tkey_repo create-key         path_to_private_key_id  path_to_public_key  key_length key_type [overwrite]\n";
        std::cout << "\t\tkey_repo get-private-key    path_to_private_key_id  path_to_file_with_key_value             [overwrite]\n";
        std::cout << "\t\tkey_repo delete-key         path_to_private_key_id  path_to_public_key\n";
        std::cout << "\tKEY OPERATIONS:\n";
        std::cout << "\t\tkey_repo sign               path_to_private_key     path_to_file_to_be_signed path_to_signature       [overwrite]\n";
        std::cout << "\t\tkey_repo check-signature    path_to_public_key      path_to_signed_file       path_to_signature\n";
        std::cout << "\t\tkey_repo encrypt-file       path_to_private_key     path_to_file              path_to_encrypted_file  [overwrite]\n";
        std::cout << "\t\tkey_repo decrypt-file       path_to_public_key      path_to_encrypted_file    path_to_decrypted_file  [overwrite]\n";
    }

public:
    SyntaxAnalyser(int argc, char** argv) : it(argc, argv) {
        // ommit
        if(argc == 1) {
            printHelp();
            exit(0);
        } else {
            it++;
        }
    }
    Token getNextToken();
};


#endif //KEYREPO_SYNTAXANALYSER_H
