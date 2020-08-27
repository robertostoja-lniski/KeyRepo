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
    auto privateKeyIdFile = syntaxAnalyser->getNextToken().value;
    int numericKeySize;

    if(keySize.length() > 9) {
        throw std::runtime_error("Far too long key len");
    }

    try {
        numericKeySize = std::stoi(keySize);
        if(numericKeySize < 0) {
            throw std::runtime_error("Negative size or id");
        }
    } catch(std::exception &e) {
        std::cout << "Key size and id have to be positive integers\n";
    }
    CreateKeyStatement createKeyStatement(algorithm, numericKeySize, pubKeyPath, privateKeyIdFile);
    currentParsedStatement = std::make_shared<CreateKeyStatement>(createKeyStatement);
}

void Parser::generatePrivateKeyOption() {
    auto privateKeyId = syntaxAnalyser->getNextToken().value;
    auto pathToStorePrivateKey = syntaxAnalyser->getNextToken().value;
    int numericPrivateKeyId;

    try {
        numericPrivateKeyId = std::stoi(privateKeyId);
        if(numericPrivateKeyId < 0) {
            throw std::runtime_error("Negative id");
        }
    } catch(std::exception &e) {
        std::cout << "Key id has to be a positive number\n";
    }
    GetPrivateKeyStatement getPrivateKeyStatement(numericPrivateKeyId, pathToStorePrivateKey);
    currentParsedStatement = std::make_shared<GetPrivateKeyStatement>(getPrivateKeyStatement);
}
void Parser::generateDeleteKeyOption() {
    auto privateKeyId = syntaxAnalyser->getNextToken().value;
    auto pubKeyPath = syntaxAnalyser->getNextToken().value;

    DeleteKeyStatement deleteKeyStatement(privateKeyId, pubKeyPath);
    currentParsedStatement = std::make_shared<DeleteKeyStatement>(deleteKeyStatement);
}
void Parser::generateSignOption() {
    auto fileToBeSigned = syntaxAnalyser->getNextToken().value;
    auto privateKeyFilePath = syntaxAnalyser->getNextToken().value;
    auto signatureOutput = syntaxAnalyser->getNextToken().value;

    SignStatement signStatement(fileToBeSigned, privateKeyFilePath, signatureOutput);
    currentParsedStatement = std::make_shared<SignStatement>(signStatement);
}
void Parser::generateCheckSignatureOption() {
    auto fileToBeChecked = syntaxAnalyser->getNextToken().value;
    auto pubKeyPath = syntaxAnalyser->getNextToken().value;
    auto signatureInput = syntaxAnalyser->getNextToken().value;

    CheckSignatureStatement checkSignatureStatement(fileToBeChecked, pubKeyPath, signatureInput);
    currentParsedStatement = std::make_shared<CheckSignatureStatement>(checkSignatureStatement);
}
void Parser::generateEncryptFileOption() {
    auto fileToBeSigned = syntaxAnalyser->getNextToken().value;
    auto privateKeyId = syntaxAnalyser->getNextToken().value;
    int numericPrivateKeyId;

    try {
        numericPrivateKeyId = std::stoi(privateKeyId);
        if(numericPrivateKeyId < 0) {
            throw std::runtime_error("Negative size or id");
        }
    } catch(std::exception &e) {
        std::cout << "Key size and id have to be positive integers\n";
    }
    EncryptFileStatement encryptFileStatement(fileToBeSigned, numericPrivateKeyId);
    currentParsedStatement = std::make_shared<EncryptFileStatement>(encryptFileStatement);
}
