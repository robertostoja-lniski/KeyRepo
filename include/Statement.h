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
    std::string password;
    bool overwrite {false};

    CreateKeyStatement(std::string privateKeyIdFile, std::string pubKeyPath, int keyLen, std::string algorithm, std::string password) :
            privateKeyIdFile(privateKeyIdFile), pubKeyPath(pubKeyPath), keyLen(keyLen), algorithm(algorithm), password(password) {}

    friend std::ostream& operator<<(std::ostream& os, const CreateKeyStatement& dt);
    std::string toString() override {
        auto ret =  "create-key " + privateKeyIdFile + " " + pubKeyPath
                 + " " + std::to_string(keyLen) + " " + algorithm + " " + password;
        return overwrite ? ret + " overwrite" : ret;
    }
    void enableOverwrite() {
        overwrite = true;
    }
};
struct GetPrivateKeyStatement : Statement {
    std::string filePathWithPrivateKeyId;
    std::string filePathToStorePrivateKey;
    std::string password;
    bool overwrite {false};

    GetPrivateKeyStatement(std::string filePathWithPrivateKeyId, std::string filePathToStorePrivateKey, std::string password) :
            filePathWithPrivateKeyId(filePathWithPrivateKeyId), filePathToStorePrivateKey(filePathToStorePrivateKey), password(password) {}

    friend std::ostream& operator<<(std::ostream& os, const GetPrivateKeyStatement& dt);
    std::string toString() override {
        auto ret =  "get-private-key "+ filePathWithPrivateKeyId + " "
            + filePathToStorePrivateKey + " " + password;
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
    std::string password;
    bool overwrite {false};

    SignStatement(std::string filePathToPrvKeyId, std::string filePathToFileToBeSigned,
            std::string signatureOutput, std::string password) :
            filePathToPrvKeyId(filePathToPrvKeyId),
            filePathToFileToBeSigned(filePathToFileToBeSigned),
            signatureOutput(signatureOutput),
            password(password)
            {}

    friend std::ostream& operator<<(std::ostream& os, const SignStatement& dt);
    std::string toString() override {
        auto ret = "sign " + filePathToPrvKeyId + " " + filePathToFileToBeSigned
             + " " + signatureOutput + " " + password;
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
    std::string filePathToFileWithKey;
    std::string filePathToFileWithIv;
    std::string filePathToFileToBeEncrypted;
    std::string output;
    bool overwrite {false};

    EncryptFileStatement(std::string filePathToFileWithKey, std::string filePathToFileWithIv,
            std::string filePathToFileToBeEncrypted, std::string output) :
            filePathToFileWithKey(filePathToFileWithKey), filePathToFileWithIv(filePathToFileWithIv), filePathToFileToBeEncrypted(filePathToFileToBeEncrypted), output(output) {}

    friend std::ostream& operator<<(std::ostream& os, const EncryptFileStatement& dt);
    std::string toString() override {
        auto ret =  "encrypt-file " + filePathToFileWithKey + " " + filePathToFileWithIv + " " + filePathToFileToBeEncrypted + " " + output;
        return overwrite ? ret + " overwrite" : ret;
    }
    void enableOverwrite() {
        overwrite = true;
    }
};
struct DecryptFileStatement : Statement {
    std::string filePathToFileWithKey;
    std::string filePathToFileWithIv;
    std::string filePathToFileToBeDecrypted;
    std::string output;
    bool overwrite {false};

    DecryptFileStatement(std::string filePathToFileWithKey, std::string filePathToFileWithIv,
            std::string filePathToFileToBeDecrypted, std::string output) :
            filePathToFileWithKey(filePathToFileWithKey), filePathToFileWithIv(filePathToFileWithIv),
            filePathToFileToBeDecrypted(filePathToFileToBeDecrypted), output(output) {}

    friend std::ostream& operator<<(std::ostream& os, const EncryptFileStatement& dt);
    std::string toString() override {
        auto ret =  "decrypt-file " + filePathToFileWithKey + " " + filePathToFileWithIv + " " + filePathToFileToBeDecrypted + " " + output;
        return overwrite ? ret + " overwrite" : ret;
    }
    void enableOverwrite() {
        overwrite = true;
    }
};
struct GetModStatement : Statement {
    std::string filePathToKeyId;

    GetModStatement(std::string filePathToKeyId) :
            filePathToKeyId(filePathToKeyId) {}

    friend std::ostream& operator<<(std::ostream& os, const GetModStatement& dt);
    std::string toString() override {
        return "gmod " + filePathToKeyId;
    }

};
struct ChangeModStatement : Statement {
    std::string filePathToKeyId;
    std::string flags;

    ChangeModStatement(std::string filePathToKeyId, std::string flags) :
            filePathToKeyId(filePathToKeyId), flags(flags) {}

    friend std::ostream& operator<<(std::ostream& os, const ChangeModStatement& dt);
    std::string toString() override {
        return "chmod " + filePathToKeyId + " " + flags;
    }

};

struct HelpRequestStatement : Statement {
    HelpRequestStatement() = default;
    std::string toString() override {
        return {"help"};
    }
};

#endif //KEYREPO_STATEMENT_H
