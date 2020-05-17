//
// Created by robert on 17.05.2020.
//

#include "../include/Executor.h"

void Executor::execute() {
    parser->parse();
    auto statementStr = parser->getCurrentParsedStatementStr();
    std::cout << statementStr;

    auto statement = parser->getCurrentParsedStatement();
    if(auto createKeyStatement = std::dynamic_pointer_cast<CreateKeyStatement>(statement)) {
        // currently only RSA support implemented
        std::string algorithm = "RSA";
        auto keyLen = createKeyStatement->keyLen;
        auto pubKeyPath = createKeyStatement->pubKeyPath;
        auto prvKetIdPath = createKeyStatement->privateKeyIdFile;

        createKey(algorithm, keyLen, pubKeyPath, prvKetIdPath);
    }
}

void Executor::createKey(const std::string& algorithm, int keyLen, const std::string& pubKeyPath, const std::string& prvKeyIdPath) {
    // dummy
    (void)algorithm;

    RSA	*r = nullptr;
    BIGNUM *bne = nullptr;
    BIO *bp_public = nullptr, *bp_private = nullptr;
    unsigned long e = RSA_F4;

    try {
        bne = BN_new();
        if(BN_set_word(bne,e) != 1) {
            throw std::runtime_error("Cannot set big num");
        }

        r = RSA_new();
        if(RSA_generate_key_ex(r, keyLen, bne, nullptr) != 1) {
            throw std::runtime_error("Cannot generate RSA keys");
        }

        bp_public = BIO_new_file(pubKeyPath.c_str(), "w+");
        if(PEM_write_bio_RSAPublicKey(bp_public, r) != 1) {
            throw std::runtime_error("Cannot save public key");
        }

        // should be passed to kernel
        bp_private = BIO_new_file(prvKeyIdPath.c_str(), "w+");
        if(PEM_write_bio_RSAPrivateKey(bp_private, r, nullptr, nullptr, 0, nullptr, nullptr) != 1) {
            throw std::runtime_error("Cannot save private key");
        }
    } catch(std::exception &e) {
        std::cout << "Error when generating key " << e.what() << "\n";
    }

    BIO_free_all(bp_public);
    BIO_free_all(bp_private);
    RSA_free(r);
    BN_free(bne);
}

