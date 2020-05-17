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
//    Token generatePrivateKeyOption();
//    Token generateDeleteKeyOption();
//    Token generateSignOption();
//    Token generateCheckSignatureOption();
//    Token generateEncryptFileOption();

    std::map<std::string, std::function<void()>> tokenGenerator {
            {"create-key", [&]() { return generateCreateKeyOption(); }},
            {"create-key", [&]() { return generateCreateKeyOption(); }},
            {"create-key", [&]() { return generateCreateKeyOption(); }},
            {"create-key", [&]() { return generateCreateKeyOption(); }},
            {"create-key", [&]() { return generateCreateKeyOption(); }},
            {"create-key", [&]() { return generateCreateKeyOption(); }},
    };
public:
    Parser(std::shared_ptr<SyntaxAnalyser> syntaxAnalyser) {
        this->syntaxAnalyser = syntaxAnalyser;
    }
    void parse();
    std::string getCurrentParsedStatement() { return currentParsedStatement->toString(); }
};


#endif //KEYREPO_PARSER_H
