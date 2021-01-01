#include <iostream>
#include "../include/TerminalEmulation.h"
#include "../include/Executor.h"
#include "../include/TimeCalc.h"
#include <memory>

int main(int argc, char** argv) {

    std::unique_ptr<TimeCalc> timeCalc = std::make_unique<EmulationTimeCalc>();
    timeCalc->writeKeyTime(200);
    timeCalc->readKeyTime(200);

    remove_fragmentation = 0;
    std::cout << ">>> optimisation turned off" << std::endl;
    timeCalc->removeKeyTime(200);
    remove_fragmentation = 1;
    std::cout << ">>> optimisation turned on" << std::endl;
    timeCalc->removeKeyTime(200);

    map_optimisation = 0;
    std::cout << ">>> optimisation turned off" << std::endl;
    timeCalc->unusedMapRowOptimisation(200);
    map_optimisation = 1;
    std::cout << ">>> optimisation turned on" << std::endl;
    timeCalc->unusedMapRowOptimisation(200);

    timeCalc->getKeySizeTime(200);
    timeCalc->getCurrentKeyNumFromEmulationTime(200);

    return 0;
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