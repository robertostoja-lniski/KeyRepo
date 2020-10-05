//
// Created by robert on 17.05.2020.
//

#include "../include/Statement.h"

std::ostream& operator<<(std::ostream& os, const CreateKeyStatement& dt)
{
    os << "Create key with: " << dt.algorithm << " algorithm, " << dt.keyLen << " length of key "
       << dt.pubKeyPath << " path to public key, " << dt.privateKeyIdFile << " and private key id\n";
    return os;
}

std::ostream &operator<<(std::ostream &os, const GetPrivateKeyStatement &dt) {
    os << "Get private key with: " << dt.filePathWithPrivateKeyId << " id " << dt.filePathToStorePrivateKey
        << " and filepath to store it.\n";
    return os;
}
std::ostream &operator<<(std::ostream &os, const DeleteKeyStatement &dt) {
    os << "Deleting a pair of keys: " << dt.privateKeyIdPath << " private key and "
        << dt.fileToPublicKey << " public key\n";
}
std::ostream &operator<<(std::ostream &os, const SignStatement &dt) {
    os << "Signing file " << dt.filePathToFileToBeSigned << " with key with id stored in "
        << dt.filePathToPrvKeyId << '\n';
}
std::ostream &operator<<(std::ostream &os, const CheckSignatureStatement &dt) {
    os << "Checking signature for " << dt.filePathToFileToBeChecked << " by "
        << dt.filePathToPublicKey << " public key\n";
}
std::ostream &operator<<(std::ostream &os, const EncryptFileStatement &dt) {
    os << "Encrypting file " << dt.filePathToFileToBeEncrypted << " by "
        << dt.filePathToFileWithKey << " key, " << dt.filePathToFileWithIv <<  " iv and with output " + dt.output + "\n";
}
std::ostream &operator<<(std::ostream &os, const DecryptFileStatement &dt) {
    os << "Decrypting file " << dt.filePathToFileToBeDecrypted << " by "
       << dt.filePathToFileWithKey << " key, " << dt.filePathToFileWithIv << " iv and with output " + dt.output + "\n";
}
