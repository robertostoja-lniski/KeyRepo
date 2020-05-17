#include <iostream>
#include "../include/SyntaxAnalyser.h"
#include "../include/Parser.h"
#include "../include/TerminalEmulation.h"
#include "../include/Executor.h"

int main(int argc, char** argv) {
    {

        std::vector<std::string> input {
                "program",
                "create-key",
                "RSA",
                "2048",
                "/home/robert/Desktop/pubkey.pem",
                "/home/robert/Desktop/prvkey.pem",
        };
        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        argc = emulatedTerminalArgs.argc;
        argv = emulatedTerminalArgs.argv;


        auto syntaxAnalyser = std::make_shared<SyntaxAnalyser>(argc, argv);
        auto parser = std::make_shared<Parser>(syntaxAnalyser);
        auto executor = std::make_shared<Executor>(parser);
        executor->execute();
    }

    std::vector<std::string> input {
            "program",
            "sign",
            "/home/abc",
            "312321",
    };

    TerminalEmulation terminalEmulation(input);
    auto emulatedTerminalArgs = terminalEmulation.getArgs();
    argc = emulatedTerminalArgs.argc;
    argv = emulatedTerminalArgs.argv;


    auto syntaxAnalyser = std::make_shared<SyntaxAnalyser>(argc, argv);
    auto parser = std::make_shared<Parser>(syntaxAnalyser);
    auto executor = std::make_shared<Executor>(parser);
    executor->execute();
    return 0;
}
