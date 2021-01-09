#include <iostream>
#include "../include/TerminalEmulation.h"
#include "../include/Executor.h"
#include "../include/TimeCalc.h"
#include <memory>

int main(int argc, char** argv) {

    std::shared_ptr<Executor> executor;
    try {
        auto syntaxAnalyser = std::make_shared<SyntaxAnalyser>(argc, argv);
        auto parser = std::make_shared<Parser>(syntaxAnalyser);
        executor = std::make_shared<Executor>(parser);
        executor->execute();
    } catch(std::exception &e) {
        std::cout << e.what();
        executor->printHelp();
    }

}