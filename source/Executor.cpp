#pragma clang diagnostic push
#pragma ide diagnostic ignored "hicpp-signed-bitwise"
//
// Created by robert on 17.05.2020.
//

#include <cstring>
#include <sstream>
#include <fstream>
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
        char msg[] = "message to sign";
        size_t len = sizeof(msg);
        unsigned char **encryptedMsg;
        size_t *encryptedMsgLen;
        sign(reinterpret_cast<const unsigned char *>(msg), len, encryptedMsg, encryptedMsgLen);
        if(encryptedMsg == nullptr) {
            throw std::runtime_error("Message was not encrypted");
        }
        currentlyEncryptedMsgLen = *encryptedMsgLen;
        currentlyEncryptedMsg = (char** )malloc(sizeof(char) * (*encryptedMsgLen) + 1);
        strncpy(*currentlyEncryptedMsg, (const char*)*encryptedMsg, *encryptedMsgLen * sizeof(char) + 1);
        if(strncmp(*currentlyEncryptedMsg, reinterpret_cast<const char *>(*encryptedMsg), currentlyEncryptedMsgLen) == 0) {
            std::cout << "\nThe same signs\n";
        }
        std::cout << '\n' << *encryptedMsg << "\n" << *currentlyEncryptedMsg;
    } else if (auto checkSignatureStatement = std::dynamic_pointer_cast<CheckSignatureStatement>(statement)) {
        char msg[] = "message to sign";
        size_t len = sizeof(msg);

        std::cout << '\n' << *currentlyEncryptedMsg;
        std::cout << '\n' << checkSignature((unsigned char*)*currentlyEncryptedMsg, currentlyEncryptedMsgLen, msg, len);
    }
}
void Executor::createKey(const std::string& algorithm, int keyLen, const std::string& pubKeyPath, const std::string& prvKeyIdPath) {
    // dummy
    (void)algorithm;
    RSA* r = nullptr;
    try {

        assignRsaKeyToPtr(keyLen, &r);
        writePublicKeyToFile(pubKeyPath, "wb", r);
        writePrivateKeyToFile(prvKeyIdPath, "wb", r);

        auto rsaPub = readPublicKeyFromFile("/home/robert/Desktop/public.pem");
        RSA_free(rsaPub);
        auto rsaPrv = readPrivateKeyFromFile("/home/robert/Desktop/private.pem");
        RSA_free(rsaPrv);

    } catch(std::exception &e) {
        std::cout << "\nError when generating key " << e.what() << "\n";
    }
    RSA_free(r);
    std::cout << "Keys are generated\n";
}
void Executor::sign(const unsigned char *data, size_t dataSize, unsigned char **encryptedData, size_t *encryptedDataSize) {
    BIO *bio = BIO_new(BIO_s_file());
    if(bio == nullptr) {
        throw std::runtime_error("Bio could not be initialised\n");
    }
    BIO_read_filename(bio, "/home/robert/Desktop/private.pem");
    RSA* rsa;
    if((rsa = PEM_read_bio_RSAPrivateKey(bio, nullptr, nullptr, nullptr)) == nullptr) {
        throw std::runtime_error("Private key cannot be accessed");
    }
    EVP_MD_CTX* mdCtx = EVP_MD_CTX_create();
    EVP_PKEY* priKey  = EVP_PKEY_new();
    EVP_PKEY_assign_RSA(priKey, rsa);
    if (EVP_DigestSignInit(mdCtx,nullptr, EVP_sha256(), nullptr, priKey) != 1) {
        EVP_MD_CTX_destroy(mdCtx);
        throw std::runtime_error("Could not initialise evp digest");
    }
    if (EVP_DigestSignUpdate(mdCtx, data, dataSize) != 1) {
        EVP_MD_CTX_destroy(mdCtx);
        throw std::runtime_error("Could not hash msgLen bytes");
    }
    if (EVP_DigestSignFinal(mdCtx, nullptr, encryptedDataSize) != 1) {
        EVP_MD_CTX_destroy(mdCtx);
        throw std::runtime_error("Could not set encrypted data size");
    }
    *encryptedData = (unsigned char*)malloc(*encryptedDataSize);
    if(*encryptedData == nullptr) {
        EVP_MD_CTX_destroy(mdCtx);
        throw std::runtime_error("Cannot allocate encrypted msg");
    }
    if (EVP_DigestSignFinal(mdCtx, *encryptedData, encryptedDataSize) != 1) {
        EVP_MD_CTX_destroy(mdCtx);
        throw std::runtime_error("Cannot sign msg");
    }
    EVP_MD_CTX_destroy(mdCtx);
}
bool Executor::checkSignature(unsigned char *data, size_t dataLen, const char *originalData, size_t originalDataSize) {

    auto rsaPub = readPublicKeyFromFile("/home/robert/Desktop/public.pem");
    RSA_free(rsaPub);
    auto rsaPrv = readPrivateKeyFromFile("/home/robert/Desktop/private.pem");
    RSA_free(rsaPrv);
    return true;
    RSA *rsa;
    EVP_PKEY* pubKey  = EVP_PKEY_new();
    EVP_PKEY_assign_RSA(pubKey, rsa);
    EVP_MD_CTX* mdCtx = EVP_MD_CTX_create();
    if (EVP_DigestVerifyInit(mdCtx, nullptr, EVP_sha256(), nullptr, pubKey) != 1) {
        throw std::runtime_error("Cannot initialise signature check");
    }
    if (EVP_DigestVerifyUpdate(mdCtx, originalData, originalDataSize) != 1) {
        throw std::runtime_error("Cannot update buffer to check signature");
    }
    auto res = EVP_DigestVerifyFinal(mdCtx, data, dataLen);
    EVP_MD_CTX_destroy(mdCtx);
    return res;
}
RSA* Executor::readPublicKeyFromFile(std::string filepath) {
     printFile(filepath);
     auto fp = getFileStructFromPath(filepath, "r");
     return readPublicKeyFromFpAndClose(&fp);
}

RSA* Executor::readPrivateKeyFromFile(std::string filepath) {
    printFile(filepath);
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

void Executor::assignRsaKeyToPtr(size_t keyLen, RSA **r) {
    BIGNUM *bne;
    bne = BN_new();
    auto bnSuccess = BN_set_word(bne,RSA_F4);
    if(!bnSuccess) {
        throw std::runtime_error("Cannot set big num");
    }

    *r = RSA_new();
    auto rsaSuccess = RSA_generate_key_ex(*r, keyLen, bne, nullptr);
    if(!rsaSuccess) {
        throw std::runtime_error("Cannot generate RSA keys");
    }

    BN_free(bne);
}


#pragma clang diagnostic pop