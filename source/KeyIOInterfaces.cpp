//
// Created by robert on 25.05.2020.
//

#include "../include/KeyIOInterfaces.h"

RSA* RsaKeyFileIOInterface::readPublicKeyFromFile(std::string filepath) {
//     printFile(filepath);
    auto fp = getFileStructFromPath(filepath, "r");
    return readPublicKeyFromFpAndClose(&fp);
}
RSA* RsaKeyFileIOInterface::readPrivateKeyFromFile(std::string filepath) {
//    printFile(filepath);
    auto fp = getFileStructFromPath(filepath, "r");
    return readPrivateKeyFromFpAndClose(&fp);
}
void RsaKeyFileIOInterface::printFile(std::string filepath) {
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
std::string RsaKeyFileIOInterface::readMessageFromFile(std::string filepath) {
    std::string fileContent;
    std::string line;
    std::ifstream fileToRead(filepath);
    if (fileToRead.is_open()) {
        while (getline(fileToRead,line)) {
            fileContent += line + "\n";
        }
        fileToRead.close();
    }
        // eliminates endl at the endl
    return fileContent.substr(0, fileContent.size() - 1);
}
FILE *RsaKeyFileIOInterface::getFileStructFromPath(std::string filepath, std::string modes) {
    FILE *fp = fopen(filepath.c_str(), modes.c_str());
    if(fp == nullptr) {
        throw std::runtime_error("Cannot open key file");
    }
    return fp;
}
RSA *RsaKeyFileIOInterface::readPublicKeyFromFpAndClose(FILE **fp) {
    auto rsa = PEM_read_RSA_PUBKEY(*fp, nullptr, nullptr, nullptr);
    fclose(*fp);
    if(!rsa) {
        throw std::runtime_error("Could not read pubkey from file");
    }
    return rsa;
}
RSA *RsaKeyFileIOInterface::readPrivateKeyFromFpAndClose(FILE **fp) {
    auto rsa = PEM_read_RSAPrivateKey(*fp, nullptr, nullptr, nullptr);
    fclose(*fp);
    if(!rsa) {
        throw std::runtime_error("Could not read private key from file");
    }
    return rsa;
}
void RsaKeyFileIOInterface::writePublicKeyToFile(std::string filepath, std::string mode, RSA *r) {
    auto fp = getFileStructFromPath(filepath, mode);
    auto success = PEM_write_RSA_PUBKEY(fp, r);
    if(!success) {
        throw std::runtime_error("Cannot save public key");
    }
    fclose(fp);
}
void RsaKeyFileIOInterface::writePrivateKeyToFile(std::string filepath, std::string mode, RSA *r) {
    // temporary solution for proof of concept only
    auto id = kernelEmulation->write(r);
    std::ofstream os;
    os.open(filepath);
    os << id;
    os.close();
//    auto fp = getFileStructFromPath(filepath, mode);
//    auto success = PEM_write_RSAPrivateKey(fp, r, nullptr, nullptr, 0, nullptr, nullptr);
//    if(!success) {
//        throw std::runtime_error("Cannot save private key");
//    }
//    fclose(fp);
}
void RsaKeyFileIOInterface::writeToFile(std::string filepath, std::string data) {
    std::ofstream myfile;
    myfile.open (filepath);
    myfile << data;
    myfile.close();
}