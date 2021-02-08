#include <iostream>
#include "../include/TerminalEmulation.h"
#include "../include/Executor.h"
#include "../include/TimeCalc.h"
#include <memory>

int main(int argc, char** argv) {

    std::shared_ptr<Executor> executor;
    try {
        auto lexer = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexer);
        executor = std::make_shared<Executor>(parser);
        executor->execute();
    } catch(std::exception &e) {
        std::cout << e.what();
        executor->printHelp();
    }

}