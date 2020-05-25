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
        auto pubKeyPath = createKeyStatement->pubKeyPath;
        auto prvKetIdPath = createKeyStatement->privateKeyIdFile;
        createKey(algorithm, keyLen, pubKeyPath, prvKetIdPath);

    } else if (auto signStatement = std::dynamic_pointer_cast<SignStatement>(statement)) {
        std::string fileToBeSigned = signStatement->filePathToFileToBeSigned;
        auto messageToSign = interface->readMessageFromFile(fileToBeSigned);
        std::string prvKeyPath = "/home/robert/Desktop/private.pem";
        RSA* rsaPrv = interface->readPrivateKeyFromFile(prvKeyPath);
        auto encryptedMessage = sign(rsaPrv, messageToSign);
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
        auto ret = checkSignature(rsaPub, messageToCheckHash, messageToCheck);
        std::cout << ret << '\n';
        RSA_free(rsaPub);
    }
}
void Executor::createKey(const std::string& algorithm, int keyLen, const std::string& pubKeyPath, const std::string& prvKeyIdPath) {
    // dummy
    (void)algorithm;
    RSA* r;
    try {
        r = RSA_new();
        assignRsaKeyToPtr(keyLen, &r);

        printf("\nkey pointer is %p\n", r);
        interface->writePublicKeyToFile(pubKeyPath, "wb", r);
        printf("\nkey pointer is %p\n", r);
        interface->writePrivateKeyToFile(prvKeyIdPath, "wb", r);

    } catch(std::exception &e) {
        std::cout << "\nError when generating key " << e.what() << "\n";
    }
    RSA_free(r);
    std::cout << "Keys are generated\n";
}

void Executor::assignRsaKeyToPtr(size_t keyLen, RSA** r) {
    BIGNUM *bne;
    bne = BN_new();
    auto bnSuccess = BN_set_word(bne,RSA_F4);
    if(!bnSuccess) {
        throw std::runtime_error("Cannot set big num");
    }

    auto rsaSuccess = RSA_generate_key_ex(*r, keyLen, bne, nullptr);
    if(!rsaSuccess) {
        throw std::runtime_error("Cannot generate RSA keys");
    }

    BN_free(bne);
}

std::string Executor::sign(RSA *r, std::string messageToSign) {

    EVP_MD_CTX* m_RSASignCtx = EVP_MD_CTX_create();
    EVP_PKEY* prv  = EVP_PKEY_new();
    EVP_PKEY_assign_RSA(prv, r);
    if (EVP_DigestSignInit(m_RSASignCtx,NULL, EVP_sha256(), NULL, prv) != 1) {
        throw std::runtime_error("Digest init failed");
    }
    if (EVP_DigestSignUpdate(m_RSASignCtx, messageToSign.c_str(), messageToSign.length()) != 1) {
        throw std::runtime_error("Digest sign update failed");
    }
    auto lenAfterSign = std::make_shared<size_t>();
    if (EVP_DigestSignFinal(m_RSASignCtx, NULL, lenAfterSign.get()) != 1) {
        throw std::runtime_error("Digest sign final failed");
    }
    std::cout << *lenAfterSign << '\n';
    std::string encryptedMessage(*lenAfterSign, ' ');
    if (EVP_DigestSignFinal(m_RSASignCtx, (unsigned char*)encryptedMessage.c_str(), lenAfterSign.get()) != 1) {
        throw std::runtime_error("Digest init failed");
    }
    EVP_MD_CTX_destroy(m_RSASignCtx);
    return encryptedMessage;
}

bool Executor::checkSignature(RSA *rsa, std::string hash, std::string msg) {
    EVP_PKEY* pubKey  = EVP_PKEY_new();
    EVP_PKEY_assign_RSA(pubKey, rsa);
    EVP_MD_CTX* m_RSAVerifyCtx = EVP_MD_CTX_create();

    if(EVP_DigestVerifyInit(m_RSAVerifyCtx,NULL, EVP_sha256(),NULL,pubKey) != 1) {
        throw std::runtime_error("Digest verify init failed");
    }
    if(EVP_DigestVerifyUpdate(m_RSAVerifyCtx, msg.c_str(), msg.length()) != 1) {
        throw std::runtime_error("Digest verify update failed");
    }
    if(EVP_DigestVerifyFinal(m_RSAVerifyCtx, (unsigned char* )hash.c_str(), hash.length()) != 1) {
        throw std::runtime_error("Verification failed");
    }
    EVP_MD_CTX_destroy(m_RSAVerifyCtx);
    return true;
}

#pragma clang diagnostic pop