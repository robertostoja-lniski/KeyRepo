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
    auto toReturn = *it;
    try {
        it++;
    } catch(std::exception &e) {
        std::cout << "End of args";
        exit(1);
    }
    return toReturn;
}
