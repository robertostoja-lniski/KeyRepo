//
// Created by Robert Ostoja-Lniski on 1/1/21.
//

#ifndef KEYREPO_TIMECALC_H
#define KEYREPO_TIMECALC_H

#include <iostream>
#include <vector>
#include <random>
#include <functional>
#include <algorithm>

extern "C" {
    #include "KernelEmulation.h"
};

class TimeCalc {
public:
    virtual void writeKeyTime(unsigned int trials) = 0;
    virtual void readKeyTime(unsigned int trials) = 0;
    virtual void removeKeyTime(unsigned int trials) = 0;
    virtual void getKeySizeTime(unsigned int trials) = 0;
    virtual void getCurrentKeyNumFromEmulationTime(unsigned int trials) = 0;
    virtual void unusedMapRowOptimisation(unsigned int trials) = 0;
    virtual ~TimeCalc() = 0;
};


class EmulationTimeCalc : public TimeCalc {
private:
    const uint64_t testedSize = 4096;
public:
    void writeKeyTime(unsigned int trials);
    void readKeyTime(unsigned int trials);
    void removeKeyTime(unsigned int trials);
    void getKeySizeTime(unsigned int trials);
    void getCurrentKeyNumFromEmulationTime(unsigned int trials);
    void unusedMapRowOptimisation(unsigned int trials);
    ~EmulationTimeCalc() = default;
};


#endif //KEYREPO_TIMECALC_H
