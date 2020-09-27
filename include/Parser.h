//
// Created by robert on 17.05.2020.
//

#ifndef KEYREPO_PARSER_H
#define KEYREPO_PARSER_H
#include "SyntaxAnalyser.h"
#include "Token.h"
#include <memory>
#include "Statement.h"

class Parser {

private:
    std::shared_ptr<SyntaxAnalyser> syntaxAnalyser;
    std::shared_ptr<Statement> currentParsedStatement;
    void generateCreateKeyOption();
    void generateGetPrivateKeyOption();
    void generateDeleteKeyOption();
    void generateSignOption();
    void generateCheckSignatureOption();
    void generateEncryptFileOption();
    void generateDecryptFileOption();
    void generateHelpOption();

    std::map<std::string, std::function<void()>> tokenGenerator {
            {"create-key", [&]() { return generateCreateKeyOption(); }},
            {"get-private-key", [&]() { return generateGetPrivateKeyOption(); }},
            {"delete-key", [&]() { return generateDeleteKeyOption(); }},
            {"sign", [&]() { return generateSignOption(); }},
            {"check-signature", [&]() { return generateCheckSignatureOption(); }},
            {"encrypt-file", [&]() { return generateEncryptFileOption(); }},
            {"decrypt-file", [&]() { return generateDecryptFileOption(); }},
            {"-h", [&]() { return generateHelpOption(); }},
            {"--help", [&]() { return generateHelpOption(); }},
    };
public:
    Parser(std::shared_ptr<SyntaxAnalyser> syntaxAnalyser) {
        this->syntaxAnalyser = syntaxAnalyser;
    }
    void parse();
    std::string getCurrentParsedStatementStr() { return currentParsedStatement->toString(); }
    std::shared_ptr<Statement> getCurrentParsedStatement() { return currentParsedStatement; }
};


#endif //KEYREPO_PARSER_H
