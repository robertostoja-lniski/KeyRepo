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
#define MAX_KEY_LEN 32 * 2048
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

        auto password = createKeyStatement->password;
        if(password.empty()) {
            throw std::runtime_error("Create key: Password is now mandatory for security reasons!");
        }

        auto pubKeyPath = createKeyStatement->pubKeyPath;
        auto prvKeyIdPath = createKeyStatement->privateKeyIdFile;

        if(pubKeyPath == prvKeyIdPath) {
            throw std::runtime_error("Create key: Prv and Pub files have to differ");
        }

        interface->throwIfOverwriteForbidden(prvKeyIdPath, overwrite);
        interface->throwIfOverwriteForbidden(pubKeyPath, overwrite);

        auto r = openSSLHandler->createKey(keyLen);
        interface->writePrivateKeyToFile(prvKeyIdPath, "wb", r.get(), password, overwrite);
        interface->writePublicKeyToFile(pubKeyPath, "wb", r.get(),overwrite);

        return {"Keys created"};

    } else if (auto signStatement = std::dynamic_pointer_cast<SignStatement>(statement)) {
        auto fileToBeSigned = signStatement->filePathToFileToBeSigned;
        auto prvKeyPath = signStatement->filePathToPrvKeyId;
        auto signatureOutput = signStatement->signatureOutput;
        auto password = signStatement->password;
        auto overwrite = signStatement->overwrite;

        interface->throwIfOverwriteForbidden(signatureOutput, overwrite);

        auto messageToSign = interface->readFromFile(fileToBeSigned);

        RSA* rsaPrv = interface->readPrivateKeyFromFile<RSA*>(prvKeyPath, password);

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

        std::string messageToCheck = interface->readFromFile(filePathToFileToBeChecked);
        if(VERBOSE) {
            std::cout << messageToCheck << '\n';
        }

        auto messageToCheckHash = interface->readFromFile(signatureInput);
        if(VERBOSE) {
            std::cout << messageToCheckHash << " it was encrypted msg\n";
        }

        auto rsaPub = interface->readPublicKeyFromFile<RSA*>(filePathToPublicKey);
        auto isSignatureCorrect = openSSLHandler->checkSignature(rsaPub, messageToCheckHash, messageToCheck);
        if(isSignatureCorrect) {
            std::cout << "Signature correct" << std::endl;
            return {"Signature correct"};
        }

        std::cout << "Signature not correct" << std::endl;
        return {"Signature not correct"};

    } else if (auto deleteKeyStatement = std::dynamic_pointer_cast<DeleteKeyStatement>(statement)) {

        auto filePathToPrivateKeyId = deleteKeyStatement->privateKeyIdPath;
        auto filePathToPublicKey = deleteKeyStatement->fileToPublicKey;

        interface->throwIfCannotRemoveFile(filePathToPrivateKeyId);
        interface->throwIfCannotRemoveFile(filePathToPublicKey);

        interface->removePrivateKey(filePathToPrivateKeyId);
        interface->removePublicKey(filePathToPublicKey);

        return {"Keys deleted"};

    } else if (auto getPrivateKeyStatement = std::dynamic_pointer_cast<GetPrivateKeyStatement>(statement)) {

        auto filePathWithPrvKeyId = getPrivateKeyStatement->filePathWithPrivateKeyId;
        auto filePathToStoreKey = getPrivateKeyStatement->filePathToStorePrivateKey;
        auto password = getPrivateKeyStatement->password;
        auto overwrite = getPrivateKeyStatement->overwrite;

        interface->throwIfOverwriteForbidden(filePathToStoreKey, overwrite);
        auto prvKey = interface->getPrivateKey(filePathWithPrvKeyId, password);
        auto keySize = prvKey.size();
        interface->writeToFile(filePathToStoreKey, prvKey, overwrite);
        return {"Private key saved"};

    } else if (auto encryptFileStatement = std::dynamic_pointer_cast<EncryptFileStatement>(statement)) {

        auto filePathWithKey = encryptFileStatement->filePathToFileWithKey;
        auto filePathWithIv = encryptFileStatement->filePathToFileWithIv;

        auto filePathToFileToBeEncrypted = encryptFileStatement->filePathToFileToBeEncrypted;
        auto output = encryptFileStatement->output;
        auto overwrite = encryptFileStatement->overwrite;

        interface->throwIfOverwriteForbidden(output, overwrite);
        auto iFp = interface->getFileStructFromPath(filePathToFileToBeEncrypted, "rb");
        auto oFp = interface->getFileStructFromPath(output, "wb");

        auto key = interface->readFromFile(filePathWithKey);
        auto iv = interface->readFromFile(filePathWithIv);

        auto config = std::make_shared<Config>();
        config->key = key.c_str();
        config->iv = iv.c_str();

        openSSLHandler->encrypt(config, iFp, oFp);

        fclose(iFp);
        fclose(oFp);

        return {"File encrypted"};

    } else if (auto decryptFileStatement = std::dynamic_pointer_cast<DecryptFileStatement>(statement)) {

        auto filePathWithKey = decryptFileStatement->filePathToFileWithKey;
        auto filePathWithIv = decryptFileStatement->filePathToFileWithIv;

        auto filePathToFileToBeDecrypted = decryptFileStatement->filePathToFileToBeDecrypted;
        auto output = decryptFileStatement->output;
        auto overwrite = decryptFileStatement->overwrite;

        interface->throwIfOverwriteForbidden(output, overwrite);
        auto iFp = interface->getFileStructFromPath(filePathToFileToBeDecrypted, "rb");
        auto oFp = interface->getFileStructFromPath(output, "wb");

        auto key = interface->readFromFile(filePathWithKey);
        auto iv = interface->readFromFile(filePathWithIv);

        auto config = std::make_shared<Config>();
        config->key = key.c_str();
        config->iv = iv.c_str();

        openSSLHandler->decrypt(config, iFp, oFp);

        fclose(iFp);
        fclose(oFp);

        return {"File decrypted"};

    } else if (auto getModeStatement = std::dynamic_pointer_cast<GetModStatement>(statement)) {

        auto pathToKeyId = getModeStatement->filePathToKeyId;
        auto modes = interface->getKeyMode(pathToKeyId);

        auto modeHandler = std::make_unique<ModHandler>();
        return {modeHandler->parseIntModesToString(modes)};

    } else if (auto changeModStatement = std::dynamic_pointer_cast<ChangeModStatement>(statement)) {

        auto pathToKeyId = changeModStatement->filePathToKeyId;
        auto flags = changeModStatement->flags;

        auto modeHandler = std::make_unique<ModHandler>();
        auto modeSetter = modeHandler->stringToModeSetter(flags);
        auto currentModes = interface->getKeyMode(pathToKeyId);
        auto newModeParam = modeHandler->modeSetterToInt(modeSetter, currentModes);
        interface->changeKeyMode(pathToKeyId, newModeParam);


        return {"Mode is set"};

    } else if (auto helpRequestStatement = std::dynamic_pointer_cast<HelpRequestStatement>(statement)) {
        printHelp();
        return {"Help printed"};
    }

    throw std::runtime_error("Critical error\n");
}



#pragma clang diagnostic pop