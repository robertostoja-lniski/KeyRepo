#pragma clang diagnostic push
#pragma ide diagnostic ignored "hicpp-signed-bitwise"
//
// Created by robert on 17.05.2020.
//

#include <cstring>
#include <sstream>
#include <fstream>

#include "../include/Executor.h"

#define VERBOSE 0
#define MAX_KEY_LEN 16 * 4096
#define MIN_KEY_LEN 2048

std::string Executor::execute() {
    parser->parse();
    auto statementStr = parser->getCurrentParsedStatementStr();
    if(VERBOSE) {
        std::cout << "\n" << statementStr;
    }

    auto statement = parser->getCurrentParsedStatement();
    if(auto createKeyStatement = std::dynamic_pointer_cast<CreateKeyStatement>(statement)) {
        // currently only RSA support implemented
        auto algorithm = createKeyStatement->algorithm;
        auto overwrite = createKeyStatement->overwrite;

        if(algorithm != "RSA") {
            throw std::runtime_error("Create key: Algorithm not supported");
        }

        auto keyLen = createKeyStatement->keyLen;
        if(keyLen <= 0) {
            throw std::runtime_error("Create key: Key cannot be negative");
        }

        if(keyLen < MIN_KEY_LEN) {
            throw std::runtime_error("Create key: Use value of 2048 or higher");
        }

        if(keyLen > MAX_KEY_LEN) {
            throw std::runtime_error("Create key: Keys with len greater that "
                    + std::to_string(MAX_KEY_LEN) + " are not supperted");
        }

        auto pubKeyPath = createKeyStatement->pubKeyPath;
        auto prvKeyIdPath = createKeyStatement->privateKeyIdFile;

        if(pubKeyPath == prvKeyIdPath) {
            throw std::runtime_error("Create key: Prv and Pub files have to differ");
        }

        auto r = openSSLHandler->createKey(keyLen);
        interface->writePrivateKeyToFile(prvKeyIdPath, "wb", r.get(), overwrite);
        interface->writePublicKeyToFile(pubKeyPath, "wb", r.get(),overwrite);

        return {"Keys created"};

    } else if (auto signStatement = std::dynamic_pointer_cast<SignStatement>(statement)) {
        auto fileToBeSigned = signStatement->filePathToFileToBeSigned;
        auto prvKeyPath = signStatement->filePathToPrvKeyId;
        auto signatureOutput = signStatement->signatureOutput;
        auto overwrite = signStatement->overwrite;

        auto messageToSign = interface->readMessageFromFile(fileToBeSigned);
        RSA* rsaPrv = interface->readPrivateKeyFromFile(prvKeyPath);

        auto encryptedMessage = openSSLHandler->sign(rsaPrv, messageToSign);

        if(VERBOSE) {
            std::cout << "encrypted msg is " << encryptedMessage << std::endl;
        }
        interface->writeToFile(signatureOutput, encryptedMessage, overwrite);
        RSA_free(rsaPrv);

        return {"File signed"};

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

        auto rsaPub = interface->readPublicKeyFromFile(filePathToPublicKey);
        auto isSignatureCorrect = openSSLHandler->checkSignature(rsaPub, messageToCheckHash, messageToCheck);
        if(isSignatureCorrect) {
            return {"Signature correct"};
        }
        return {"Signature not correct"};

    } else if (auto deleteKeyStatement = std::dynamic_pointer_cast<DeleteKeyStatement>(statement)) {

        auto filePathToPrivateKeyId = deleteKeyStatement->privateKeyIdPath;
        auto filePathToPublicKey = deleteKeyStatement->fileToPublicKey;

        interface->removePrivateKey(filePathToPrivateKeyId);
        interface->removePublicKey(filePathToPublicKey);

        return {"Keys deleted"};

    } else if (auto getPrivateKeyStatement = std::dynamic_pointer_cast<GetPrivateKeyStatement>(statement)) {

        auto filePathWithPrvKeyId = getPrivateKeyStatement->filePathWithPrivateKeyId;
        auto filePathToStoreKey = getPrivateKeyStatement->filePathToStorePrivateKey;
        auto overwrite = getPrivateKeyStatement->overwrite;

        auto prvKey = interface->getPrivateKey(filePathWithPrvKeyId);
        interface->writeToFile(filePathToStoreKey, prvKey, overwrite);
        return {"Private key saved"};

    } else if (auto encryptFileStatement = std::dynamic_pointer_cast<EncryptFileStatement>(statement)) {

        throw std::runtime_error("Not implemented");

    } else if (auto decryptFileStatement = std::dynamic_pointer_cast<DecryptFileStatement>(statement)) {

        throw std::runtime_error("Not implemented");

    }

    throw std::runtime_error("Critical error\n");
}



#pragma clang diagnostic pop