#include <iostream>
#include "../include/SyntaxAnalyser.h"
#include "../include/Parser.h"
#include "../include/TerminalEmulation.h"
#include "../include/Executor.h"

int main(int argc, char** argv) {

    auto syntaxAnalyser = std::make_shared<SyntaxAnalyser>(argc, argv);
    auto parser = std::make_shared<Parser>(syntaxAnalyser);
    auto executor = std::make_shared<Executor>(parser);

    try {
        executor->execute();
    } catch(std::exception &e) {
        std::cout << e.what();
        executor->printHelp();
    }

}