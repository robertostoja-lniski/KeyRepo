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

std::shared_ptr<RSA> OpenSSLHandler::createKey(int keyLen, const std::string& pubKeyPath, const std::string& prvKeyIdPath) {
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
