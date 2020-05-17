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

#endif //KEYREPO_STATEMENT_H
