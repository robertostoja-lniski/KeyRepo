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

    numericKeySize = std::stoi(keySize);
    if(numericKeySize < 0) {
        throw std::runtime_error("Negative size or id");
    }

    CreateKeyStatement createKeyStatement(algorithm, numericKeySize, pubKeyPath, privateKeyIdFile);

    try {
        auto overwrite = syntaxAnalyser->getNextToken().value;
        if(overwrite == "overwrite") {
            createKeyStatement.enableOverwrite();
        }
    } catch (std::exception &e) { /* dummy */ }

    currentParsedStatement = std::make_shared<CreateKeyStatement>(createKeyStatement);
}

void Parser::generateGetPrivateKeyOption() {
    auto pathToFileWithPrivateKeyId = syntaxAnalyser->getNextToken().value;
    auto pathToStorePrivateKey = syntaxAnalyser->getNextToken().value;

    GetPrivateKeyStatement getPrivateKeyStatement(pathToFileWithPrivateKeyId, pathToStorePrivateKey);

    try {
        auto overwrite = syntaxAnalyser->getNextToken().value;
        if(overwrite == "overwrite") {
            getPrivateKeyStatement.enableOverwrite();
        }
    } catch (std::exception &e) { /* dummy */ }

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

    try {
        auto overwrite = syntaxAnalyser->getNextToken().value;
        if(overwrite == "overwrite") {
            signStatement.enableOverwrite();
        }
    } catch (std::exception &e) { /* dummy */ }

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
    auto fileToBeEncrypted = syntaxAnalyser->getNextToken().value;
    auto output = syntaxAnalyser->getNextToken().value;
    auto filePathWithPrivateKey = syntaxAnalyser->getNextToken().value;

    EncryptFileStatement encryptFileStatement(fileToBeEncrypted, output, filePathWithPrivateKey);

    try {
        auto overwrite = syntaxAnalyser->getNextToken().value;
        if(overwrite == "overwrite") {
            encryptFileStatement.enableOverwrite();
        }
    } catch (std::exception &e) { /* dummy */ }

    currentParsedStatement = std::make_shared<EncryptFileStatement>(encryptFileStatement);
}

void Parser::generateDecryptFileOption() {
    auto fileToBeDecrypted = syntaxAnalyser->getNextToken().value;
    auto output = syntaxAnalyser->getNextToken().value;
    auto filePathToPublicKey = syntaxAnalyser->getNextToken().value;

    DecryptFileStatement decryptFileStatement(fileToBeDecrypted, output, filePathToPublicKey);

    try {
        auto overwrite = syntaxAnalyser->getNextToken().value;
        if(overwrite == "overwrite") {
            decryptFileStatement.enableOverwrite();
        }
    } catch (std::exception &e) { /* dummy */ }

    currentParsedStatement = std::make_shared<DecryptFileStatement>(decryptFileStatement);
}
