#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN  // in only one cpp file
#include <boost/test/unit_test.hpp>
#include "../include/TerminalEmulation.h"
#include "../include/Statement.h"
#include "../include/Parser.h"
#include <memory>
#include <iostream>
namespace testHelpers {
    std::string toString(std::vector<std::string> input) {
        std::string serialisedArgs;
        // do not serialise program name
        for(int i = 1; i < input.size() -1; i++) {
            serialisedArgs += input[i];
            serialisedArgs += " ";
        }
        // do not add whitespace after last arg
        serialisedArgs += input[input.size()-1];
        return serialisedArgs;
    }
};
BOOST_AUTO_TEST_CASE(INTEGRATION_TEST_0)
{
    std::vector<std::string> input {
            "program",
            "create-key",
            "SHA",
            "2048",
            "/home/r.ostoja",
            "123"
    };
    TerminalEmulation terminalEmulation(input);
    auto args = terminalEmulation.getArgs();
    auto argc = args.argc;
    auto argv = args.argv;

    auto syntaxAnalyser = std::make_shared<SyntaxAnalyser>(argc, argv);
    auto parser = std::make_unique<Parser>(syntaxAnalyser);
    parser->parse();
    auto statement = parser->getCurrentParsedStatement();
    auto serialisedInput = testHelpers::toString(input);
    BOOST_CHECK_EQUAL(statement, serialisedInput);
}
BOOST_AUTO_TEST_CASE(INTEGRATION_TEST_1)
{
    std::vector<std::string> input {
            "program",
            "get-private-key",
            "413243",
            "/home/abc",
    };
    TerminalEmulation terminalEmulation(input);
    auto args = terminalEmulation.getArgs();
    auto argc = args.argc;
    auto argv = args.argv;

    auto syntaxAnalyser = std::make_shared<SyntaxAnalyser>(argc, argv);
    auto parser = std::make_unique<Parser>(syntaxAnalyser);
    parser->parse();
    auto statement = parser->getCurrentParsedStatement();
    auto serialisedInput = testHelpers::toString(input);
    BOOST_CHECK_EQUAL(statement, serialisedInput);
}
