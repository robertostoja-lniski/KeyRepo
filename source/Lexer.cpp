//
// Created by robert on 17.05.2020.
//

#include <iostream>
#include <cstring>
#include "../include/Lexer.h"

Token Lexer::getNextToken() {
    auto value = getNextAndRead();
    return {value};
}
std::string Lexer::getNextAndRead() {
    std::string toReturn;
    toReturn = *it;
    it++;
    return toReturn;
}
