//
// Created by robert on 26.05.2020.
//

#include "../include/OpenSSLHandler.h"

std::string OpenSSLHandler::sign(RSA *r, std::string messageToSign) {

    std::shared_ptr<EVP_MD_CTX> ctxRSA(EVP_MD_CTX_create(), [](auto ctx) { EVP_MD_CTX_destroy(ctx); });
    std::shared_ptr<EVP_PKEY> prvKey(EVP_PKEY_new(), [](auto key) { EVP_PKEY_free(key); });
    EVP_PKEY_assign_RSA(prvKey.get(), r);
    if (EVP_DigestSignInit(ctxRSA.get(), nullptr, EVP_sha256(), nullptr, prvKey.get()) != 1) {
        throw std::runtime_error("OpenSSLHandler: Digest init failed");
    }
    if (EVP_DigestSignUpdate(ctxRSA.get(), messageToSign.c_str(), messageToSign.length()) != 1) {
        throw std::runtime_error("OpenSSLHandler: Digest sign update failed");
    }
    auto lenAfterSign = std::make_shared<size_t>();
    if (EVP_DigestSignFinal(ctxRSA.get(), nullptr, lenAfterSign.get()) != 1) {
        throw std::runtime_error("OpenSSLHandler: Digest sign final failed");
    }
    std::string encryptedMessage(*lenAfterSign, ' ');
    if (EVP_DigestSignFinal(ctxRSA.get(), (unsigned char*)encryptedMessage.c_str(), lenAfterSign.get()) != 1) {
        throw std::runtime_error("OpenSSLHandler: Digest init failed");
    }
    return encryptedMessage;
}

bool OpenSSLHandler::checkSignature(RSA *rsa, std::string hash, std::string msg) {
    std::shared_ptr<EVP_PKEY> pubKey(EVP_PKEY_new(), [](auto key) { EVP_PKEY_free(key); });
    EVP_PKEY_assign_RSA(pubKey.get(), rsa);
    std::shared_ptr<EVP_MD_CTX> ctxRSA(EVP_MD_CTX_create(), [](auto ctx) { EVP_MD_CTX_destroy(ctx); });

    if(EVP_DigestVerifyInit(ctxRSA.get(),nullptr, EVP_sha256(),nullptr,pubKey.get()) != 1) {
        throw std::runtime_error("OpenSSLHandler: Digest verify init failed");
    }

    if(EVP_DigestVerifyUpdate(ctxRSA.get(), msg.c_str(), msg.length()) != 1) {
        throw std::runtime_error("OpenSSLHandler: Digest verify update failed");
    }
    auto ret = EVP_DigestVerifyFinal(ctxRSA.get(), (unsigned char* )hash.c_str(), hash.length());

    if(ret == 1) {
        return true;
    }

    if(ret == 0) {
        return false;
    }

    throw std::runtime_error("OpenSSLHandler: Verification failed");
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
    unsigned char input[BUF_SIZE];
    unsigned char output[BUF_SIZE + EVP_CIPHER_block_size(EVP_aes_256_cbc())];

    int outputSize;
    std::shared_ptr<EVP_CIPHER_CTX> ctx(EVP_CIPHER_CTX_new(), [](auto ctx){ EVP_CIPHER_CTX_cleanup(ctx); });

    if(!EVP_CipherInit_ex(ctx.get(), EVP_aes_256_cbc(), nullptr, nullptr, nullptr, mode)){
        throw std::runtime_error("OpenSSLHandler: cannot create cipher ctx");
    }

    OPENSSL_assert(EVP_CIPHER_CTX_key_length(ctx.get()) == AES_256_KEY_SIZE);
    OPENSSL_assert(EVP_CIPHER_CTX_iv_length(ctx.get()) == AES_BLOCK_SIZE);

    if(!EVP_CipherInit_ex(ctx.get(), nullptr, nullptr, (unsigned char*)config->key, (unsigned char*)config->iv, mode)){
        throw std::runtime_error("OpenSSLHandler: cannot create cipher ctx");
    }

    int maxIterations = MAX_FILE_READ_TRIALS;
    while(maxIterations--){
        int bytesRead = fread(input, sizeof(unsigned char), BUF_SIZE, ifp);

        if (ferror(ifp)){
            throw std::runtime_error("OpenSSLHandler: cannot read input file");
        }

        if(!EVP_CipherUpdate(ctx.get(), output, &outputSize, input, bytesRead)){
            throw std::runtime_error("OpenSSLHandler: cannot encrypt/decrypt input file");
        }
        fwrite(output, sizeof(unsigned char), outputSize, ofp);
        if (ferror(ofp)) {
            throw std::runtime_error("OpenSSLHandler: cannot write to output file");
        }

        if(bytesRead < BUF_SIZE) {
            break;
        }

    }
    assert(maxIterations > 0);
    if(!EVP_CipherFinal_ex(ctx.get(), output, &outputSize)){
        throw std::runtime_error("OpenSSLHandler: cannot encrypt last block of file");
    }
    fwrite(output, sizeof(unsigned char), outputSize, ofp);
    if (ferror(ofp)) {
        throw std::runtime_error("OpenSSLHandler: cannot write last block to output file");
    }
}
