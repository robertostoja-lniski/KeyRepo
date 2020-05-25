#pragma clang diagnostic push
#pragma ide diagnostic ignored "hicpp-signed-bitwise"
//
// Created by robert on 17.05.2020.
//

#include <cstring>
#include <sstream>
#include <fstream>
#include <openssl/evp.h>
#include "../include/Executor.h"
void Executor::execute() {

    parser->parse();
    auto statementStr = parser->getCurrentParsedStatementStr();
    std::cout << "\n" << statementStr;

    auto statement = parser->getCurrentParsedStatement();
    if(auto createKeyStatement = std::dynamic_pointer_cast<CreateKeyStatement>(statement)) {
        // currently only RSA support implemented
        auto algorithm = createKeyStatement->algorithm;
        if(algorithm != "RSA") {
            throw std::runtime_error("Algorithm not supported");
        }
        auto keyLen = createKeyStatement->keyLen;
        std::string pubKeyPath = createKeyStatement->pubKeyPath;
        std::string prvKeyIdPath = createKeyStatement->privateKeyIdFile;
        auto r = openSSLHandler->createKey(keyLen, pubKeyPath, prvKeyIdPath);
        interface->writePublicKeyToFile(pubKeyPath, "wb", r.get());
        interface->writePrivateKeyToFile(prvKeyIdPath, "wb", r.get());

    } else if (auto signStatement = std::dynamic_pointer_cast<SignStatement>(statement)) {
        std::string fileToBeSigned = signStatement->filePathToFileToBeSigned;
        auto messageToSign = interface->readMessageFromFile(fileToBeSigned);
        std::string prvKeyPath = "/home/robert/Desktop/private.pem";
        RSA* rsaPrv = interface->readPrivateKeyFromFile(prvKeyPath);
        auto encryptedMessage = openSSLHandler->sign(rsaPrv, messageToSign);
        std::cout << encryptedMessage << std::endl;
        interface->writeToFile("/home/robert/Desktop/signature.txt", encryptedMessage);
        RSA_free(rsaPrv);

    } else if (auto checkSignatureStatement = std::dynamic_pointer_cast<CheckSignatureStatement>(statement)) {
        auto filePathToFileToBeChecked = checkSignatureStatement->filePathToFileToBeChecked;
        auto filePathToPublicKey = checkSignatureStatement->filePathToPublicKey;
        std::string messageToCheck = interface->readMessageFromFile(filePathToFileToBeChecked);
        std::cout << messageToCheck << '\n';
        auto messageToCheckHash = interface->readMessageFromFile("/home/robert/Desktop/signature.txt");
        RSA* rsaPub = interface->readPublicKeyFromFile(filePathToPublicKey);
        std::cout << messageToCheckHash << "it was encrypted msg\n";
        auto ret = openSSLHandler->checkSignature(rsaPub, messageToCheckHash, messageToCheck);
        std::cout << ret << '\n';
        RSA_free(rsaPub);
    }
}



#pragma clang diagnostic pop