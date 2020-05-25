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
        auto messageToSign = readMessageFromFile(fileToBeSigned);
        std::string prvKeyPath = "/home/robert/Desktop/private.pem";
        RSA* rsaPrv = readPrivateKeyFromFile(prvKeyPath);
        auto encryptedMessage = sign(rsaPrv, messageToSign);
        std::cout << encryptedMessage << std::endl;
        writeToFile("/home/robert/Desktop/signature.txt", encryptedMessage);
        RSA_free(rsaPrv);

    } else if (auto checkSignatureStatement = std::dynamic_pointer_cast<CheckSignatureStatement>(statement)) {
        auto filePathToFileToBeChecked = checkSignatureStatement->filePathToFileToBeChecked;
        auto filePathToPublicKey = checkSignatureStatement->filePathToPublicKey;
        std::string messageToCheck = readMessageFromFile(filePathToFileToBeChecked);
        std::cout << messageToCheck << '\n';
        auto messageToCheckHash = readMessageFromFile("/home/robert/Desktop/signature.txt");
        RSA* rsaPub = readPublicKeyFromFile(filePathToPublicKey);
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
        writePublicKeyToFile(pubKeyPath, "wb", r);
        printf("\nkey pointer is %p\n", r);
        writePrivateKeyToFile(prvKeyIdPath, "wb", r);

    } catch(std::exception &e) {
        std::cout << "\nError when generating key " << e.what() << "\n";
    }
    RSA_free(r);
    std::cout << "Keys are generated\n";
}

RSA* Executor::readPublicKeyFromFile(std::string filepath) {
//     printFile(filepath);
     auto fp = getFileStructFromPath(filepath, "r");
     return readPublicKeyFromFpAndClose(&fp);
}
RSA* Executor::readPrivateKeyFromFile(std::string filepath) {
//    printFile(filepath);
    auto fp = getFileStructFromPath(filepath, "r");
    return readPrivateKeyFromFpAndClose(&fp);
}
void Executor::printFile(std::string filepath) {
    std::cout << "\nprinting " + filepath + "\n";
    std::string line;
    std::ifstream myfile (filepath);
    if (myfile.is_open())
    {
        while ( getline (myfile,line) )
        {
            std::cout << line << '\n';
        }
        myfile.close();
    }
    else std::cout << "Unable to open file";
}
FILE *Executor::getFileStructFromPath(std::string filepath, std::string modes) {
    FILE *fp = fopen(filepath.c_str(), modes.c_str());
    if(fp == nullptr) {
        throw std::runtime_error("Cannot open key file");
    }
    return fp;
}
RSA *Executor::readPublicKeyFromFpAndClose(FILE **fp) {
    auto rsa = PEM_read_RSA_PUBKEY(*fp, nullptr, nullptr, nullptr);
    fclose(*fp);
    if(!rsa) {
        throw std::runtime_error("Could not read pubkey from file");
    }
    return rsa;
}
RSA *Executor::readPrivateKeyFromFpAndClose(FILE **fp) {
    auto rsa = PEM_read_RSAPrivateKey(*fp, nullptr, nullptr, nullptr);
    fclose(*fp);
    if(!rsa) {
        throw std::runtime_error("Could not read private key from file");
    }
    return rsa;
}
void Executor::writePublicKeyToFile(std::string filepath, std::string mode, RSA *r) {
    auto fp = getFileStructFromPath(filepath, mode);
    auto success = PEM_write_RSA_PUBKEY(fp, r);
    if(!success) {
        throw std::runtime_error("Cannot save public key");
    }
    fclose(fp);
}
void Executor::writePrivateKeyToFile(std::string filepath, std::string mode, RSA *r) {
    auto fp = getFileStructFromPath(filepath, mode);
    auto success = PEM_write_RSAPrivateKey(fp, r, nullptr, nullptr, 0, nullptr, nullptr);
    if(!success) {
        throw std::runtime_error("Cannot save private key");
    }
    fclose(fp);
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

std::string Executor::readMessageFromFile(std::string filepath) {
    std::string fileContent;
    std::string line;
    std::ifstream fileToRead(filepath);
    if (fileToRead.is_open()) {
        while (getline(fileToRead,line)) {
            fileContent += line + "\n";
        }
        fileToRead.close();
    }
    // eliminates endl at the end
    return fileContent.substr(0, fileContent.size() - 1);
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