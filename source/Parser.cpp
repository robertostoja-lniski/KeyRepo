//
// Created by robert on 17.05.2020.
//

#include <iostream>
#include "../include/Parser.h"

void Parser::parse() {
    auto currentToken = syntaxAnalyser->getNextToken();
    auto currentValue = currentToken.value;
    if(tokenGenerator.find(currentValue) == tokenGenerator.end()) {
        throw std::runtime_error("Parser: Unknown token");
    }
    auto generator = tokenGenerator[currentValue];
    generator();
}

void Parser::generateCreateKeyOption() {

    auto privateKeyIdFile = syntaxAnalyser->getNextToken().value;
    auto pubKeyPath = syntaxAnalyser->getNextToken().value;
    auto keySize = syntaxAnalyser->getNextToken().value;
    auto algorithm = syntaxAnalyser->getNextToken().value;
    auto password = syntaxAnalyser->getNextToken().value;
    int numericKeySize;

    if(keySize.length() > 9) {
        throw std::runtime_error("Parser: Far too long key len");
    }

    for(auto it = keySize.begin(); it < keySize.end(); it++) {
        if(!isdigit(*it)) {
            throw std::runtime_error("Parser: Error in keyLen - unacceptable symbol");
        }
    }

    numericKeySize = std::stoi(keySize);
    if(numericKeySize < 0) {
        throw std::runtime_error("Parser: Negative size or id");
    }

    CreateKeyStatement createKeyStatement(privateKeyIdFile, pubKeyPath, numericKeySize, algorithm, password);

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
    auto password = syntaxAnalyser->getNextToken().value;

    GetPrivateKeyStatement getPrivateKeyStatement(pathToFileWithPrivateKeyId, pathToStorePrivateKey, password);

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
    auto privateKeyFilePath = syntaxAnalyser->getNextToken().value;
    auto fileToBeSigned = syntaxAnalyser->getNextToken().value;
    auto signatureOutput = syntaxAnalyser->getNextToken().value;
    auto password = syntaxAnalyser->getNextToken().value;

    SignStatement signStatement(privateKeyFilePath, fileToBeSigned, signatureOutput, password);

    try {
        auto overwrite = syntaxAnalyser->getNextToken().value;
        if(overwrite == "overwrite") {
            signStatement.enableOverwrite();
        }
    } catch (std::exception &e) { /* dummy */ }

    currentParsedStatement = std::make_shared<SignStatement>(signStatement);
}
void Parser::generateCheckSignatureOption() {
    auto pubKeyPath = syntaxAnalyser->getNextToken().value;
    auto fileToBeChecked = syntaxAnalyser->getNextToken().value;
    auto signatureInput = syntaxAnalyser->getNextToken().value;

    CheckSignatureStatement checkSignatureStatement(pubKeyPath, fileToBeChecked, signatureInput);
    currentParsedStatement = std::make_shared<CheckSignatureStatement>(checkSignatureStatement);
}

void Parser::generateEncryptFileOption() {
    auto filePathWithKey = syntaxAnalyser->getNextToken().value;
    auto filePathWithIv = syntaxAnalyser->getNextToken().value;
    auto fileToBeEncrypted = syntaxAnalyser->getNextToken().value;
    auto output = syntaxAnalyser->getNextToken().value;

    EncryptFileStatement encryptFileStatement(filePathWithKey, filePathWithIv, fileToBeEncrypted, output);

    try {
        auto overwrite = syntaxAnalyser->getNextToken().value;
        if(overwrite == "overwrite") {
            encryptFileStatement.enableOverwrite();
        }
    } catch (std::exception &e) { /* dummy */ }

    currentParsedStatement = std::make_shared<EncryptFileStatement>(encryptFileStatement);
}

void Parser::generateDecryptFileOption() {
    auto filePathWithKey = syntaxAnalyser->getNextToken().value;
    auto filePathWithIv = syntaxAnalyser->getNextToken().value;
    auto fileToBeDecrypted = syntaxAnalyser->getNextToken().value;
    auto output = syntaxAnalyser->getNextToken().value;

    DecryptFileStatement decryptFileStatement(filePathWithKey, filePathWithIv, fileToBeDecrypted, output);

    try {
        auto overwrite = syntaxAnalyser->getNextToken().value;
        if(overwrite == "overwrite") {
            decryptFileStatement.enableOverwrite();
        }
    } catch (std::exception &e) { /* dummy */ }

    currentParsedStatement = std::make_shared<DecryptFileStatement>(decryptFileStatement);
}

void Parser::generateHelpOption() {

    HelpRequestStatement helpRequestStatement;
    currentParsedStatement = std::make_shared<HelpRequestStatement>(helpRequestStatement);
}

void Parser::generateGetModOption() {
    auto filePathToPrivateKey = syntaxAnalyser->getNextToken().value;
    currentParsedStatement = std::make_shared<GetModStatement>(filePathToPrivateKey);
}

void Parser::generateChangeModOption() {
    auto filePathToPrivateKey = syntaxAnalyser->getNextToken().value;
    auto flags = syntaxAnalyser->getNextToken().value;
    currentParsedStatement = std::make_shared<ChangeModStatement>(filePathToPrivateKey, flags);
}
