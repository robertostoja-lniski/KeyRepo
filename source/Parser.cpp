//
// Created by robert on 17.05.2020.
//

#include <iostream>
#include "../include/Parser.h"

void Parser::parse() {
    auto currentToken = syntaxAnalyser->getNextToken();
    auto currentValue = currentToken.value;
    if(tokenGenerator.find(currentValue) == tokenGenerator.end()) {
        throw std::runtime_error("Unknown token");
    }
    auto generator = tokenGenerator[currentValue];
    generator();
}

void Parser::generateCreateKeyOption() {
    auto algorithm = syntaxAnalyser->getNextToken().value;
    auto keySize = syntaxAnalyser->getNextToken().value;
    auto pubKeyPath = syntaxAnalyser->getNextToken().value;
    auto privateKeyId = syntaxAnalyser->getNextToken().value;
    int numericKeySize;
    int numericPrivateKeyId;

    try {
        numericKeySize = std::stoi(keySize);
        numericPrivateKeyId = std::stoi(privateKeyId);
        if(numericKeySize < 0 || numericPrivateKeyId < 0) {
            throw std::runtime_error("Negative size or id");
        }
    } catch(std::exception &e) {
        std::cout << "Key size and id have to be positive integers\n";
    }
    CreateKeyStatement createKeyStatement(algorithm, numericKeySize, pubKeyPath, numericPrivateKeyId);
    currentParsedStatement = std::make_shared<CreateKeyStatement>(createKeyStatement);
}