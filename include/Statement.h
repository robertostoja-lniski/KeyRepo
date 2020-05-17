//
// Created by robert on 17.05.2020.
//

#ifndef KEYREPO_STATEMENT_H
#define KEYREPO_STATEMENT_H
#include <string>
#include <ostream>

struct Statement {
    virtual std::string toString() = 0;
};

struct CreateKeyStatement : Statement {
    std::string algorithm;
    int keyLen;
    std::string pubKeyPath;
    int privateKeyId;

    CreateKeyStatement(std::string algorithm, int keyLen, std::string pubKeyPath, int privateKeyId) :
        algorithm(algorithm), keyLen(keyLen), pubKeyPath(pubKeyPath), privateKeyId(privateKeyId) {}

    friend std::ostream& operator<<(std::ostream& os, const CreateKeyStatement& dt);
    std::string toString() override {
        return "create-key " + algorithm + " " + std::to_string(keyLen)
                + " " + pubKeyPath + " " + std::to_string(privateKeyId);
    }
};

struct GetPrivateKeyStatement : Statement {
    int privateKeyId;
    std::string filePathToStorePrivateKey;

    GetPrivateKeyStatement(int privateKeyId, std::string filePathToStorePrivateKey) :
        privateKeyId(privateKeyId), filePathToStorePrivateKey(filePathToStorePrivateKey) {}

    friend std::ostream& operator<<(std::ostream& os, const GetPrivateKeyStatement& dt);
    std::string toString() override {
        return "get-private-key "+ std::to_string(privateKeyId) + " "
            + filePathToStorePrivateKey;
    }
};

#endif //KEYREPO_STATEMENT_H
