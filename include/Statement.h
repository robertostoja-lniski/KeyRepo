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
    std::string privateKeyIdFile;
    std::string pubKeyPath;
    int keyLen;
    std::string algorithm;
    bool overwrite {false};

    CreateKeyStatement(std::string privateKeyIdFile, std::string pubKeyPath, int keyLen, std::string algorithm) :
            privateKeyIdFile(privateKeyIdFile), pubKeyPath(pubKeyPath), keyLen(keyLen), algorithm(algorithm) {}

    friend std::ostream& operator<<(std::ostream& os, const CreateKeyStatement& dt);
    std::string toString() override {
        auto ret =  "create-key " + privateKeyIdFile + " " + pubKeyPath
                 + " " + std::to_string(keyLen) + " " + algorithm;
        return overwrite ? ret + " overwrite" : ret;
    }
    void enableOverwrite() {
        overwrite = true;
    }
};
struct GetPrivateKeyStatement : Statement {
    std::string filePathWithPrivateKeyId;
    std::string filePathToStorePrivateKey;
    bool overwrite {false};

    GetPrivateKeyStatement(std::string filePathWithPrivateKeyId, std::string filePathToStorePrivateKey) :
            filePathWithPrivateKeyId(filePathWithPrivateKeyId), filePathToStorePrivateKey(filePathToStorePrivateKey) {}

    friend std::ostream& operator<<(std::ostream& os, const GetPrivateKeyStatement& dt);
    std::string toString() override {
        auto ret =  "get-private-key "+ filePathWithPrivateKeyId + " "
            + filePathToStorePrivateKey;
        return overwrite ? ret + " overwrite" : ret;
    }
    void enableOverwrite() {
        overwrite = true;
    }
};
struct DeleteKeyStatement : Statement {
    std::string privateKeyIdPath;
    std::string fileToPublicKey;

    DeleteKeyStatement(std::string privateKeyIdPath, std::string fileToPublicKey) :
            privateKeyIdPath(privateKeyIdPath), fileToPublicKey(fileToPublicKey) {}

    friend std::ostream& operator<<(std::ostream& os, const DeleteKeyStatement& dt);
    std::string toString() override {
        return "delete-key "+ privateKeyIdPath + " "
               + fileToPublicKey;
    }
};
struct SignStatement : Statement {
    std::string filePathToPrvKeyId;
    std::string filePathToFileToBeSigned;
    std::string signatureOutput;
    bool overwrite {false};

    SignStatement(std::string filePathToPrvKeyId, std::string filePathToFileToBeSigned,
            std::string signatureOutput) :
            filePathToPrvKeyId(filePathToPrvKeyId), filePathToFileToBeSigned(filePathToFileToBeSigned),
            signatureOutput(signatureOutput) {}

    friend std::ostream& operator<<(std::ostream& os, const SignStatement& dt);
    std::string toString() override {
        auto ret = "sign " + filePathToPrvKeyId + " " + filePathToFileToBeSigned
             + " " + signatureOutput;
        return overwrite ? ret + " overwrite" : ret;
    }
    void enableOverwrite() {
        overwrite = true;
    }
};
struct CheckSignatureStatement : Statement {
    std::string filePathToPublicKey;
    std::string filePathToFileToBeChecked;
    std::string signatureInput;

    CheckSignatureStatement(std::string filePathToPublicKey, std::string filePathToFileToBeChecked,
                            std::string signatureInput) :
                            filePathToPublicKey(filePathToPublicKey), filePathToFileToBeChecked(filePathToFileToBeChecked),
                            signatureInput(signatureInput) {}

    friend std::ostream& operator<<(std::ostream& os, const CheckSignatureStatement& dt);
    std::string toString() override {
        return "check-signature " + filePathToPublicKey + " "  + filePathToFileToBeChecked
            + " " + signatureInput;
    }
};
struct EncryptFileStatement : Statement {
    std::string filePathWithPrivateKeyId;
    std::string filePathToFileToBeEncrypted;
    std::string output;
    bool overwrite {false};

    EncryptFileStatement(std::string filePathWithPrivateKeyId, std::string filePathToFileToBeEncrypted, std::string output) :
            filePathWithPrivateKeyId(filePathWithPrivateKeyId), filePathToFileToBeEncrypted(filePathToFileToBeEncrypted), output(output) {}

    friend std::ostream& operator<<(std::ostream& os, const EncryptFileStatement& dt);
    std::string toString() override {
        auto ret =  "encrypt-file " + filePathWithPrivateKeyId + " " + filePathToFileToBeEncrypted + " " + output;
        return overwrite ? ret + " overwrite" : ret;
    }
    void enableOverwrite() {
        overwrite = true;
    }
};
struct DecryptFileStatement : Statement {
    std::string filePathToFileToPublicKey;
    std::string filePathToFileToBeDecrypted;
    std::string output;
    bool overwrite {false};

    DecryptFileStatement(std::string filePathToFileToPublicKey, std::string filePathToFileToBeDecrypted, std::string output) :
            filePathToFileToBeDecrypted(filePathToFileToBeDecrypted), filePathToFileToPublicKey(filePathToFileToPublicKey), output(output) {}

    friend std::ostream& operator<<(std::ostream& os, const EncryptFileStatement& dt);
    std::string toString() override {
        auto ret =  "decrypt-file " + filePathToFileToPublicKey + " " + filePathToFileToBeDecrypted + " " + output;
        return overwrite ? ret + " overwrite" : ret;
    }
    void enableOverwrite() {
        overwrite = true;
    }
};
#endif //KEYREPO_STATEMENT_H
