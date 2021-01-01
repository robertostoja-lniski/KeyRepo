#include <iostream>
#include "../include/TerminalEmulation.h"
#include "../include/Executor.h"
#include "../include/TimeCalc.h"
#include <memory>

int main(int argc, char** argv) {

//    std::unique_ptr<TimeCalc> timeCalc = std::make_unique<EmulationTimeCalc>();
////    timeCalc->writeKeyTime(200);
////    timeCalc->readKeyTime(200);
////    timeCalc->removeKeyTime(200);
////    timeCalc->setKeyModeTime(200);
////    timeCalc->setTestedSize(2048);
////    timeCalc->getKeyModeTime(200);
////    timeCalc->getKeySizeTime(200);
////    timeCalc->getCurrentKeyNumFromEmulationTime(200);
//
//    timeCalc->setTestedSize(2048);
//
//    uint64_t t1 = 0;
//    uint64_t t2 = 0;
//    uint64_t t3 = 0;
//    uint64_t t4 = 0;
//    uint64_t trials = 25;
//    for(int i = 0; i < trials; i++) {
//
//        timeCalc->setTestedSize(2048);
//        map_optimisation = 0;
////        std::cout << "optimisation turned off\n";
//        t1 += timeCalc->unusedMapRowOptimisation(1);
//        std::cout << "... " << (i + 1) * 100 / (4 * trials) << "%" << std::endl;
//        map_optimisation = 1;
////        std::cout << "optimisation turned on\n";
//        t2 += timeCalc->unusedMapRowOptimisation(1);
//        std::cout << "... " << 2 * (i + 1) * 100 / (4 * trials) << "%" << std::endl;
//
//        timeCalc->setTestedSize(4096);
//        map_optimisation = 0;
////        std::cout << "optimisation turned off\n";
//        t3 += timeCalc->unusedMapRowOptimisation(1);
//        std::cout << "... " << 3 * (i + 1) * 100 / (4 * trials) << "%" << std::endl;
//        map_optimisation = 1;
////        std::cout << "optimisation turned on\n";
//        t4 += timeCalc->unusedMapRowOptimisation(1);
//        std::cout << "... " << 4 * (i + 1) * 100 / (4 * trials) << "%" << std::endl;
//    }
//
//    std::cout << "2048 + off: " << t1 / trials << std::endl;
//    std::cout << "2048 + on: " << t2 / trials << std::endl;
//    std::cout << "4096 + off: " << t3 / trials << std::endl;
//    std::cout << "4096 + on: " << t4 / trials << std::endl;
//    std::cout << "2048 gain: " << (double)(t1 - t2) * 100 / (double)t1 << std::endl;
//    std::cout << "4096 gain: " << (double)(t3 - t4) * 100 / (double)t3 << std::endl;
//
//
////    timeCalc->setTestedSize(2048);
////
////    timeCalc->writeKeyTime(200);
////    timeCalc->readKeyTime(200);
////    timeCalc->removeKeyTime(200);
////    timeCalc->setKeyModeTime(200);
////    timeCalc->getKeyModeTime(200);
////    timeCalc->getKeySizeTime(200);
////    timeCalc->getCurrentKeyNumFromEmulationTime(200);
//
////    timeCalc->setTestedSize(2048);
////    std::cout << ">>> key set to " << 2048 << std::endl;
////    for(int i = 1; i <= 3; i++) {
////        remove_fragmentation = 0;
////        std::cout << ">>> optimisation turned off" << std::endl;
////        timeCalc->defragmentationOptimisation(2, i);
////        remove_fragmentation = 1;
////        std::cout << ">>> optimisation turned on" << std::endl;
////        timeCalc->defragmentationOptimisation(2, i);
////    }
////
////    timeCalc->setTestedSize(4096);
////    std::cout << ">>> key set to " << 4096 << std::endl;
////    for(int i = 2; i <= 2; i++) {
////        remove_fragmentation = 0;
////        std::cout << ">>> optimisation turned off" << std::endl;
////        timeCalc->defragmentationOptimisation(10, i);
////        remove_fragmentation = 1;
////        std::cout << ">>> optimisation turned on" << std::endl;
////        timeCalc->defragmentationOptimisation(10, i);
////    }
//
//    return 0;
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