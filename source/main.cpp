#include <iostream>
#include "../include/TerminalEmulation.h"
#include "../include/Executor.h"
#include "../include/TimeCalc.h"
#include <memory>

int main(int argc, char** argv) {
    EmulationTimeCalc timeCalc;
    timeCalc.setTestedSize(2048);
    timeCalc.removeKeyTime(200);

}
