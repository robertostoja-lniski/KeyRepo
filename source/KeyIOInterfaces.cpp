//
// Created by robert on 25.05.2020.
//

#include "../include/KeyIOInterfaces.h"

RSA* RsaKeyFileIOInterface::readPublicKeyFromFile(std::string filepath) {
    auto fp = getFileStructFromPath(filepath, "r");
    return readPublicKeyFromFpAndClose(&fp);
}
RSA* RsaKeyFileIOInterface::readPrivateKey(std::string filepath) {

    char* cPrvKey = NULL;
    auto keyId = readFromFile(filepath.c_str());

    uint64_t id;
    std::istringstream iss(keyId);
    iss >> id;

    uint64_t keyLen;
    if(readKey(&id, &cPrvKey, &keyLen) < 0) {
        throw std::runtime_error("KeyIOInterface: Failed to read private key");
    }

    BIO* bo = BIO_new( BIO_s_mem() );
    if(bo == nullptr) {
        throw std::runtime_error("KeyIOInterface: Failed to read private key");
    }
    if(BIO_write( bo, cPrvKey,strlen(cPrvKey)) <=0) {
        throw std::runtime_error("KeyIOInterface: Failed to read private key");
    }

    EVP_PKEY* pkey = nullptr;
    if(!PEM_read_bio_PrivateKey( bo, &pkey, nullptr, nullptr )){
        throw std::runtime_error("KeyIOInterface: Failed to read private key");
    }

    BIO_free(bo);

    auto funcRet = EVP_PKEY_get1_RSA( pkey );
    if(!funcRet) {
        throw std::runtime_error("KeyIOInterface: Failed to read private key");
    }

    return  funcRet;
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
std::string RsaKeyFileIOInterface::readFromFile(std::string filepath) {
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

void RsaKeyFileIOInterface::writePublicKeyToFile(std::string filepath, std::string mode, RSA *r, bool overwrite) {
    auto fp = getFileStructFromPath(filepath, mode);
    auto success = PEM_write_RSA_PUBKEY(fp, r);
    fclose(fp);

    if(!success) {
        throw std::runtime_error("KeyIOInterface: Failed to write public key");
    }
}
void RsaKeyFileIOInterface::writePrivateKeyToFile(std::string filepath, std::string mode, RSA *r, bool overwrite) {
    uint64_t* id = nullptr;

    writeKeyToTemporaryFile(r);
    KeyNode* keyNode;
    auto node = generateKeyNodeFromKeyInFile(&keyNode);
    auto result = write(keyNode->keyContent, keyNode->keySize, &id);
    free(keyNode);

    if(result == -1) {
        throw std::runtime_error("KeyIOInterface: Write key to partition failed");
    }

    if(result == -2) {
        throw std::runtime_error("KeyIOInterface: Partition full");
    }

    if(id == nullptr) {
        throw std::runtime_error("Unhandled Error!");
    }

    throwIfOverwriteForbidden(filepath, overwrite);

    std::ofstream os;
    os.open(filepath);
    os << *id;
    os.close();

    free(id);
}
void RsaKeyFileIOInterface::writeToFile(std::string filepath, std::string data, bool overwrite) {

    throwIfOverwriteForbidden(filepath, overwrite);

    std::ofstream myfile;
    myfile.open (filepath);
    myfile << data;
    myfile.close();
}

void RsaKeyFileIOInterface::removePrivateKey(std::string privateKeyPath) {

    auto keyId = readFromFile(privateKeyPath.c_str());

    uint64_t id;
    std::istringstream iss(keyId);
    iss >> id;

    auto result = remove(&id, privateKeyPath.c_str());
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
    char* prvKey = nullptr;

    auto keyId = readFromFile(std::move(filepathWithPrvKeyId));

    uint64_t id;
    std::istringstream iss(keyId);
    iss >> id;

    auto ret = get(&id, &prvKey);
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

int RsaKeyFileIOInterface::getKeyMode(std::string filepathWithPrvKeyId) {

    int* modes = nullptr;

    auto keyId = readFromFile(std::move(filepathWithPrvKeyId));

    uint64_t id;
    std::istringstream iss(keyId);
    iss >> id;

    auto ret = getMode(&id, &modes);
    if(ret != 0) {
        throw std::runtime_error("KeyIOInterface: Cannot get private key modes");
    }

    int funcRet = *modes;
    free(modes);
    return funcRet;

}

void RsaKeyFileIOInterface::changeKeyMode(std::string filepathWithPrvKeyId, int newMode) {

    auto keyId = readFromFile(std::move(filepathWithPrvKeyId));

    uint64_t id;
    std::istringstream iss(keyId);
    iss >> id;

    auto ret = setMode(&id, &newMode);
    if(ret != 0) {
        throw std::runtime_error("KeyIOInterface: Cannot get private key modes");
    }
}

int RsaKeyFileIOInterface::generateKeyNodeFromKeyInFile(KeyNode** keyNode) {
    std::string line;

    FILE *fp;
    size_t buffSize = 4096 * 16;
    char str[buffSize];
    memset(str, 0x00, buffSize);

    fp = fopen(tmpKeyStorage, "r");
    if (fp == NULL){
        return -1;
    }

    while (fgets(str, buffSize, fp) != NULL) {
        line += str;
        memset(str, 0x00, buffSize);
    }
    fclose(fp);


    size_t allocationSize = sizeof(KeyNode);
    if(line.size() > 4096) {
        allocationSize += line.size() - 4096;
    }

    KeyNode* keyData = (KeyNode* )malloc(allocationSize);
    if(keyData == NULL) {
        return -1;
    }

    memcpy(keyData->keyContent, line.c_str(), line.size());
    keyData->keySize = line.size();

    *keyNode = keyData;
    return 0;
}

int RsaKeyFileIOInterface::writeKeyToTemporaryFile(RSA* r) {
    FILE *fp = fopen(tmpKeyStorage, "wb");
    if(fp == nullptr) {
        return 1;
    }
    int success = PEM_write_RSAPrivateKey(fp, r, nullptr, nullptr, 0, nullptr, nullptr);
    if(!success) {
        return 1;
    }
    fclose(fp);
}
