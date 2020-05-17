//
// Created by robert on 17.05.2020.
//

#include "../include/Statement.h"

std::ostream& operator<<(std::ostream& os, const CreateKeyStatement& dt)
{
    os << "Create key with: " << dt.algorithm << " algorithm, " << dt.keyLen << " length of key "
       << dt.pubKeyPath << " path to public key, " << dt.privateKeyId << " and private key id";
    return os;
}
