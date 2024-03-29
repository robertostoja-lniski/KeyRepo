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
#include <chrono>


extern "C" {
    #include "KeyRepoSyscallWrapper.h"
};

class TimeCalc {
public:
    virtual void writeKeyTime(unsigned int trials) = 0;
    virtual void readKeyTime(unsigned int trials) = 0;
    virtual void removeKeyTime(unsigned int trials) = 0;
    virtual void cacheTest(unsigned int trials) = 0;
    virtual void freedSlotTest(unsigned int trials) = 0;
    virtual void getKeySizeTime(unsigned int trials) = 0;
    virtual void getKeyModeTime(unsigned int trials) = 0;
    virtual void setKeyModeTime(unsigned int trials) = 0;
    virtual void getKeyNumTime(unsigned int trials) = 0;
    virtual uint64_t unusedMapRowOptimisation(unsigned int trials) = 0;
    virtual void defragmentationOptimisation(unsigned int trials, unsigned int scenario_id) = 0;
    virtual ~TimeCalc() = 0;
    virtual void setTestedSize(uint64_t size) = 0;
};


class EmulationTimeCalc : public TimeCalc {
private:
    uint64_t testedSize {4096};
public:
    void setTestedSize(uint64_t size);
    void writeKeyTime(unsigned int trials);
    void readKeyTime(unsigned int trials);
    void removeKeyTime(unsigned int trials);
    void cacheTest(unsigned int trials);
    void freedSlotTest(unsigned int trials);
    void getKeySizeTime(unsigned int trials);
    void getKeyModeTime(unsigned int trials);
    void setKeyModeTime(unsigned int trials);
    void getKeyNumTime(unsigned int trials);
    uint64_t unusedMapRowOptimisation(unsigned int trials);
    void defragmentationOptimisation(unsigned int trials, unsigned int scenario_id);
    ~EmulationTimeCalc() = default;

    void test(unsigned int trials);
};


#endif //KEYREPO_TIMECALC_H
