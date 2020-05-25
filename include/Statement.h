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
    std::string privateKeyIdFile;

    CreateKeyStatement(std::string algorithm, int keyLen, std::string pubKeyPath, std::string privateKeyIdFile) :
        algorithm(algorithm), keyLen(keyLen), pubKeyPath(pubKeyPath), privateKeyIdFile(privateKeyIdFile) {}

    friend std::ostream& operator<<(std::ostream& os, const CreateKeyStatement& dt);
    std::string toString() override {
        return "create-key " + algorithm + " " + std::to_string(keyLen)
                + " " + pubKeyPath + " " + privateKeyIdFile;
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
struct DeleteKeyStatement : Statement {
    int privateKeyId;
    std::string fileToPublicKey;

    DeleteKeyStatement(int privateKeyId, std::string fileToPublicKey) :
            privateKeyId(privateKeyId), fileToPublicKey(fileToPublicKey) {}

    friend std::ostream& operator<<(std::ostream& os, const DeleteKeyStatement& dt);
    std::string toString() override {
        return "delete-key "+ std::to_string(privateKeyId) + " "
               + fileToPublicKey;
    }
};
struct SignStatement : Statement {
    std::string filePathToFileToBeSigned;
    int privateKeyId;

    SignStatement(std::string filePathToFileToBeSigned, int privateKeyId) :
            filePathToFileToBeSigned(filePathToFileToBeSigned), privateKeyId(privateKeyId) {}

    friend std::ostream& operator<<(std::ostream& os, const SignStatement& dt);
    std::string toString() override {
        return "sign " + filePathToFileToBeSigned + " "
            + std::to_string(privateKeyId) + "\n";
    }
};
struct CheckSignatureStatement : Statement {
    std::string filePathToFileToBeChecked;
    std::string filePathToPublicKey;

    CheckSignatureStatement(std::string filePathToFileToBeChecked, std::string filePathToPublicKey) :
        filePathToFileToBeChecked(filePathToFileToBeChecked), filePathToPublicKey(filePathToPublicKey) {}

    friend std::ostream& operator<<(std::ostream& os, const CheckSignatureStatement& dt);
    std::string toString() override {
        return "check-signature " + filePathToFileToBeChecked + " "
               + filePathToPublicKey + "\n";
    }
};
struct EncryptFileStatement : Statement {
    std::string filePathToFileToBeEncrypted;
    int privateKeyId;

    EncryptFileStatement(std::string filePathToFileToBeEncrypted, int privateKeyId) :
            filePathToFileToBeEncrypted(filePathToFileToBeEncrypted), privateKeyId(privateKeyId) {}

    friend std::ostream& operator<<(std::ostream& os, const EncryptFileStatement& dt);
    std::string toString() override {
        return "encrypt-file " + filePathToFileToBeEncrypted + " "
               + std::to_string(privateKeyId);
    }
};

#endif //KEYREPO_STATEMENT_H
