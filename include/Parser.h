//
// Created by robert on 17.05.2020.
//

#ifndef KEYREPO_PARSER_H
#define KEYREPO_PARSER_H
#include "Lexer.h"
#include "Token.h"
#include <memory>
#include "Statement.h"

class Parser {

private:
    std::shared_ptr<Lexer> syntaxAnalyser;
    std::shared_ptr<Statement> currentParsedStatement;
    void generateCreateKeyOption();
    void generateGetPrivateKeyOption();
    void generateDeleteKeyOption();
    void generateSignOption();
    void generateCheckSignatureOption();
    void generateEncryptFileOption();
    void generateDecryptFileOption();
    void generateGetModOption();
    void generateChangeModOption();
    void generateHelpOption();


    std::map<std::string, std::function<void()>> tokenGenerator {
            {"create-key", [&]() { return generateCreateKeyOption(); }},
            {"get-private-key", [&]() { return generateGetPrivateKeyOption(); }},
            {"delete-key", [&]() { return generateDeleteKeyOption(); }},
            {"sign", [&]() { return generateSignOption(); }},
            {"check-signature", [&]() { return generateCheckSignatureOption(); }},
            {"encrypt-file", [&]() { return generateEncryptFileOption(); }},
            {"decrypt-file", [&]() { return generateDecryptFileOption(); }},
            {"gmod", [&]() { return generateGetModOption(); }},
            {"chmod", [&]() { return generateChangeModOption(); }},
            {"-h", [&]() { return generateHelpOption(); }},
            {"--help", [&]() { return generateHelpOption(); }},
    };
public:
    Parser(std::shared_ptr<Lexer> syntaxAnalyser) {
        this->syntaxAnalyser = syntaxAnalyser;
    }
    void parse();
    std::string getCurrentParsedStatementStr() { return currentParsedStatement->toString(); }
    std::shared_ptr<Statement> getCurrentParsedStatement() { return currentParsedStatement; }
};


#endif //KEYREPO_PARSER_H
