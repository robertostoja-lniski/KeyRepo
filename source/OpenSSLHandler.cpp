//
// Created by robert on 26.05.2020.
//

#include "../include/OpenSSLHandler.h"

std::string OpenSSLHandler::sign(RSA *r, std::string messageToSign) {

    EVP_MD_CTX* m_RSASignCtx = EVP_MD_CTX_create();
    EVP_PKEY* prv  = EVP_PKEY_new();
    EVP_PKEY_assign_RSA(prv, r);
    if (EVP_DigestSignInit(m_RSASignCtx,NULL, EVP_sha256(), NULL, prv) != 1) {
        throw std::runtime_error("OpenSSLHandler: Digest init failed");
    }
    if (EVP_DigestSignUpdate(m_RSASignCtx, messageToSign.c_str(), messageToSign.length()) != 1) {
        throw std::runtime_error("OpenSSLHandler: Digest sign update failed");
    }
    auto lenAfterSign = std::make_shared<size_t>();
    if (EVP_DigestSignFinal(m_RSASignCtx, NULL, lenAfterSign.get()) != 1) {
        throw std::runtime_error("OpenSSLHandler: Digest sign final failed");
    }
    std::string encryptedMessage(*lenAfterSign, ' ');
    if (EVP_DigestSignFinal(m_RSASignCtx, (unsigned char*)encryptedMessage.c_str(), lenAfterSign.get()) != 1) {
        throw std::runtime_error("OpenSSLHandler: Digest init failed");
    }
    EVP_MD_CTX_destroy(m_RSASignCtx);
    return encryptedMessage;
}

bool OpenSSLHandler::checkSignature(RSA *rsa, std::string hash, std::string msg) {
    EVP_PKEY* pubKey  = EVP_PKEY_new();
    EVP_PKEY_assign_RSA(pubKey, rsa);
    EVP_MD_CTX* m_RSAVerifyCtx = EVP_MD_CTX_create();

    if(EVP_DigestVerifyInit(m_RSAVerifyCtx,NULL, EVP_sha256(),NULL,pubKey) != 1) {
        throw std::runtime_error("OpenSSLHandler: Digest verify init failed");
    }
    if(EVP_DigestVerifyUpdate(m_RSAVerifyCtx, msg.c_str(), msg.length()) != 1) {
        throw std::runtime_error("OpenSSLHandler: Digest verify update failed");
    }
    if(EVP_DigestVerifyFinal(m_RSAVerifyCtx, (unsigned char* )hash.c_str(), hash.length()) != 1) {
        throw std::runtime_error("OpenSSLHandler: Verification failed");
    }
    EVP_MD_CTX_destroy(m_RSAVerifyCtx);
    return true;
}

std::shared_ptr<RSA> OpenSSLHandler::createKey(int keyLen) {
    std::shared_ptr<BIGNUM> bne(BN_new(), BN_free);
    auto bnSuccess = BN_set_word(bne.get(),RSA_F4);
    if(!bnSuccess) {
        throw std::runtime_error("OpenSSLHandler: Cannot set big num");
    }
    std::shared_ptr<RSA> r(RSA_new(), RSA_free);
    auto rsaSuccess = RSA_generate_key_ex(r.get(), keyLen, bne.get(), nullptr);
    if(!rsaSuccess) {
        throw std::runtime_error("OpenSSLHandler: Cannot generate RSA keys");
    }
    return r;
}

void OpenSSLHandler::encrypt(std::shared_ptr<Config> config, FILE *ifp, FILE *ofp) {
    encrypt_or_decrypt(config, ifp, ofp, 1);
}


void OpenSSLHandler::decrypt(std::shared_ptr<Config> config, FILE *ifp, FILE *ofp) {
    encrypt_or_decrypt(config, ifp, ofp, 0);
}

void OpenSSLHandler::encrypt_or_decrypt(std::shared_ptr<Config> config, FILE *ifp, FILE *ofp, int mode) {
    size_t BUFSIZE = 4096;
    unsigned char input[BUFSIZE];
    unsigned char output[BUFSIZE + EVP_CIPHER_block_size(EVP_aes_256_cbc())];

    int outputSize;
    EVP_CIPHER_CTX *ctx;

    ctx = EVP_CIPHER_CTX_new();
    if(ctx == nullptr){
        throw std::runtime_error("OpenSSLHandler: cannot create cipher ctx");
    }

    if(!EVP_CipherInit_ex(ctx, EVP_aes_256_cbc(), nullptr, nullptr, nullptr, mode)){
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("OpenSSLHandler: cannot create cipher ctx");
    }

    OPENSSL_assert(EVP_CIPHER_CTX_key_length(ctx) == AES_256_KEY_SIZE);
    OPENSSL_assert(EVP_CIPHER_CTX_iv_length(ctx) == AES_BLOCK_SIZE);

    auto dummy = mode;
    if(!EVP_CipherInit_ex(ctx, nullptr, nullptr, (unsigned char*)config->key, (unsigned char*)config->iv, mode)){
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("OpenSSLHandler: cannot create cipher ctx");
    }

    int maxIterations = 1000;
    while(maxIterations--){
        int bytesRead = fread(input, sizeof(unsigned char), BUFSIZE, ifp);

        if (ferror(ifp)){
            throw std::runtime_error("OpenSSLHandler: cannot read input file");
            EVP_CIPHER_CTX_cleanup(ctx);
        }

        if(!EVP_CipherUpdate(ctx, output, &outputSize, input, bytesRead)){
            throw std::runtime_error("OpenSSLHandler: cannot encrypt/decrypt input file");
            EVP_CIPHER_CTX_cleanup(ctx);
        }
        fwrite(output, sizeof(unsigned char), outputSize, ofp);
        if (ferror(ofp)) {
            throw std::runtime_error("OpenSSLHandler: cannot write to output file");
            EVP_CIPHER_CTX_cleanup(ctx);
        }

        if(bytesRead < BUFSIZE) {
            break;
        }

    }

    assert(maxIterations > 0);
    if(!EVP_CipherFinal_ex(ctx, output, &outputSize)){
        throw std::runtime_error("OpenSSLHandler: cannot encrypt last block of file");
        EVP_CIPHER_CTX_cleanup(ctx);
    }
    fwrite(output, sizeof(unsigned char), outputSize, ofp);
    if (ferror(ofp)) {
        throw std::runtime_error("OpenSSLHandler: cannot write last block to output file");
        EVP_CIPHER_CTX_cleanup(ctx);
    }
    EVP_CIPHER_CTX_cleanup(ctx);
}
