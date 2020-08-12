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
        std::cout << "No args given\n";
    }

public:
    SyntaxAnalyser(int argc, char** argv) : it(argc, argv) {
        // ommit
        if(argc == 1) {
            printHelp();
        } else {
            it++;
        }
    }
    Token getNextToken();
};


#endif //KEYREPO_SYNTAXANALYSER_H
