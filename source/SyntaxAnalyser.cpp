//
// Created by robert on 17.05.2020.
//

#include <iostream>
#include <cstring>
#include "../include/SyntaxAnalyser.h"

Token SyntaxAnalyser::getNextToken() {
    auto value = getNextAndRead();
    return {value};
}
std::string SyntaxAnalyser::getNextAndRead() {
    std::string toReturn;
    toReturn = *it;
    it++;
    return toReturn;
}
