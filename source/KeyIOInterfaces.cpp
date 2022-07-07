//
// Created by robert on 25.05.2020.
//

#include "../include/KeyIOInterfaces.h"

void KeyPartitionIOInterface::printFile(std::string filepath) {
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
std::string KeyPartitionIOInterface::readFromFile(std::string filepath) {
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
FILE *KeyPartitionIOInterface::getFileStructFromPath(std::string filepath, std::string modes) {
    FILE *fp = fopen(filepath.c_str(), modes.c_str());
    if(fp == nullptr) {
        throw std::runtime_error("KeyIOInterface: Cannot open file");
    }
    return fp;
}

void KeyPartitionIOInterface::writeToFile(std::string filepath, std::string data, bool overwrite) {

    throwIfOverwriteForbidden(filepath, overwrite);

    std::ofstream myfile;
    myfile.open (filepath);
    myfile << data;
    myfile.close();
}

void KeyPartitionIOInterface::removePrivateKey(std::string privateKeyPath) {

    auto keyId = readFromFile(privateKeyPath.c_str());

    uint64_t id;
    std::istringstream iss(keyId);
    iss >> id;

    // TODO in devel mode
    auto result = remove_key(id, "dummy");
    if(result < 0) {
        throw std::runtime_error("KeyIOInterface: Failed to remove private key");
    }
}

void KeyPartitionIOInterface::removePublicKey(std::string publicKeyPath) {
    auto move_call = "cp " + publicKeyPath + " " + publicKeyPath + ".bak";
    if(system(move_call.c_str()) == 256) {
        throw std::runtime_error("KeyIOInterface: Failed to remove public key");
    }
    auto rm_call = "rm " + publicKeyPath;
    if(system(rm_call.c_str()) == -1) {
        throw std::runtime_error("KeyIOInterface: Failed to remove public key");
    }
}

std::string KeyPartitionIOInterface::getPrivateKey(std::string filepathWithPrvKeyId) {

    auto keyId = readFromFile(std::move(filepathWithPrvKeyId));

    uint64_t id;
    std::istringstream iss(keyId);
    iss >> id;

    uint64_t keyLen;
//    TODO in devel mode
    auto getSizeRet = get_key_size(id, "dummy", &keyLen);
    if(getSizeRet != 0) {
        throw std::runtime_error("KeyIOInterface: Cannot get private key");
    }

    char* prvKey = (char* )malloc(keyLen + 1);
    if(!prvKey) {
        throw std::runtime_error("KeyIOInterface: Cannot get private key");
    }

//    TODO in devel mode
    auto ret = read_key(id, "dummy", prvKey, keyLen);
    if(ret != 0) {
        throw std::runtime_error("KeyIOInterface: Cannot get private key");
    }

    auto keyStr = std::string(prvKey);
    free(prvKey);
    return keyStr;
}

void KeyPartitionIOInterface::throwIfOverwriteForbidden(std::string filepath, bool overwrite) {
    std::ifstream f(filepath);
    if(f.good() && !overwrite) {
        throw std::runtime_error("KeyIOInterface: Overwrite forbidden!");
    }
}

void KeyPartitionIOInterface::throwIfCannotRemoveFile(std::string filepath) {
    std::ifstream f(filepath);
    if(!f.good()) {
        throw std::runtime_error("KeyIOInterface: File is not good");
    }
}

int KeyPartitionIOInterface::getKeyMode(std::string filepathWithPrvKeyId) {

    int modes;

    auto keyId = readFromFile(std::move(filepathWithPrvKeyId));

    uint64_t id;
    std::istringstream iss(keyId);
    iss >> id;

//    TODO in devel mode
    auto ret = get_mode(id, "dummy", &modes);
    if(ret != 0) {
        throw std::runtime_error("KeyIOInterface: Cannot get private key modes");
    }

    return modes;

}

void KeyPartitionIOInterface::changeKeyMode(std::string filepathWithPrvKeyId, int newMode) {

    auto keyId = readFromFile(std::move(filepathWithPrvKeyId));

    uint64_t id;
    std::istringstream iss(keyId);
    iss >> id;

//    TODO in devel mode
    auto ret = set_mode(id, "dummy", newMode);
    if(ret != 0) {
        throw std::runtime_error("KeyIOInterface: Cannot get private key modes");
    }
}

boost::any KeyPartitionIOInterface::protectedReadPublicKeyFromFile(std::string filepath) {
    auto fp = getFileStructFromPath(filepath, "r");
    auto rsa = PEM_read_RSA_PUBKEY(fp, nullptr, nullptr, nullptr);
    fclose(fp);
    if(!rsa) {
        throw std::runtime_error("KeyIOInterface: Could not read pubkey from file");
    }
    return rsa;
}

boost::any KeyPartitionIOInterface::protectedReadPrivateKeyFromFile(std::string filepath) {

    auto keyId = readFromFile(filepath.c_str());

    uint64_t id;
    std::istringstream iss(keyId);
    iss >> id;

    uint64_t keyLen;

//    TODO in devel mode
    auto getSizeRet = get_key_size(id, "dummy", &keyLen);
    if(getSizeRet !=0 ) {
        throw std::runtime_error("KeyIOInterface: Cannot get private key");
    }

    char* prvKey = (char* )malloc(keyLen + 1);
    if(!prvKey) {
        throw std::runtime_error("KeyIOInterface: Cannot get private key");
    }

//    TODO in devel mode
    auto readKeyRet = read_key(id, "dummy", prvKey, keyLen);
    if(readKeyRet < 0) {
        throw std::runtime_error("KeyIOInterface: Cannot get private key");
    }
//    std::cout << prvKey;
    BIO* bo = BIO_new( BIO_s_mem() );
    if(bo == nullptr) {
        throw std::runtime_error("KeyIOInterface: Failed to read private key");
    }
    if(BIO_write( bo, prvKey,strlen(prvKey)) <= 0) {
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

    return funcRet;
}

void KeyPartitionIOInterface::protectedWritePublicKeyToFile(std::string filepath, std::string mode, boost::any r, bool overwrite) {
    auto fp = getFileStructFromPath(filepath, mode);
    auto success = PEM_write_RSA_PUBKEY(fp, boost::any_cast<RSA*>(r));
    fclose(fp);

    if(!success) {
        throw std::runtime_error("KeyIOInterface: Failed to write public key");
    }
}

void KeyPartitionIOInterface::protectedWritePrivateKeyToFile(std::string filepath, std::string mode, boost::any r, bool overwrite) {
    uint64_t id;

    std::shared_ptr<BIO> bio(BIO_new(BIO_s_mem()), BIO_free);
    if(!bio) {
        throw std::runtime_error("KeyIOInterface: OpenSSL BIO init error");
    }

    if(!PEM_write_bio_RSAPrivateKey(bio.get(), boost::any_cast<RSA*>(r), nullptr, nullptr, 0, nullptr, nullptr)) {
        throw std::runtime_error("KeyIOInterface: OpenSSL BIO write private key error");
    }

    auto keylen = BIO_pending(bio.get());
    char* pem_key = (char* )malloc(keylen+1);
    if(!pem_key) {
        throw std::runtime_error("KeyIOInterface: OpenSSL BIO malloc error");
    }

    if(BIO_read(bio.get(), pem_key, keylen) <=0 ){
        throw std::runtime_error("KeyIOInterface: Unhandled OpenSSL BIO read error");
    }

    // TODO in devel mode
    auto result = write_key(pem_key, "dummy", keylen, &id, KEY_TYPE_RSA);

    if(result == -1) {
        throw std::runtime_error("KeyIOInterface: Write key to partition failed");
    }

    if(result == -5) {
        throw std::runtime_error("KeyIOInterface: Partition full");
    }

    throwIfOverwriteForbidden(filepath, overwrite);

    std::ofstream os;
    os.open(filepath);
    os << id;
    os.close();

}