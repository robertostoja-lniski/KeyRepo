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

#define VERBOSE 0
#define MAX_KEY_LEN 16 * 4096
#define MIN_KEY_LEN 1

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
        if(keyLen <= 0) {
            throw std::runtime_error("Key cannot be negative");
        }

        if(keyLen > MAX_KEY_LEN) {
            throw std::runtime_error("Keys with len greater that "
                    + std::to_string(MAX_KEY_LEN) + " are not supperted");
        }

        std::string pubKeyPath = createKeyStatement->pubKeyPath;
        std::string prvKeyIdPath = createKeyStatement->privateKeyIdFile;
        auto r = openSSLHandler->createKey(keyLen, pubKeyPath, prvKeyIdPath);

        auto ret = interface->writePrivateKeyToFile(prvKeyIdPath, "wb", r.get());
        if(ret == -1) {
            result = CallResult::WRITE_PRV_KEY_FAIL;
            return;
        }

        ret = interface->writePublicKeyToFile(pubKeyPath, "wb", r.get());
        if(ret == 1) {
            result = CallResult::WRITE_PUB_KEY_FAIL;
            return;
        }

        result = CallResult::WRITE_KEYS_SUCCESS;


    } else if (auto signStatement = std::dynamic_pointer_cast<SignStatement>(statement)) {
        auto fileToBeSigned = signStatement->filePathToFileToBeSigned;
        auto prvKeyPath = signStatement->filePathToPrvKeyId;
        auto signatureOutput = signStatement->signatureOutput;

        auto messageToSign = interface->readMessageFromFile(fileToBeSigned);
        RSA* rsaPrv = interface->readPrivateKeyFromFile(prvKeyPath);

        if(rsaPrv == nullptr) {
            result = CallResult::TRIED_TO_READ_NOT_EXISTING_PRV_KEY;
            return;
        }

        auto encryptedMessage = openSSLHandler->sign(rsaPrv, messageToSign);

        if(VERBOSE) {
            std::cout << "encrypted msg is " << encryptedMessage << std::endl;
        }

        interface->writeToFile(signatureOutput, encryptedMessage);
        RSA_free(rsaPrv);
        result = CallResult::SIGN_SUCCESS;

    } else if (auto checkSignatureStatement = std::dynamic_pointer_cast<CheckSignatureStatement>(statement)) {

        auto filePathToFileToBeChecked = checkSignatureStatement->filePathToFileToBeChecked;
        auto filePathToPublicKey = checkSignatureStatement->filePathToPublicKey;
        auto signatureInput = checkSignatureStatement->signatureInput;

        std::string messageToCheck = interface->readMessageFromFile(filePathToFileToBeChecked);
        if(VERBOSE) {
            std::cout << messageToCheck << '\n';
        }

        auto messageToCheckHash = interface->readMessageFromFile(signatureInput);
        if(VERBOSE) {
            std::cout << messageToCheckHash << " it was encrypted msg\n";
        }

        try {
            RSA* rsaPub = interface->readPublicKeyFromFile(filePathToPublicKey);
            openSSLHandler->checkSignature(rsaPub, messageToCheckHash, messageToCheck);
            result = CallResult::SIGNATURE_THE_SAME;

        } catch(std::exception &e) {
            result = CallResult::SIGNATURE_NOT_THE_SAME;
        }

        RSA_free(rsaPub);

    } else if (auto deleteKeyStatement = std::dynamic_pointer_cast<DeleteKeyStatement>(statement)) {

        auto filePathToPrivateKeyId = deleteKeyStatement->privateKeyIdPath;
        auto filePathToPublicKey = deleteKeyStatement->fileToPublicKey;

        if(interface->removePrivateKey(filePathToPrivateKeyId) == -1) {
            result = CallResult::NO_PRV_KEY_TO_REMOVE;
            return;
        }

        if(interface->removePublicKey(filePathToPublicKey) == -1){
            result = CallResult::NO_PUB_KEY_TO_REMOVE;
            return;
        }

        result = CallResult::KEY_REMOVE_SUCCESS;
    }
}



#pragma clang diagnostic pop