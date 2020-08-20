#include <iostream>
#include "../include/SyntaxAnalyser.h"
#include "../include/Parser.h"
#include "../include/TerminalEmulation.h"
#include "../include/Executor.h"

int main(int argc, char** argv) {

    std::cout << argc << std::endl;
    if(argc > 1) {
        auto syntaxAnalyser = std::make_shared<SyntaxAnalyser>(argc, argv);
        auto parser = std::make_shared<Parser>(syntaxAnalyser);
        auto executor = std::make_shared<Executor>(parser);
        executor->execute();
        exit(0);
    }

    std::cout << "Demo params used\n";
    {
        std::vector<std::string> input {
                "program",
                "create-key",
                "RSA",
                "2048",
                "/home/robert/Desktop/public.pem",
                "/home/robert/Desktop/private.pem",
        };
        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        argc = emulatedTerminalArgs.argc;
        argv = emulatedTerminalArgs.argv;


        auto syntaxAnalyser = std::make_shared<SyntaxAnalyser>(argc, argv);
        auto parser = std::make_shared<Parser>(syntaxAnalyser);
        auto executor = std::make_shared<Executor>(parser);
        executor->execute();
        exit(0);
    }
    {
        std::vector<std::string> input {
                "program",
                "sign",
                "/home/robert/Desktop/file.txt",
                "/home/robert/Desktop/private.pem",
                "/home/robert/Desktop/signature.txt",
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
    {
        std::vector<std::string> input {
                "program",
                "check-signature",
                "/home/robert/Desktop/file.txt",
                "/home/robert/Desktop/public.pem",
                "/home/robert/Desktop/signature.txt",
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
}
