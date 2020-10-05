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

    char* cPrvKeyPath = NULL;
    auto ret = readKey(filepath.c_str(), &cPrvKeyPath);

    if(ret == -1) {
        throw std::runtime_error("KeyIOInterface: Failed to read private key");
    }

    auto prvKeyPath = std::string(cPrvKeyPath);

    auto fp = getFileStructFromPath(prvKeyPath, "r");
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

    if(!fileToRead.good()) {
        throw std::runtime_error("KeyIOInterface: Failed to read file");
    }

    if (fileToRead.is_open()) {
        while (getline(fileToRead,line)) {
            fileContent += line + "\n";
        }
        fileToRead.close();
    }
    if(fileContent.empty()) {
        return {""};
    }
        // eliminates endl at the endl
    return fileContent.substr(0, fileContent.size() - 1);
}
FILE *RsaKeyFileIOInterface::getFileStructFromPath(std::string filepath, std::string modes) {
    FILE *fp = fopen(filepath.c_str(), modes.c_str());
    if(fp == nullptr) {
        throw std::runtime_error("KeyIOInterface: Cannot open file");
    }
    return fp;
}
RSA *RsaKeyFileIOInterface::readPublicKeyFromFpAndClose(FILE **fp) {
    auto rsa = PEM_read_RSA_PUBKEY(*fp, nullptr, nullptr, nullptr);
    fclose(*fp);
    if(!rsa) {
        throw std::runtime_error("KeyIOInterface: Could not read pubkey from file");
    }
    return rsa;
}

RSA *RsaKeyFileIOInterface::readPrivateKeyFromFpAndClose(FILE **fp) {
    auto rsa = PEM_read_RSAPrivateKey(*fp, nullptr, nullptr, nullptr);
    fclose(*fp);
    if(!rsa) {
        throw std::runtime_error("KeyIOInterface: Could not read private key from file");
    }
    return rsa;
}

void RsaKeyFileIOInterface::writePublicKeyToFile(std::string filepath, std::string mode, RSA *r, bool overwrite) {
    auto fp = getFileStructFromPath(filepath, mode);
    auto success = PEM_write_RSA_PUBKEY(fp, r);
    fclose(fp);

    if(!success) {
        throw std::runtime_error("KeyIOInterface: Failed to write public key");
    }
}
void RsaKeyFileIOInterface::writePrivateKeyToFile(std::string filepath, std::string mode, RSA *r, bool overwrite) {
    auto result = write(r);
    if(result == 0) {
        throw std::runtime_error("KeyIOInterface: Write key to partition failed");
    }

    if(result == 1) {
        throw std::runtime_error("KeyIOInterface: Partition full");
    }

    throwIfOverwriteForbidden(filepath, overwrite);

    std::ofstream os;
    os.open(filepath);
    os << result;
    os.close();

}
void RsaKeyFileIOInterface::writeToFile(std::string filepath, std::string data, bool overwrite) {

    throwIfOverwriteForbidden(filepath, overwrite);

    std::ofstream myfile;
    myfile.open (filepath);
    myfile << data;
    myfile.close();
}

void RsaKeyFileIOInterface::removePrivateKey(std::string privateKeyPath) {
    auto result = remove(privateKeyPath.c_str());
    if(result == -1) {
        throw std::runtime_error("KeyIOInterface: Failed to remove private key");
    }
}

void RsaKeyFileIOInterface::removePublicKey(std::string publicKeyPath) {
    auto move_call = "cp " + publicKeyPath + " " + publicKeyPath + ".bak";
    if(system(move_call.c_str()) == 256) {
        throw std::runtime_error("KeyIOInterface: Failed to remove public key");
    }
    auto rm_call = "rm " + publicKeyPath;
    if(system(rm_call.c_str()) == -1) {
        throw std::runtime_error("KeyIOInterface: Failed to remove public key");
    }
}

std::string RsaKeyFileIOInterface::getPrivateKey(std::string filepathWithPrvKeyId) {
    char* prvKey = NULL;
    auto ret = get(filepathWithPrvKeyId.c_str(), &prvKey);
    if(ret != 0) {
        throw std::runtime_error("KeyIOInterface: Cannot get private key");
    }
    return std::string(prvKey);
}

void RsaKeyFileIOInterface::throwIfOverwriteForbidden(std::string filepath, bool overwrite) {
    std::ifstream f(filepath);
    if(f.good() && !overwrite) {
        throw std::runtime_error("KeyIOInterface: Overwrite forbidden!");
    }
}

void RsaKeyFileIOInterface::throwIfCannotRemoveFile(std::string filepath) {
    std::ifstream f(filepath);
    if(!f.good()) {
        throw std::runtime_error("KeyIOInterface: File is not good");
    }
}
