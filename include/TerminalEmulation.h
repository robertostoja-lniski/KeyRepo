//
// Created by robert on 17.05.2020.
//

#ifndef KEYREPO_TERMINALEMULATION_H
#define KEYREPO_TERMINALEMULATION_H
#include "string.h"
struct Args {
    char** argv {nullptr};
    int argc;
    Args() = default;
    ~Args() {
        for(int i = 0; i < argc; i ++) {
            free (argv[i]);
        }
        free(argv);
    }
};
class TerminalEmulation {
private:
    Args args;
    std::vector<std::string> input;
    void initialiseInputDataArray() {
        int argc = (int)(input.size() + 1);
        auto argv = (char** )malloc(argc * sizeof(**args.argv));
        if(argv == nullptr) {
            throw std::runtime_error("Cannot allocate memory for test input");
        }
        args.argc = argc;
        args.argv = argv;
    }

    void copyStringInputToCharArrayInput() {
        for(int i = 0; i < input.size(); i++) {
            auto argToCharPtr = input[i].c_str();
            auto signLen = strlen(argToCharPtr) + 1;
            args.argv[i] = (char* )malloc(signLen * sizeof(**args.argv));
            strncpy(args.argv[i], argToCharPtr, signLen);
        }
    }
public:
    TerminalEmulation(std::vector<std::string> emulatedInput) : input(emulatedInput){
        initialiseInputDataArray();
        copyStringInputToCharArrayInput();
    }
    Args getArgs() {
        return args;
    }
};

#endif //KEYREPO_TERMINALEMULATION_H
