#include <iostream>
#include "../include/SyntaxAnalyser.h"
#include "../include/Parser.h"
#include "../include/TerminalEmulation.h"

int main(int argc, char** argv) {
    std::vector<std::string> input {
            "program",
            "create-key",
            "SHA",
            "2048",
            "/home/r.ostoja",
            "123"
    };
    TerminalEmulation terminalEmulation(input);
//    return 0;
    auto emulatedTerminalArgs = terminalEmulation.getArgs();
    argc = emulatedTerminalArgs.argc;
    argv = emulatedTerminalArgs.argv;
    auto syntaxAnalyser = std::make_shared<SyntaxAnalyser>(argc, argv);
    auto parser = std::make_unique<Parser>(syntaxAnalyser);
    parser->parse();
    auto statement = parser->getCurrentParsedStatement();

    return 0;
}
