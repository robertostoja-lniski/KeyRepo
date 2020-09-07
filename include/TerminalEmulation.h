//
// Created by robert on 17.05.2020.
//

#ifndef KEYREPO_TERMINALEMULATION_H
#define KEYREPO_TERMINALEMULATION_H

#include <vector>
#include "string.h"

class TerminalEmulation {
private:
    struct Args {
        char** argv {nullptr};
        int argc {0};
        Args() = default;
    };
    Args args;
    std::vector<std::string> input;
    void initialiseInputDataArray() {
        args.argc = input.size();
        auto size = args.argc * sizeof(*args.argv);
        args.argv = (char** )malloc(size);
        if(args.argv == nullptr) {
            throw std::runtime_error("Cannot allocate memory for test input");
        }
    }

    void copyStringInputToCharArrayInput() {
        for(int i = 0; i < input.size(); i++) {
            auto argToCharPtr = input[i].c_str();
            auto signLen = strlen(argToCharPtr) + 1;
            auto size = sizeof(**args.argv);
            args.argv[i] = (char* )malloc(signLen * sizeof(**args.argv));
            if(args.argv[i] == nullptr) {
                throw std::runtime_error("Cannot allocate memory for test input");
            }
            strncpy(args.argv[i], argToCharPtr, signLen);
        }
    }
public:
    TerminalEmulation(std::vector<std::string> emulatedInput) : input(emulatedInput){
        initialiseInputDataArray();
        copyStringInputToCharArrayInput();
    }
    ~TerminalEmulation() {
        for(int i = 0; i < args.argc; i ++) {
            auto sign = args.argv[i];
            free (args.argv[i]);
        }
        if(args.argv != nullptr) {
            free(args.argv);
        }
    }
    Args getArgs() {
        return args;
    }
};

#endif //KEYREPO_TERMINALEMULATION_H
