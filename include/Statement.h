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
    bool overwrite {false};

    CreateKeyStatement(std::string algorithm, int keyLen, std::string pubKeyPath, std::string privateKeyIdFile) :
        algorithm(algorithm), keyLen(keyLen), pubKeyPath(pubKeyPath), privateKeyIdFile(privateKeyIdFile) {}

    friend std::ostream& operator<<(std::ostream& os, const CreateKeyStatement& dt);
    std::string toString() override {
        auto ret =  "create-key " + algorithm + " " + std::to_string(keyLen)
                + " " + pubKeyPath + " " + privateKeyIdFile;
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
    std::string filePathToFileToBeSigned;
    std::string filePathToPrvKeyId;
    std::string signatureOutput;
    bool overwrite {false};

    SignStatement(std::string filePathToFileToBeSigned, std::string filePathToPrvKeyId,
                    std::string signatureOutput) :
            filePathToFileToBeSigned(filePathToFileToBeSigned), filePathToPrvKeyId(filePathToPrvKeyId),
            signatureOutput(signatureOutput) {}

    friend std::ostream& operator<<(std::ostream& os, const SignStatement& dt);
    std::string toString() override {
        auto ret = "sign " + filePathToFileToBeSigned + " "
            + filePathToPrvKeyId + " " + signatureOutput;
        return overwrite ? ret + " overwrite" : ret;
    }
    void enableOverwrite() {
        overwrite = true;
    }
};
struct CheckSignatureStatement : Statement {
    std::string filePathToFileToBeChecked;
    std::string filePathToPublicKey;
    std::string signatureInput;

    CheckSignatureStatement(std::string filePathToFileToBeChecked, std::string filePathToPublicKey,
                            std::string signatureInput) :
        filePathToFileToBeChecked(filePathToFileToBeChecked), filePathToPublicKey(filePathToPublicKey),
        signatureInput(signatureInput) {}

    friend std::ostream& operator<<(std::ostream& os, const CheckSignatureStatement& dt);
    std::string toString() override {
        return "check-signature " + filePathToFileToBeChecked + " "
               + filePathToPublicKey + " " + signatureInput;
    }
};
struct EncryptFileStatement : Statement {
    std::string filePathToFileToBeEncrypted;
    std::string output;
    std::string filePathWithPrivateKeyId;
    bool overwrite {false};

    EncryptFileStatement(std::string filePathToFileToBeEncrypted, std::string output, std::string filePathWithPrivateKeyId) :
            filePathToFileToBeEncrypted(filePathToFileToBeEncrypted), output(output), filePathWithPrivateKeyId(filePathWithPrivateKeyId) {}

    friend std::ostream& operator<<(std::ostream& os, const EncryptFileStatement& dt);
    std::string toString() override {
        auto ret =  "encrypt-file " + filePathToFileToBeEncrypted + " " + output + " "
               + filePathWithPrivateKeyId;
        return overwrite ? ret + " overwrite" : ret;
    }
    void enableOverwrite() {
        overwrite = true;
    }
};
struct DecryptFileStatement : Statement {
    std::string filePathToFileToBeDecrypted;
    std::string output;
    std::string filePathToFileToPublicKey;
    bool overwrite {false};

    DecryptFileStatement(std::string filePathToFileToBeDecrypted, std::string output, std::string filePathToFileToPublicKey) :
            filePathToFileToBeDecrypted(filePathToFileToBeDecrypted), output(output), filePathToFileToPublicKey(filePathToFileToPublicKey) {}

    friend std::ostream& operator<<(std::ostream& os, const EncryptFileStatement& dt);
    std::string toString() override {
        auto ret =  "decrypt-file " + filePathToFileToBeDecrypted + " " + output + " "
               + filePathToFileToPublicKey;
        return overwrite ? ret + " overwrite" : ret;
    }
    void enableOverwrite() {
        overwrite = true;
    }
};
#endif //KEYREPO_STATEMENT_H
