#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN  // in only one cpp file
#include <boost/test/unit_test.hpp>
#include "../include/TerminalEmulation.h"
#include "../include/Executor.h"

extern "C" {
    #include "../include/KeyRepoSyscallWrapper.h"
};

#include <memory>

#define LONG_RUN 1
#define RAW_PARTITION 1

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

    std::string readFileIntoString(std::string path){
        std::string fileContent;
        std::string line;
        std::ifstream fileToRead(path);
        if (fileToRead.is_open()) {
            while (getline(fileToRead,line)) {
                fileContent += line;
            }
            fileToRead.close();
        }
        return fileContent;
    }

};

BOOST_AUTO_TEST_CASE(PRETEST)
{
    system("rm -rf ~/.keyPartitionV2/*");
}

BOOST_AUTO_TEST_CASE(FLAGS_UNIT_TEST_1)
{
    auto modHandler = std::make_unique<ModHandler>();
    auto flags = modHandler->stringToModeSetter("+rw").value;
    BOOST_CHECK_EQUAL(flags, 600);
}
BOOST_AUTO_TEST_CASE(FLAGS_UNIT_TEST_2)
{
    auto modHandler = std::make_unique<ModHandler>();
    auto flags = modHandler->stringToModeSetter("+r").value;
    BOOST_CHECK_EQUAL(flags, 400);
}
BOOST_AUTO_TEST_CASE(FLAGS_UNIT_TEST_3)
{
    auto modHandler = std::make_unique<ModHandler>();
    auto flags = modHandler->stringToModeSetter("+w").value;
    BOOST_CHECK_EQUAL(flags, 200);
}
BOOST_AUTO_TEST_CASE(FLAGS_UNIT_TEST_4)
{
    auto modHandler = std::make_unique<ModHandler>();
    auto flags = modHandler->stringToModeSetter("g+rw").value;
    BOOST_CHECK_EQUAL(flags, 60);
}
BOOST_AUTO_TEST_CASE(FLAGS_UNIT_TEST_5)
{
    auto modHandler = std::make_unique<ModHandler>();
    auto flags = modHandler->stringToModeSetter("g+r").value;
    BOOST_CHECK_EQUAL(flags, 40);
}
BOOST_AUTO_TEST_CASE(FLAGS_UNIT_TEST_6)
{
    auto modHandler = std::make_unique<ModHandler>();
    auto flags = modHandler->stringToModeSetter("g+w").value;
    BOOST_CHECK_EQUAL(flags, 20);
}
BOOST_AUTO_TEST_CASE(FLAGS_UNIT_TEST_7)
{
    auto modHandler = std::make_unique<ModHandler>();
    auto flags = modHandler->stringToModeSetter("o+r").value;
    BOOST_CHECK_EQUAL(flags, 4);
}
BOOST_AUTO_TEST_CASE(FLAGS_UNIT_TEST_8)
{
    auto modHandler = std::make_unique<ModHandler>();
    auto flags = modHandler->stringToModeSetter("o+w").value;
    BOOST_CHECK_EQUAL(flags, 2);
}
BOOST_AUTO_TEST_CASE(FLAGS_UNIT_TEST_9)
{
    auto modHandler = std::make_unique<ModHandler>();
    auto flags = modHandler->stringToModeSetter("o+rw").value;
    BOOST_CHECK_EQUAL(flags, 6);
}
BOOST_AUTO_TEST_CASE(FLAGS_UNIT_TEST_10)
{
    auto modHandler = std::make_unique<ModHandler>();
    auto flags = modHandler->stringToModeSetter("og+rw").value;
    BOOST_CHECK_EQUAL(flags, 66);
}
BOOST_AUTO_TEST_CASE(FLAGS_UNIT_TEST_11)
{
    auto modHandler = std::make_unique<ModHandler>();
    auto flags = modHandler->stringToModeSetter("ou+rw").value;
    BOOST_CHECK_EQUAL(flags, 606);
}
BOOST_AUTO_TEST_CASE(FLAGS_UNIT_TEST_12)
{
    auto modHandler = std::make_unique<ModHandler>();
    auto flags = modHandler->stringToModeSetter("gu+rw").value;
    BOOST_CHECK_EQUAL(flags, 660);
}
BOOST_AUTO_TEST_CASE(FLAGS_UNIT_TEST_13)
{
    auto modHandler = std::make_unique<ModHandler>();
    auto flags = modHandler->stringToModeSetter("a+rw").value;
    BOOST_CHECK_EQUAL(flags, 666);
}
BOOST_AUTO_TEST_CASE(FLAGS_UNIT_TEST_14)
{
    auto modHandler = std::make_unique<ModHandler>();
    auto flags = modHandler->stringToModeSetter("a+r").value;
    BOOST_CHECK_EQUAL(flags, 444);
}
BOOST_AUTO_TEST_CASE(FLAGS_UNIT_TEST_15)
{
    auto modHandler = std::make_unique<ModHandler>();
    auto flags = modHandler->stringToModeSetter("a+w").value;
    BOOST_CHECK_EQUAL(flags, 222);
}
BOOST_AUTO_TEST_CASE(FLAGS_UNIT_TEST_16)
{
    auto modHandler = std::make_unique<ModHandler>();
    auto flags = modHandler->stringToModeSetter("guo+rw").value;
    BOOST_CHECK_EQUAL(flags, 666);
}
BOOST_AUTO_TEST_CASE(FLAGS_UNIT_TEST_17)
{
    auto modHandler = std::make_unique<ModHandler>();
    auto flags = modHandler->stringToModeSetter("uog+r").value;
    BOOST_CHECK_EQUAL(flags, 444);
}
BOOST_AUTO_TEST_CASE(FLAGS_UNIT_TEST_18)
{
    auto modHandler = std::make_unique<ModHandler>();
    auto flags = modHandler->stringToModeSetter("oug+rw").value;
    BOOST_CHECK_EQUAL(flags, 666);
}

BOOST_AUTO_TEST_CASE(FLAGS_UNIT_TEST_19)
{
    auto modHandler = std::make_unique<ModHandler>();
    auto flags = modHandler->stringToModeSetter("-rw").value;
    BOOST_CHECK_EQUAL(flags, 600);
}
BOOST_AUTO_TEST_CASE(FLAGS_UNIT_TEST_20)
{
    auto modHandler = std::make_unique<ModHandler>();
    auto flags = modHandler->stringToModeSetter("-r").value;
    BOOST_CHECK_EQUAL(flags, 400);
}
BOOST_AUTO_TEST_CASE(FLAGS_UNIT_TEST_21)
{
    auto modHandler = std::make_unique<ModHandler>();
    auto flags = modHandler->stringToModeSetter("-w").value;
    BOOST_CHECK_EQUAL(flags, 200);
}
BOOST_AUTO_TEST_CASE(FLAGS_UNIT_TEST_22)
{
    auto modHandler = std::make_unique<ModHandler>();
    auto flags = modHandler->stringToModeSetter("g-rw").value;
    BOOST_CHECK_EQUAL(flags, 60);
}
BOOST_AUTO_TEST_CASE(FLAGS_UNIT_TEST_23)
{
    auto modHandler = std::make_unique<ModHandler>();
    auto flags = modHandler->stringToModeSetter("g-r").value;
    BOOST_CHECK_EQUAL(flags, 40);
}
BOOST_AUTO_TEST_CASE(FLAGS_UNIT_TEST_24)
{
    auto modHandler = std::make_unique<ModHandler>();
    auto action = modHandler->stringToModeSetter("g+w").action;
    BOOST_CHECK_EQUAL((int)action, 0);
}
BOOST_AUTO_TEST_CASE(FLAGS_UNIT_TEST_25)
{
    auto modHandler = std::make_unique<ModHandler>();
    auto action = modHandler->stringToModeSetter("o-r").action;
    BOOST_CHECK_EQUAL((int)action, 1);
}
BOOST_AUTO_TEST_CASE(FLAGS_UNIT_TEST_26)
{
    auto modHandler = std::make_unique<ModHandler>();
    auto action = modHandler->stringToModeSetter("o-w").action;
    BOOST_CHECK_EQUAL((int)action, 1);
}
BOOST_AUTO_TEST_CASE(FLAGS_UNIT_TEST_27)
{
    auto modHandler = std::make_unique<ModHandler>();
    auto action = modHandler->stringToModeSetter("777").action;
    BOOST_CHECK_EQUAL((int)action, 2);
}
BOOST_AUTO_TEST_CASE(FLAGS_UNIT_TEST_28)
{
    auto modHandler = std::make_unique<ModHandler>();
    auto action = modHandler->stringToModeSetter("000").action;
    BOOST_CHECK_EQUAL((int)action, 2);
}
BOOST_AUTO_TEST_CASE(FLAGS_UNIT_TEST_NEG_1)
{
    auto modHandler = std::make_unique<ModHandler>();
    int flags = 0;

    bool caught {false};
    try {
        flags = modHandler->stringToModeSetter("+").value;
    } catch (std::exception &e) {
        if(e.what() == std::string("ModeHandler: Wrong string flag format")) {
            caught = true;
        }
    }

    BOOST_CHECK_EQUAL(caught, true);
}
BOOST_AUTO_TEST_CASE(FLAGS_UNIT_TEST_NEG_2)
{
    auto modHandler = std::make_unique<ModHandler>();
    int flags = 0;

    bool caught {false};
    try {
        flags = modHandler->stringToModeSetter("a+").value;
    } catch (std::exception &e) {
        if(e.what() == std::string("ModeHandler: Wrong string flag format")) {
            caught = true;
        }
    }

    BOOST_CHECK_EQUAL(caught, true);
}
BOOST_AUTO_TEST_CASE(FLAGS_UNIT_TEST_NEG_3)
{
    auto modHandler = std::make_unique<ModHandler>();
    int flags = 0;

    bool caught {false};
    try {
        flags = modHandler->stringToModeSetter("au+").value;
    } catch (std::exception &e) {
        if(e.what() == std::string("ModeHandler: Wrong string flag format")) {
            caught = true;
        }
    }

    BOOST_CHECK_EQUAL(caught, true);
}
BOOST_AUTO_TEST_CASE(FLAGS_UNIT_TEST_NEG_4)
{
    auto modHandler = std::make_unique<ModHandler>();
    int flags = 0;

    bool caught {false};
    try {
        flags = modHandler->stringToModeSetter("rw").value;
    } catch (std::exception &e) {
        if(e.what() == std::string("ModeHandler: Wrong string flag format")) {
            caught = true;
        }
    }

    BOOST_CHECK_EQUAL(caught, true);
}
BOOST_AUTO_TEST_CASE(FLAGS_UNIT_TEST_NEG_5)
{
    auto modHandler = std::make_unique<ModHandler>();
    int flags = 0;

    bool caught {false};
    try {
        flags = modHandler->stringToModeSetter("1+1").value;
    } catch (std::exception &e) {
        if(e.what() == std::string("ModeHandler: Wrong string flag format")) {
            caught = true;
        }
    }

    BOOST_CHECK_EQUAL(caught, true);
}
BOOST_AUTO_TEST_CASE(FLAGS_UNIT_TEST_NEG_6)
{
    auto modHandler = std::make_unique<ModHandler>();
    int flags = 0;

    bool caught {false};
    try {
        flags = modHandler->stringToModeSetter("a+7r").value;
    } catch (std::exception &e) {
        if(e.what() == std::string("ModeHandler: Wrong string flag format")) {
            caught = true;
        }
    }

    BOOST_CHECK_EQUAL(caught, true);
}
BOOST_AUTO_TEST_CASE(FLAGS_UNIT_TEST_NEG_7)
{
    auto modHandler = std::make_unique<ModHandler>();
    int flags = 0;

    bool caught {false};
    try {
        flags = modHandler->stringToModeSetter("u+a+g").value;
    } catch (std::exception &e) {
        if(e.what() == std::string("ModeHandler: Wrong string flag format")) {
            caught = true;
        }
    }

    BOOST_CHECK_EQUAL(caught, true);
}
BOOST_AUTO_TEST_CASE(FLAGS_UNIT_TEST_NEG_8)
{
    auto modHandler = std::make_unique<ModHandler>();
    int flags = 0;

    bool caught {false};
    try {
        flags = modHandler->stringToModeSetter("999").value;
    } catch (std::exception &e) {
        if(e.what() == std::string("ModeHandler: Wrong string flag format")) {
            caught = true;
        }
    }

    BOOST_CHECK_EQUAL(caught, true);
}
BOOST_AUTO_TEST_CASE(FLAGS_UNIT_TEST_NEG_9)
{
    auto modHandler = std::make_unique<ModHandler>();
    int flags = 0;

    bool caught {false};
    try {
        flags = modHandler->stringToModeSetter("a+777y").value;
    } catch (std::exception &e) {
        if(e.what() == std::string("ModeHandler: Wrong string flag format")) {
            caught = true;
        }
    }

    BOOST_CHECK_EQUAL(caught, true);
}
BOOST_AUTO_TEST_CASE(FLAGS_UNIT_TEST_NEG_10)
{
    auto modHandler = std::make_unique<ModHandler>();
    int flags = 0;

    bool caught {false};
    try {
        flags = modHandler->stringToModeSetter("ugo+rwrwrrwrwrwrwrwrwrwrwrwrwrwrwrwrwrwrwrwrwrwrwrwrwr").value;
    } catch (std::exception &e) {
        if(e.what() == std::string("ModeHandler: Wrong string flag format")) {
            caught = true;
        }
    }

    BOOST_CHECK_EQUAL(caught, true);
}
BOOST_AUTO_TEST_CASE(FLAGS_UNIT_TEST_NEG_11)
{
    auto modHandler = std::make_unique<ModHandler>();
    int flags = 0;

    bool caught {false};
    try {
        flags = modHandler->stringToModeSetter("aa+rw").value;
    } catch (std::exception &e) {
        if(e.what() == std::string("ModeHandler: Wrong string flag format")) {
            caught = true;
        }
    }

    BOOST_CHECK_EQUAL(caught, true);
}
BOOST_AUTO_TEST_CASE(FLAGS_UNIT_TEST_NEG_12)
{
    auto modHandler = std::make_unique<ModHandler>();
    int flags = 0;

    bool caught {false};
    try {
        flags = modHandler->stringToModeSetter("utuugugugugugutgut+rw").value;
    } catch (std::exception &e) {
        if(e.what() == std::string("ModeHandler: Wrong string flag format")) {
            caught = true;
        }
    }

    BOOST_CHECK_EQUAL(caught, true);
}
BOOST_AUTO_TEST_CASE(FLAGS_UNIT_TEST_NEG_13)
{
    auto modHandler = std::make_unique<ModHandler>();
    int flags = 0;

    bool caught {false};
    try {
        flags = modHandler->stringToModeSetter("999").value;
    } catch (std::exception &e) {
        if(e.what() == std::string("ModeHandler: Wrong string flag format")) {
            caught = true;
        }
    }

    BOOST_CHECK_EQUAL(caught, true);
}
BOOST_AUTO_TEST_CASE(FLAGS_UNIT_TEST_NEG_14)
{
    auto modHandler = std::make_unique<ModHandler>();
    int flags = 0;

    bool caught {false};
    try {
        flags = modHandler->stringToModeSetter("1000").value;
    } catch (std::exception &e) {
        if(e.what() == std::string("ModeHandler: Wrong string flag format")) {
            caught = true;
        }
    }

    BOOST_CHECK_EQUAL(caught, true);
}
BOOST_AUTO_TEST_CASE(FLAGS_UNIT_TEST_NEG_15)
{
    auto modHandler = std::make_unique<ModHandler>();
    int flags = 0;

    bool caught {false};
    try {
        flags = modHandler->stringToModeSetter("769").value;
    } catch (std::exception &e) {
        if(e.what() == std::string("ModeHandler: Wrong string flag format")) {
            caught = true;
        }
    }

    BOOST_CHECK_EQUAL(caught, true);
}
BOOST_AUTO_TEST_CASE(FLAGS_UNIT_TEST_NEG_16)
{
    auto modHandler = std::make_unique<ModHandler>();
    int flags = 0;

    bool caught {false};
    try {
        flags = modHandler->stringToModeSetter("-777").value;
    } catch (std::exception &e) {
        if(e.what() == std::string("ModeHandler: Wrong string flag format")) {
            caught = true;
        }
    }

    BOOST_CHECK_EQUAL(caught, true);
}
BOOST_AUTO_TEST_CASE(FLAGS_UNIT_TEST_NEG_17)
{
    auto modHandler = std::make_unique<ModHandler>();
    int flags = 0;

    bool caught {false};
    try {
        flags = modHandler->stringToModeSetter("-1-1-1").value;
    } catch (std::exception &e) {
        if(e.what() == std::string("ModeHandler: Wrong string flag format")) {
            caught = true;
        }
    }

    BOOST_CHECK_EQUAL(caught, true);
}
BOOST_AUTO_TEST_CASE(FLAGS_UNIT_TEST_NEG_18)
{
    auto modHandler = std::make_unique<ModHandler>();
    int flags = 0;

    bool caught {false};
    try {
        flags = modHandler->stringToModeSetter("090").value;
    } catch (std::exception &e) {
        if(e.what() == std::string("ModeHandler: Wrong string flag format")) {
            caught = true;
        }
    }

    BOOST_CHECK_EQUAL(caught, true);
}
BOOST_AUTO_TEST_CASE(FLAGS_UNIT_TEST_NEG_19)
{
    auto modHandler = std::make_unique<ModHandler>();
    int flags = 0;

    bool caught {false};
    try {
        flags = modHandler->stringToModeSetter("auo+-777").value;
    } catch (std::exception &e) {
        if(e.what() == std::string("ModeHandler: Wrong string flag format")) {
            caught = true;
        }
    }

    BOOST_CHECK_EQUAL(caught, true);
}

BOOST_AUTO_TEST_CASE(FLAGS_UNIT_TEST_NEG_20)
{
    auto modHandler = std::make_unique<ModHandler>();
    int flags = 0;

    bool caught {false};
    try {
        flags = modHandler->stringToModeSetter("-").value;
    } catch (std::exception &e) {
        if(e.what() == std::string("ModeHandler: Wrong string flag format")) {
            caught = true;
        }
    }

    BOOST_CHECK_EQUAL(caught, true);
}
BOOST_AUTO_TEST_CASE(FLAGS_UNIT_TEST_NEG_21)
{
    auto modHandler = std::make_unique<ModHandler>();
    int flags = 0;

    bool caught {false};
    try {
        flags = modHandler->stringToModeSetter("+777").value;
    } catch (std::exception &e) {
        if(e.what() == std::string("ModeHandler: Wrong string flag format")) {
            caught = true;
        }
    }

    BOOST_CHECK_EQUAL(caught, true);
}
BOOST_AUTO_TEST_CASE(FLAGS_UNIT_TEST_NEG_22)
{
    auto modHandler = std::make_unique<ModHandler>();
    int flags = 0;

    bool caught {false};
    try {
        flags = modHandler->stringToModeSetter("+auo-666").value;
    } catch (std::exception &e) {
        if(e.what() == std::string("ModeHandler: Wrong string flag format")) {
            caught = true;
        }
    }

    BOOST_CHECK_EQUAL(caught, true);
}


BOOST_AUTO_TEST_CASE(NOT_ENOUGH_ARGS_EXCEPTION)
{
    std::vector<std::string> input {
            "program",
    };
    TerminalEmulation terminalEmulation(input);
    auto args = terminalEmulation.getArgs();
    auto argc = args.argc;
    auto argv = args.argv;

    bool caught {false};

    try {
        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_unique<Parser>(lexicalAnalyser);
        parser->parse();
    } catch(std::exception &e) {
        if(e.what() == std::string("Not enough args")) {
            caught = true;
        }
    }

    BOOST_CHECK_EQUAL(caught, true);
}

BOOST_AUTO_TEST_CASE(HELP_FLAG_TEST_1)
{
    std::vector<std::string> input {
            "program",
            "-h"
    };
    TerminalEmulation terminalEmulation(input);
    auto args = terminalEmulation.getArgs();
    auto argc = args.argc;
    auto argv = args.argv;

    auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
    auto parser = std::make_unique<Parser>(lexicalAnalyser);
    parser->parse();
    auto statement = parser->getCurrentParsedStatementStr();
    BOOST_CHECK_EQUAL(statement, std::string("help"));
}

BOOST_AUTO_TEST_CASE(HELP_FLAG_TEST_2)
{
    std::vector<std::string> input {
            "program",
            "--help"
    };
    TerminalEmulation terminalEmulation(input);
    auto args = terminalEmulation.getArgs();
    auto argc = args.argc;
    auto argv = args.argv;

    auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
    auto parser = std::make_unique<Parser>(lexicalAnalyser);
    parser->parse();
    auto statement = parser->getCurrentParsedStatementStr();
    BOOST_CHECK_EQUAL(statement, std::string("help"));
}

BOOST_AUTO_TEST_CASE(HELP_FLAG_TEST_3)
{
    std::vector<std::string> input {
            "program",
            "--help"
    };
    TerminalEmulation terminalEmulation(input);
    auto args = terminalEmulation.getArgs();
    auto argc = args.argc;
    auto argv = args.argv;

    auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
    auto parser = std::make_shared<Parser>(lexicalAnalyser);
    auto executor = std::make_shared<Executor>(parser);

    auto ret = executor->execute();
    BOOST_CHECK_EQUAL(ret, std::string("Help printed"));
}

BOOST_AUTO_TEST_CASE(HELP_FLAG_TEST_4)
{
    std::vector<std::string> input {
            "program",
            "--help"
    };
    TerminalEmulation terminalEmulation(input);
    auto args = terminalEmulation.getArgs();
    auto argc = args.argc;
    auto argv = args.argv;

    auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
    auto parser = std::make_shared<Parser>(lexicalAnalyser);
    auto executor = std::make_shared<Executor>(parser);

    auto ret = executor->execute();
    BOOST_CHECK_EQUAL(ret, std::string("Help printed"));
}

BOOST_AUTO_TEST_CASE(INTEGRATION_TEST_0)
{
    std::vector<std::string> input {
            "program",
            "create-key",
            "/tmp/r.ostoja",
            "/tmp/r.ostoja",
            "2048",
            "pass",
            "SHA",
    };
    TerminalEmulation terminalEmulation(input);
    auto args = terminalEmulation.getArgs();
    auto argc = args.argc;
    auto argv = args.argv;

    auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
    auto parser = std::make_unique<Parser>(lexicalAnalyser);
    parser->parse();
    auto statement = parser->getCurrentParsedStatementStr();
    auto serialisedInput = testHelpers::toString(input);
    BOOST_CHECK_EQUAL(statement, serialisedInput);
}

BOOST_AUTO_TEST_CASE(INTEGRATION_TEST_1)
{
    std::vector<std::string> input {
            "program",
            "get-private-key",
            "/tmp/private",
            "/tmp/abc",
            "pass",
    };
    TerminalEmulation terminalEmulation(input);
    auto args = terminalEmulation.getArgs();
    auto argc = args.argc;
    auto argv = args.argv;

    auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
    auto parser = std::make_unique<Parser>(lexicalAnalyser);
    parser->parse();
    auto statement = parser->getCurrentParsedStatementStr();
    auto serialisedInput = testHelpers::toString(input);
    BOOST_CHECK_EQUAL(statement, serialisedInput);
}
BOOST_AUTO_TEST_CASE(INTEGRATION_TEST_2)
{
    std::vector<std::string> input {
            "program",
            "delete-key",
            "/tmp/private",
            "/tmp/pubKeyPath"
    };
    TerminalEmulation terminalEmulation(input);
    auto args = terminalEmulation.getArgs();
    auto argc = args.argc;
    auto argv = args.argv;

    auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
    auto parser = std::make_unique<Parser>(lexicalAnalyser);
    parser->parse();
    auto statement = parser->getCurrentParsedStatementStr();
    auto serialisedInput = testHelpers::toString(input);
    BOOST_CHECK_EQUAL(statement, serialisedInput);
}
BOOST_AUTO_TEST_CASE(INTEGRATION_TEST_3)
{
    std::vector<std::string> input {
            "program_sign",
            "sign",
            "/tmp/private",
            "/tmp/signature",
            "outpath",
            "pass",
    };
    TerminalEmulation terminalEmulation(input);
    auto args = terminalEmulation.getArgs();
    auto argc = args.argc;
    auto argv = args.argv;

    auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
    auto parser = std::make_unique<Parser>(lexicalAnalyser);
    parser->parse();
    auto statement = parser->getCurrentParsedStatementStr();
    auto serialisedInput = testHelpers::toString(input);
    BOOST_CHECK_EQUAL(statement, serialisedInput);
}
BOOST_AUTO_TEST_CASE(INTEGRATION_TEST_4)
{
    std::vector<std::string> input {
            "program",
            "check-signature",
            "/tmp/pubKeyPath",
            "/tmp/signature",
            "signature_path",
    };
    TerminalEmulation terminalEmulation(input);
    auto args = terminalEmulation.getArgs();
    auto argc = args.argc;
    auto argv = args.argv;

    auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
    auto parser = std::make_unique<Parser>(lexicalAnalyser);
    parser->parse();
    auto statement = parser->getCurrentParsedStatementStr();
    auto serialisedInput = testHelpers::toString(input);
    BOOST_CHECK_EQUAL(statement, serialisedInput);
}
BOOST_AUTO_TEST_CASE(INTEGRATION_TEST_5)
{
    std::vector<std::string> input {
            "program",
            "encrypt-file",
            "/tmp/key",
            "/tmp/iv",
            "/tmp/file_to_encrypt",
            "/tmp/path_to_encrypted_file",
    };
    TerminalEmulation terminalEmulation(input);
    auto args = terminalEmulation.getArgs();
    auto argc = args.argc;
    auto argv = args.argv;

    auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
    auto parser = std::make_unique<Parser>(lexicalAnalyser);
    parser->parse();
    auto statement = parser->getCurrentParsedStatementStr();
    auto serialisedInput = testHelpers::toString(input);
    BOOST_CHECK_EQUAL(statement, serialisedInput);
}
BOOST_AUTO_TEST_CASE(INTEGRATION_TEST_Decrypt)
{
    std::vector<std::string> input {
            "program",
            "decrypt-file",
            "/tmp/key",
            "/tmp/iv",
            "/tmp/encrypted_file",
            "/tmp/path_to_decrypted_file",
            "output",
    };
    TerminalEmulation terminalEmulation(input);
    auto args = terminalEmulation.getArgs();
    auto argc = args.argc;
    auto argv = args.argv;

    auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
    auto parser = std::make_unique<Parser>(lexicalAnalyser);
    parser->parse();
    auto statement = parser->getCurrentParsedStatementStr();
    auto serialisedInput = testHelpers::toString(input);
    BOOST_CHECK_EQUAL(statement + " output", serialisedInput);
}

BOOST_AUTO_TEST_CASE(INTEGRATION_TEST_GET_MODE)
{
    std::vector<std::string> input {
            "program",
            "gmod",
            "/tmp/key"
    };
    TerminalEmulation terminalEmulation(input);
    auto args = terminalEmulation.getArgs();
    auto argc = args.argc;
    auto argv = args.argv;

    auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
    auto parser = std::make_unique<Parser>(lexicalAnalyser);
    parser->parse();
    auto statement = parser->getCurrentParsedStatementStr();
    auto serialisedInput = testHelpers::toString(input);
    BOOST_CHECK_EQUAL(statement, serialisedInput);
}

BOOST_AUTO_TEST_CASE(INTEGRATION_TEST_CHMOD)
{
    std::vector<std::string> input {
            "program",
            "chmod",
            "/tmp/key",
            "g+3123"
    };
    TerminalEmulation terminalEmulation(input);
    auto args = terminalEmulation.getArgs();
    auto argc = args.argc;
    auto argv = args.argv;

    auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
    auto parser = std::make_unique<Parser>(lexicalAnalyser);
    parser->parse();
    auto statement = parser->getCurrentParsedStatementStr();
    auto serialisedInput = testHelpers::toString(input);
    BOOST_CHECK_EQUAL(statement, serialisedInput);
}

BOOST_AUTO_TEST_CASE(INTEGRATION_TEST_GET_MODE_TOO_MANY_PARAMS)
{
    std::vector<std::string> input {
            "program",
            "gmod",
            "/tmp/key",
            "additional_param"
    };
    TerminalEmulation terminalEmulation(input);
    auto args = terminalEmulation.getArgs();
    auto argc = args.argc;
    auto argv = args.argv;

    auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
    auto parser = std::make_unique<Parser>(lexicalAnalyser);
    parser->parse();
    auto statement = parser->getCurrentParsedStatementStr();
    auto serialisedInput = testHelpers::toString(input);
    BOOST_CHECK_EQUAL(statement + " additional_param", serialisedInput);
}

BOOST_AUTO_TEST_CASE(INTEGRATION_TEST_CHMOD_TOO_MANY_PARAMS)
{
    std::vector<std::string> input {
            "program",
            "chmod",
            "/tmp/key",
            "g+3123",
            "additional_param"
    };
    TerminalEmulation terminalEmulation(input);
    auto args = terminalEmulation.getArgs();
    auto argc = args.argc;
    auto argv = args.argv;

    auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
    auto parser = std::make_unique<Parser>(lexicalAnalyser);
    parser->parse();
    auto statement = parser->getCurrentParsedStatementStr();
    auto serialisedInput = testHelpers::toString(input);
    BOOST_CHECK_EQUAL(statement + " additional_param", serialisedInput);
}

BOOST_AUTO_TEST_CASE(CreateKeyTooManyParams)
{
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");

    std::vector<std::string> input {
            "program",
            "create-key",
            "/tmp/r.ostoja_private",
            "/tmp/r.ostoja_public",
            "2048",
            "SHA",
            "pass",
            "additional_param"
    };
    TerminalEmulation terminalEmulation(input);
    auto args = terminalEmulation.getArgs();
    auto argc = args.argc;
    auto argv = args.argv;

    auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
    auto parser = std::make_unique<Parser>(lexicalAnalyser);
    parser->parse();
    auto statement = parser->getCurrentParsedStatementStr();
    auto serialisedInput = testHelpers::toString(input);
    BOOST_CHECK_EQUAL(statement + " additional_param", serialisedInput);

    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");

}
BOOST_AUTO_TEST_CASE(GetPrivateKeyTooManyParams)
{
    std::vector<std::string> input {
            "program",
            "get-private-key",
            "/tmp/private",
            "/tmp/abc",
            "pass",
            "additional_param"
    };
    TerminalEmulation terminalEmulation(input);
    auto args = terminalEmulation.getArgs();
    auto argc = args.argc;
    auto argv = args.argv;

    auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
    auto parser = std::make_unique<Parser>(lexicalAnalyser);
    parser->parse();
    auto statement = parser->getCurrentParsedStatementStr();
    auto serialisedInput = testHelpers::toString(input);
    BOOST_CHECK_EQUAL(statement + " additional_param", serialisedInput);
}
BOOST_AUTO_TEST_CASE(DeleteKeyTooManyParams)
{
    std::vector<std::string> input {
            "program",
            "delete-key",
            "/tmp/private",
            "/tmp/pubKeyPath",
            "additional_param"
    };
    TerminalEmulation terminalEmulation(input);
    auto args = terminalEmulation.getArgs();
    auto argc = args.argc;
    auto argv = args.argv;

    auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
    auto parser = std::make_unique<Parser>(lexicalAnalyser);
    parser->parse();
    auto statement = parser->getCurrentParsedStatementStr();
    auto serialisedInput = testHelpers::toString(input);
    BOOST_CHECK_EQUAL(statement + " additional_param", serialisedInput);
}
BOOST_AUTO_TEST_CASE(DeleteKeyTooManyParamsOverwrite)
{
    std::vector<std::string> input {
            "program",
            "delete-key",
            "/tmp/private",
            "/tmp/pubKeyPath",
            "overwrite"
    };
    TerminalEmulation terminalEmulation(input);
    auto args = terminalEmulation.getArgs();
    auto argc = args.argc;
    auto argv = args.argv;

    auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
    auto parser = std::make_unique<Parser>(lexicalAnalyser);
    parser->parse();
    auto statement = parser->getCurrentParsedStatementStr();
    auto serialisedInput = testHelpers::toString(input);
    BOOST_CHECK_EQUAL(statement + " overwrite", serialisedInput);
}
BOOST_AUTO_TEST_CASE(SignTooManyParams)
{
    std::vector<std::string> input {
            "program_sign",
            "sign",
            "/tmp/abc_private",
            "/tmp/abc_public",
            "pass",
            "output",
    };
    TerminalEmulation terminalEmulation(input);
    auto args = terminalEmulation.getArgs();
    auto argc = args.argc;
    auto argv = args.argv;

    auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
    auto parser = std::make_unique<Parser>(lexicalAnalyser);
    parser->parse();
    auto statement = parser->getCurrentParsedStatementStr();
    auto serialisedInput = testHelpers::toString(input);
    BOOST_CHECK_EQUAL(statement, serialisedInput);
}
BOOST_AUTO_TEST_CASE(CheckSignatureTooManyParams)
{
    std::vector<std::string> input {
            "program",
            "check-signature",
            "/tmp/private",
            "/tmp/pubKeyPath",
            "signature_path",
            "additional_param"
    };
    TerminalEmulation terminalEmulation(input);
    auto args = terminalEmulation.getArgs();
    auto argc = args.argc;
    auto argv = args.argv;

    auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
    auto parser = std::make_unique<Parser>(lexicalAnalyser);
    parser->parse();
    auto statement = parser->getCurrentParsedStatementStr();
    auto serialisedInput = testHelpers::toString(input);
    BOOST_CHECK_EQUAL(statement + " additional_param", serialisedInput);
}
BOOST_AUTO_TEST_CASE(EncryptFileTooManyParams)
{
    std::vector<std::string> input {
            "program",
            "encrypt-file",
            "/tmp/private-key",
            "/tmp/file-to-encrypt",
            "/tmp/path_to_encrypted_file",
            "additional_param",
            "additional_param",
    };
    TerminalEmulation terminalEmulation(input);
    auto args = terminalEmulation.getArgs();
    auto argc = args.argc;
    auto argv = args.argv;

    auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
    auto parser = std::make_unique<Parser>(lexicalAnalyser);
    parser->parse();
    auto statement = parser->getCurrentParsedStatementStr();
    auto serialisedInput = testHelpers::toString(input);
    BOOST_CHECK_EQUAL(statement + " additional_param", serialisedInput);
}

BOOST_AUTO_TEST_CASE(DecryptFileTooManyParams)
{
    std::vector<std::string> input {
            "program",
            "decrypt-file",
            "/tmp/file-to-decrypt",
            "output",
            "413243",
            "additional_param",
            "additional_param",
    };
    TerminalEmulation terminalEmulation(input);
    auto args = terminalEmulation.getArgs();
    auto argc = args.argc;
    auto argv = args.argv;

    auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
    auto parser = std::make_unique<Parser>(lexicalAnalyser);
    parser->parse();
    auto statement = parser->getCurrentParsedStatementStr();
    auto serialisedInput = testHelpers::toString(input);
    BOOST_CHECK_EQUAL(statement + " additional_param", serialisedInput);
}

BOOST_AUTO_TEST_CASE(POSITIVE_TEST_CREATE_KEY)
{
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");

    std::vector<std::string> input {
            "program",
            "create-key",
            "/tmp/private.pem",
            "/tmp/public.pem",
            "2048",
            "RSA",
            "pass",
            "overwrite",
    };
    TerminalEmulation terminalEmulation(input);
    auto emulatedTerminalArgs = terminalEmulation.getArgs();
    auto argc = emulatedTerminalArgs.argc;
    auto argv = emulatedTerminalArgs.argv;

    auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
    auto parser = std::make_shared<Parser>(lexicalAnalyser);
    auto executor = std::make_shared<Executor>(parser);
    auto msg = executor->execute();

    BOOST_CHECK_EQUAL(msg, "Keys created");
    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");

}

BOOST_AUTO_TEST_CASE(INTEGRATION_TEST_CHMOD_WRONG_INPUT_1)
{
    std::vector<std::string> input {
            "program",
            "chmod",
            "/tmp/key",
            "g+aw"
    };
    TerminalEmulation terminalEmulation(input);
    auto args = terminalEmulation.getArgs();
    auto argc = args.argc;
    auto argv = args.argv;

    auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
    auto parser = std::make_shared<Parser>(lexicalAnalyser);
    auto executor = std::make_unique<Executor>(parser);

    bool caught {false};

    try {
        auto msg = executor->execute();
    } catch (std::exception &e) {
        if(e.what() == std::string("ModeHandler: Wrong string flag format")) {
            caught = true;
        }
    }

    BOOST_CHECK_EQUAL(caught, true);
}

BOOST_AUTO_TEST_CASE(INTEGRATION_TEST_CHMOD_WRONG_INPUT_2)
{
    std::vector<std::string> input {
            "program",
            "chmod",
            "/tmp/key",
            "aw"
    };
    TerminalEmulation terminalEmulation(input);
    auto args = terminalEmulation.getArgs();
    auto argc = args.argc;
    auto argv = args.argv;

    auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
    auto parser = std::make_shared<Parser>(lexicalAnalyser);
    auto executor = std::make_unique<Executor>(parser);

    bool caught {false};

    try {
        auto msg = executor->execute();
    } catch (std::exception &e) {
        if(e.what() == std::string("ModeHandler: Wrong string flag format")) {
            caught = true;
        }
    }

    BOOST_CHECK_EQUAL(caught, true);
}

BOOST_AUTO_TEST_CASE(INTEGRATION_TEST_CHMOD_WRONG_INPUT_3)
{
    std::vector<std::string> input {
            "program",
            "chmod",
            "/tmp/key",
            "grw"
    };
    TerminalEmulation terminalEmulation(input);
    auto args = terminalEmulation.getArgs();
    auto argc = args.argc;
    auto argv = args.argv;

    auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
    auto parser = std::make_shared<Parser>(lexicalAnalyser);
    auto executor = std::make_unique<Executor>(parser);

    bool caught {false};

    try {
        auto msg = executor->execute();
    } catch (std::exception &e) {
        if(e.what() == std::string("ModeHandler: Wrong string flag format")) {
            caught = true;
        }
    }

    BOOST_CHECK_EQUAL(caught, true);
}

BOOST_AUTO_TEST_CASE(INTEGRATION_TEST_CHMOD_WRONG_INPUT_4)
{
    std::vector<std::string> input {
            "program",
            "chmod",
            "/tmp/key",
            "o+rew"
    };
    TerminalEmulation terminalEmulation(input);
    auto args = terminalEmulation.getArgs();
    auto argc = args.argc;
    auto argv = args.argv;

    auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
    auto parser = std::make_shared<Parser>(lexicalAnalyser);
    auto executor = std::make_unique<Executor>(parser);

    bool caught {false};

    try {
        auto msg = executor->execute();
    } catch (std::exception &e) {
        if(e.what() == std::string("ModeHandler: Wrong string flag format")) {
            caught = true;
        }
    }

    BOOST_CHECK_EQUAL(caught, true);
}

BOOST_AUTO_TEST_CASE(INTEGRATION_TEST_CHMOD_NOT_EXISTING_KEY)
{
    system("rm /tmp/key");
    std::vector<std::string> input {
            "program",
            "chmod",
            "/tmp/key",
            "g+rw"
    };
    TerminalEmulation terminalEmulation(input);
    auto args = terminalEmulation.getArgs();
    auto argc = args.argc;
    auto argv = args.argv;

    auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
    auto parser = std::make_shared<Parser>(lexicalAnalyser);
    auto executor = std::make_unique<Executor>(parser);

    bool caught {false};

    try {
        auto msg = executor->execute();
    } catch (std::exception &e) {
        if(e.what() == std::string("KeyIOInterface: Failed to read file")) {
            caught = true;
        }
    }

    BOOST_CHECK_EQUAL(caught, true);
}

BOOST_AUTO_TEST_CASE(INTEGRATION_TEST_GMOD_NOT_EXISTING_KEY)
{
    system("rm /tmp/key");
    std::vector<std::string> input {
            "program",
            "gmod",
            "/tmp/key",
    };
    TerminalEmulation terminalEmulation(input);
    auto args = terminalEmulation.getArgs();
    auto argc = args.argc;
    auto argv = args.argv;

    auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
    auto parser = std::make_shared<Parser>(lexicalAnalyser);
    auto executor = std::make_unique<Executor>(parser);

    bool caught {false};

    try {
        auto msg = executor->execute();
    } catch (std::exception &e) {
        if(e.what() == std::string("KeyIOInterface: Failed to read file")) {
            caught = true;
        }
    }

    BOOST_CHECK_EQUAL(caught, true);
}

BOOST_AUTO_TEST_CASE(INTEGRATION_TEST_CHMOD_WRONG_KEY)
{
    system("echo 312312321 > /tmp/key");
    std::vector<std::string> input {
            "program",
            "chmod",
            "/tmp/key",
            "g+rw"
    };
    TerminalEmulation terminalEmulation(input);
    auto args = terminalEmulation.getArgs();
    auto argc = args.argc;
    auto argv = args.argv;

    auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
    auto parser = std::make_shared<Parser>(lexicalAnalyser);
    auto executor = std::make_unique<Executor>(parser);

    bool caught {false};

    try {
        auto msg = executor->execute();
    } catch (std::exception &e) {
        if(e.what() == std::string("KeyIOInterface: Cannot get private key modes")) {
            caught = true;
        }
    }

    BOOST_CHECK_EQUAL(caught, true);
}

BOOST_AUTO_TEST_CASE(INTEGRATION_TEST_GMOD_WRONG_KEY)
{
    system("echo 312312321 > /tmp/key");
    std::vector<std::string> input {
            "program",
            "gmod",
            "/tmp/key",
    };
    TerminalEmulation terminalEmulation(input);
    auto args = terminalEmulation.getArgs();
    auto argc = args.argc;
    auto argv = args.argv;

    auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
    auto parser = std::make_shared<Parser>(lexicalAnalyser);
    auto executor = std::make_unique<Executor>(parser);

    bool caught {false};

    try {
        auto msg = executor->execute();
    } catch (std::exception &e) {
        if(e.what() == std::string("KeyIOInterface: Cannot get private key modes")) {
            caught = true;
        }
    }

   BOOST_CHECK_EQUAL(caught, true);
}

BOOST_AUTO_TEST_CASE(POSITIVE_TEST_GMOD)
{
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");

    {
        std::vector<std::string> input {
                "program",
                "create-key",
                "/tmp/private.pem",
                "/tmp/public.pem",
                "2048",
                "RSA",
                "pass",
                "overwrite",
        };
        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);
        executor->execute();
    }
    
    {
        std::vector<std::string> input{
                "program",
                "gmod",
                "/tmp/private.pem",
        };
        TerminalEmulation terminalEmulation(input);
        auto args = terminalEmulation.getArgs();
        auto argc = args.argc;
        auto argv = args.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_unique<Executor>(parser);
        auto msg = executor->execute();
        BOOST_CHECK_EQUAL(msg, "600");

    }

    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

BOOST_AUTO_TEST_CASE(POSITIVE_TEST_CREATE_CH_G_MOD)
{
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");

    {
        std::vector<std::string> input {
                "program",
                "create-key",
                "/tmp/private.pem",
                "/tmp/public.pem",
                "2048",
                "RSA",
                "pass",
                "overwrite",
        };
        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);
        executor->execute();
    }
    {
        std::vector<std::string> input{
                "program",
                "chmod",
                "/tmp/private.pem",
                "060"
        };
        TerminalEmulation terminalEmulation(input);
        auto args = terminalEmulation.getArgs();
        auto argc = args.argc;
        auto argv = args.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_unique<Executor>(parser);
        auto msg = executor->execute();

    }
    {
        std::vector<std::string> input{
                "program",
                "gmod",
                "/tmp/private.pem",
        };
        TerminalEmulation terminalEmulation(input);
        auto args = terminalEmulation.getArgs();
        auto argc = args.argc;
        auto argv = args.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_unique<Executor>(parser);
        auto msg = executor->execute();
        BOOST_CHECK_EQUAL(msg, "060");

    }

    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}
BOOST_AUTO_TEST_CASE(POSITIVE_TEST_CREATE_CH_G_MOD_2)
{
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");

    {
        std::vector<std::string> input {
                "program",
                "create-key",
                "/tmp/private.pem",
                "/tmp/public.pem",
                "2048",
                "RSA",
                "pass",
                "overwrite",
        };
        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);
        executor->execute();
    }
    {
        std::vector<std::string> input{
                "program",
                "chmod",
                "/tmp/private.pem",
                "u-w"
        };
        TerminalEmulation terminalEmulation(input);
        auto args = terminalEmulation.getArgs();
        auto argc = args.argc;
        auto argv = args.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_unique<Executor>(parser);
        auto msg = executor->execute();

    }
    {
        std::vector<std::string> input{
                "program",
                "gmod",
                "/tmp/private.pem",
        };
        TerminalEmulation terminalEmulation(input);
        auto args = terminalEmulation.getArgs();
        auto argc = args.argc;
        auto argv = args.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_unique<Executor>(parser);
        auto msg = executor->execute();
        BOOST_CHECK_EQUAL(msg, "400");

    }

    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

BOOST_AUTO_TEST_CASE(INTEGRATION_TEST_CHMOD_GMOD_ALL_CASES)
{
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");

    {
        std::vector<std::string> input {
                "program",
                "create-key",
                "/tmp/private.pem",
                "/tmp/public.pem",
                "2048",
                "RSA",
                "pass",
                "overwrite",
        };
        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);
        executor->execute();
    }

    std::vector<std::string> flags = {
        "600",
        "602",
        "604",
        "606",
        "620",
        "622",
        "624",
        "626",
        "640",
        "642",
        "644",
        "646",
        "660",
        "662",
        "664",
        "666",
    };

    bool any_error {false};

    for( auto flag : flags ) {

        {
            std::vector<std::string> input{
                    "program",
                    "chmod",
                    "/tmp/private.pem",
                    flag
            };
            TerminalEmulation terminalEmulation(input);
            auto args = terminalEmulation.getArgs();
            auto argc = args.argc;
            auto argv = args.argv;

            auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
            auto parser = std::make_shared<Parser>(lexicalAnalyser);
            auto executor = std::make_unique<Executor>(parser);


            try {
                auto msg = executor->execute();
            } catch (std::exception &e) {
                if (e.what() == std::string("Change mod: Unknown flags.")) {
                    any_error = true;
                }
            }
        }
        {
            std::vector<std::string> input{
                    "program",
                    "gmod",
                    "/tmp/private.pem",
            };
            TerminalEmulation terminalEmulation(input);
            auto args = terminalEmulation.getArgs();
            auto argc = args.argc;
            auto argv = args.argv;

            auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
            auto parser = std::make_shared<Parser>(lexicalAnalyser);
            auto executor = std::make_unique<Executor>(parser);

            std::string msg;
            try {
                msg = executor->execute();
            } catch (std::exception &e) {
                if (e.what() == std::string("KeyIOInterface: Failed to read file")) {
                    any_error = true;
                }
            }
            if (msg != flag) {
                any_error = true;
            }
        }
    }
   
    BOOST_CHECK_EQUAL(any_error, false);

    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}


BOOST_AUTO_TEST_CASE(CREATE_KEY_DANGEROUS_LEN) {
    std::vector<std::string> input{
            "program",
            "create-key",
            "/tmp/private.pem",
            "/tmp/public.pem",
            "1024",
            "RSA",
            "pass",
            "overwrite",
    };
    TerminalEmulation terminalEmulation(input);
    auto emulatedTerminalArgs = terminalEmulation.getArgs();
    auto argc = emulatedTerminalArgs.argc;
    auto argv = emulatedTerminalArgs.argv;

    auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
    auto parser = std::make_shared<Parser>(lexicalAnalyser);
    auto executor = std::make_shared<Executor>(parser);

    bool caught {false};

    try {
        auto msg = executor->execute();
    } catch (std::exception &e) {
        if(e.what() == std::string("Create key: Use value of 2048 or higher")) {
            caught = true;
        }
    }

    BOOST_CHECK_EQUAL(caught, true);
}

BOOST_AUTO_TEST_CASE(POSITIVE_TEST_SIGN)
{
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");

    {
        std::vector<std::string> input {
                "program",
                "create-key",
                "/tmp/private.pem",
                "/tmp/public.pem",
                "2048",
                "RSA",
                "pass",
                "overwrite",
        };
        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);
        executor->execute();
    }
    {
        system("echo a > /tmp/file.txt");
        std::vector<std::string> input {
                "program",
                "sign",
                "/tmp/private.pem",
                "/tmp/file.txt",
                "/tmp/signature.txt",
                "pass",
                "overwrite",
        };

        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);
        auto msg = executor->execute();

        BOOST_CHECK_EQUAL(msg, "File signed");
    }
    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");

}

BOOST_AUTO_TEST_CASE(POSITIVE_TEST_SIGN_OFFSET_PART)
{
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");
    system("echo czary_mary >> ~/.keyPartitionV2/meta");

    {
        std::vector<std::string> input {
                "program",
                "create-key",
                "/tmp/private.pem",
                "/tmp/public.pem",
                "2048",
                "RSA",
                "pass",
                "overwrite",
        };
        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);
        executor->execute();
    }

//    /*
    {
        system("echo a > /tmp/file.txt");
        std::vector<std::string> input {
                "program",
                "sign",
                "/tmp/private.pem",
                "/tmp/file.txt",
                "/tmp/signature.txt",
                "pass",
                "overwrite",
        };

        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);
        auto msg = executor->execute();

        BOOST_CHECK_EQUAL(msg, "File signed");
    }
//     */
    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");

}
BOOST_AUTO_TEST_CASE(POSITIVE_TEST_DELETE_KEY)
{
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");

    {
        std::vector<std::string> input {
                "program",
                "create-key",
                "/tmp/private.pem",
                "/tmp/public.pem",
                "2048",
                "RSA",
                "pass",
                "overwrite",
        };
        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);
        executor->execute();
    }
    {
        std::vector<std::string> input {
                "program",
                "delete-key",
                "/tmp/private.pem",
                "/tmp/public.pem",
        };

        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);
        auto msg = executor->execute();

        BOOST_CHECK_EQUAL(msg, "Keys deleted");
    }
    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");

}

BOOST_AUTO_TEST_CASE(POSITIVE_TEST_GET_PRIVATE_KEY)
{
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");

    {
        std::vector<std::string> input {
                "program",
                "create-key",
                "/tmp/private.pem",
                "/tmp/public.pem",
                "2048",
                "RSA",
                "pass",
                "overwrite",
        };
        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);
        executor->execute();
    }
    {
        std::vector<std::string> input {
                "program",
                "get-private-key",
                "/tmp/private.pem",
                "/tmp/private_key_value",
                "pass",
                "overwrite",
        };

        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);
        auto msg = executor->execute();

        BOOST_CHECK_EQUAL(msg, "Private key saved");
    }
    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");

}

BOOST_AUTO_TEST_CASE(CHECK_SIGNATURE_INTEGRATION_TEST_6)
{
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");

    {
        std::vector<std::string> input {
                "program",
                "create-key",
                "/tmp/private.pem",
                "/tmp/public.pem",
                "2048",
                "RSA",
                "pass",
                "overwrite",
        };
        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);
        executor->execute();
    }
    {
        std::vector<std::string> input {
                "program",
                "get-private-key",
                "/tmp/private.pem",
                "/tmp/private_key_value",
                "pass",
                "overwrite",
        };

        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);
        auto msg = executor->execute();

        BOOST_CHECK_EQUAL(msg, "Private key saved");
    }
    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");

}

BOOST_AUTO_TEST_CASE(POSITIVE_TEST_ENCRYPT_FILE)
{
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");

    system("echo 3123213213 > /tmp/file_to_encrypt.txt");
    system("echo 0123456789abcdeF0123456789abcdeF > /tmp/key");
    system("echo 1234567887654321 > /tmp/iv");
    {
        std::vector<std::string> input {
                "program",
                "encrypt-file",
                "/tmp/key",
                "/tmp/iv",
                "/tmp/file_to_encrypt.txt",
                "/tmp/encrypted_file.txt",
                "overwrite"
        };

        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);
        auto msg = executor->execute();

        BOOST_CHECK_EQUAL(msg, "File encrypted");
    }
    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");

}


BOOST_AUTO_TEST_CASE(CHECK_SIGNATURE_INTEGRATION_TEST_7)
{
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");
    {
        std::vector<std::string> input {
                "program",
                "create-key",
                "/tmp/private.pem",
                "/tmp/public.pem",
                "2048",
                "RSA",
                "pass",
                "overwrite",
        };
        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);
        executor->execute();
    }
    {
        std::vector<std::string> input {
                "program",
                "sign",
                "/tmp/private.pem",
                "/tmp/file.txt",
                "/tmp/signature.txt",
                "pass",
                "overwrite",
        };

        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);
        executor->execute();
    }
    {
        std::vector<std::string> input {
                "program",
                "check-signature",
                "/tmp/public.pem",
                "/tmp/file.txt",
                "/tmp/signature.txt",
        };

        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);
        auto msg = executor->execute();

        BOOST_CHECK_EQUAL(msg, "Signature correct");
    }
    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

BOOST_AUTO_TEST_CASE(ENCRYPT_NOT_EXISTING_KEY)
{
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");
    {
        system("rm /tmp/not_existing_key");
        std::vector<std::string> input {
                "program",
                "encrypt-file",
                "/tmp/not_existing_key",
                "/tmp/iv",
                "/tmp/file.txt",
                "/tmp/encrypted_file.txt",
                "overwrite"
        };

        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);

        bool caught {false};
        try {
            executor->execute();
        }catch(std::exception &e) {
            if(e.what() == std::string("KeyIOInterface: Failed to read file")) {
                caught = true;
            }
        }

        BOOST_CHECK_EQUAL(caught, true);
    }

    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

BOOST_AUTO_TEST_CASE(ENCRYPT_NOT_EXISTING_IV)
{
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");
    {
        system("rm /tmp/g_iv");
        system("echo 312312321 >> /tmp/key");
        std::vector<std::string> input {
                "program",
                "encrypt-file",
                "/tmp/key",
                "/tmp/not_existing_iv",
                "/tmp/file.txt",
                "/tmp/encrypted_file.txt",
                "overwrite"
        };

        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);

        bool caught {false};
        try {
            executor->execute();
        }catch(std::exception &e) {
            if(e.what() == std::string("KeyIOInterface: Failed to read file")) {
                caught = true;
            }
        }

        BOOST_CHECK_EQUAL(caught, true);
    }

    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}
BOOST_AUTO_TEST_CASE(ENCRYPT_EMPTY_IV)
{
        system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");
        {
            system("echo "" > /tmp/not_existing_iv");
            system("echo 312312321 >> /tmp/key");
            std::vector <std::string> input{
                    "program",
                    "encrypt-file",
                    "/tmp/key",
                    "/tmp/not_existing_iv",
                    "/tmp/file.txt",
                    "/tmp/encrypted_file.txt",
                    "overwrite"
            };

            TerminalEmulation terminalEmulation(input);
            auto emulatedTerminalArgs = terminalEmulation.getArgs();
            auto argc = emulatedTerminalArgs.argc;
            auto argv = emulatedTerminalArgs.argv;

            auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
            auto parser = std::make_shared<Parser>(lexicalAnalyser);
            auto executor = std::make_shared<Executor>(parser);
            auto msg = executor->execute();
            BOOST_CHECK_EQUAL(msg, std::string("File encrypted"));
        }

        system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
        }

BOOST_AUTO_TEST_CASE(ENCRYPT_EMPTY_KEY)
{
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");
    {
        system("echo "" > /tmp/key");
        system("echo 312312321 >> /tmp/not_existing_iv");
        std::vector<std::string> input {
                "program",
                "encrypt-file",
                "/tmp/key",
                "/tmp/not_existing_iv",
                "/tmp/file.txt",
                "/tmp/encrypted_file.txt",
                "overwrite"
        };

        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);
        auto msg = executor->execute();
        BOOST_CHECK_EQUAL(msg, std::string("File encrypted"));
    }

    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

BOOST_AUTO_TEST_CASE(ENCRYPT_EMPTY_FILE_IV)
    {
        system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");
    {
        system("echo "" > /tmp/not_existing_iv");
        system("echo 312312321 >> /tmp/key");
        system("echo "" > /tmp/file.txt");
        std::vector<std::string> input {
                "program",
                "encrypt-file",
                "/tmp/key",
                "/tmp/not_existing_iv",
                "/tmp/file.txt",
                "/tmp/encrypted_file.txt",
                "overwrite"
        };

        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);

        auto msg = executor->execute();
        BOOST_CHECK_EQUAL(msg, std::string("File encrypted"));
    }

    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

BOOST_AUTO_TEST_CASE(DECRYPT_NOT_EXISTING_IV)
{
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");
    {
        system("rm /tmp/not_existing_iv");
        system("echo 312312321 >> /tmp/key");
        std::vector<std::string> input {
                "program",
                "decrypt-file",
                "/tmp/key",
                "/tmp/not_existing_iv",
                "/tmp/file.txt",
                "/tmp/decrypted_file.txt",
                "overwrite"
        };

        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);

        bool caught {false};
        try {
            executor->execute();
        }catch(std::exception &e) {
            if(e.what() == std::string("KeyIOInterface: Failed to read file")) {
                caught = true;
            }
        }

        BOOST_CHECK_EQUAL(caught, true);
    }

    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

BOOST_AUTO_TEST_CASE(ENCRYPT_NOT_EXISTING_FILE)
{
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");
    {
        system("rm /tmp/not_existing_file");
        std::vector<std::string> input {
                "program",
                "encrypt-file",
                "/tmp/key",
                "/tmp/iv",
                "/tmp/not_existing_file",
                "/tmp/encrypted_file.txt",
                "overwrite"
        };

        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);

        bool caught {false};
        try {
            executor->execute();
        }catch(std::exception &e) {
            if(e.what() == std::string("KeyIOInterface: Cannot open file")) {
                caught = true;
            }
        }

        BOOST_CHECK_EQUAL(caught, true);
    }

    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

BOOST_AUTO_TEST_CASE(DECRYPT_NOT_EXISTING_KEY)
{
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");
    {
        system("rm /tmp/not_existing_key");
        std::vector<std::string> input {
                "program",
                "decrypt-file",
                "/tmp/not_existing_key",
                "/tmp/iv",
                "/tmp/file.txt",
                "/tmp/encrypted_file.txt",
                "overwrite"
        };

        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);

        bool caught {false};
        try {
            executor->execute();
        }catch(std::exception &e) {
            if(e.what() == std::string("KeyIOInterface: Failed to read file")) {
                caught = true;
            }
        }

        BOOST_CHECK_EQUAL(caught, true);
    }

    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}
BOOST_AUTO_TEST_CASE(ENCRYPT_OVERWRITE_TRIAL_WITHOUT_FLAG)
{
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");
    {
        std::vector<std::string> input{
                "program",
                "create-key",
                "/tmp/private.pem",
                "/tmp/public.pem",
                "2048",
                "RSA",
                "pass",
                "overwrite",
        };
        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);
        executor->execute();
    }
    {
        system("echo a > /tmp/to_encrypt");
        system("echo a > /tmp/encrypted_file.txt");

        std::vector<std::string> input {
                "program",
                "encrypt-file",
                "/tmp/private.pem",
                "/tmp/to_encrypt",
                "/tmp/encrypted_file.txt",
        };

        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);

        try {
            executor->execute();
        } catch (std::exception &e) {
            /* dummy */
        }

        std::string afterPotentialCreate = testHelpers::readFileIntoString("/tmp/encrypted_file.txt");
        bool areSame = (afterPotentialCreate == "a");
        BOOST_CHECK_EQUAL(areSame, true);
    }

    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

BOOST_AUTO_TEST_CASE(DECRYPT_OVERWRITE_TRIAL_WITHOUT_FLAG)
{
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");
    {
        std::vector<std::string> input{
                "program",
                "create-key",
                "/tmp/private.pem",
                "/tmp/public.pem",
                "2048",
                "RSA",
                "pass",
                "overwrite",
        };
        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);
        executor->execute();
    }
    {
        system("echo a > /tmp/to_decrypt");
        system("echo a > /tmp/decrypted_file.txt");

        std::vector<std::string> input {
                "program",
                "decrypt-file",
                "/tmp/public.pem",
                "/tmp/to_decrypt",
                "/tmp/decrypted_file.txt",
        };

        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);

        try {
            executor->execute();
        } catch (std::exception &e) {
            /* dummy */
        }

        std::string afterPotentialCreate = testHelpers::readFileIntoString("/tmp/decrypted_file.txt");
        bool areSame = (afterPotentialCreate == "a");
        BOOST_CHECK_EQUAL(areSame, true);
    }

    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

BOOST_AUTO_TEST_CASE(ENCRYPT_CONTENT_CHECK)
{
    std::string content = "ewqewqeuywqoeyowqie8735614370856430857n14c78f18h8yxdhdanns7d78ae64bc07845ncg4w8X7MR";
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");
    {
        system("echo 0123456789abcdeF0123456789abcdeF > /tmp/key");
        system("echo 1234567887654321 > /tmp/iv");
        std::string syscall = "echo " + content + " > /tmp/file.txt";
        system(syscall.c_str());
        std::vector<std::string> input {
                "program",
                "encrypt-file",
                "/tmp/key",
                "/tmp/iv",
                "/tmp/file.txt",
                "/tmp/encrypted_file.txt",
                "overwrite"
        };

        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);
        executor->execute();

        std::string contentAfterDecryption = testHelpers::readFileIntoString("/tmp/file.txt");
        std::string encryptedFileContent = testHelpers::readFileIntoString("/tmp/encrypted_file.txt");
        bool areSame = (contentAfterDecryption == content) && (encryptedFileContent != content);
        BOOST_CHECK_EQUAL(areSame, true);
    }
    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}
BOOST_AUTO_TEST_CASE(ENCRYPT_DECRYPT)
{
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");
    {
        system("echo 3123213213 > /tmp/file_to_encrypt.txt");
        system("echo 0123456789abcdeF0123456789abcdeF > /tmp/key");
        system("echo 1234567887654321 > /tmp/iv");

        std::vector<std::string> input {
                "program",
                "encrypt-file",
                "/tmp/key",
                "/tmp/iv",
                "/tmp/file.txt",
                "/tmp/encrypted_file.txt",
                "overwrite"
        };

        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);
        executor->execute();
    }
    {
        std::vector<std::string> input {
                "program",
                "decrypt-file",
                "/tmp/key",
                "/tmp/iv",
                "/tmp/encrypted_file.txt",
                "/tmp/decrypted_file.txt",
                "overwrite"
        };

        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);
        auto msg = executor->execute();

        BOOST_CHECK_EQUAL(msg, "File decrypted");
    }
    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

BOOST_AUTO_TEST_CASE(ENCRYPT_DECRYPT_CONTENT_CHECK)
{
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");
    std::string content = "ewqewqeuywqoeyowqie8735614370856430857n14c78f18h8yxdhdanns7d78ae64bc07845ncg4w8X7MR0fd9";
    {
        std::string syscall = "echo " + content + " > /tmp/file.txt";
        system(syscall.c_str());
        system("echo 3123213213 > /tmp/file_to_encrypt.txt");
        system("echo 0123456789abcdeF0123456789abcdeF > /tmp/key");
        system("echo 1234567887654321 > /tmp/iv");

        std::vector<std::string> input {
                "program",
                "encrypt-file",
                "/tmp/key",
                "/tmp/iv",
                "/tmp/file.txt",
                "/tmp/encrypted_file.txt",
                "overwrite"
        };

        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);
        executor->execute();
    }
    {
        std::string contentAfterEncryption = testHelpers::readFileIntoString("/tmp/encrypted_file.txt");
        std::vector<std::string> input {
                "program",
                "decrypt-file",
                "/tmp/key",
                "/tmp/iv",
                "/tmp/encrypted_file.txt",
                "/tmp/decrypted_file.txt",
                "overwrite"
        };

        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);
        auto msg = executor->execute();

        std::string contentAfterEncryption2 = testHelpers::readFileIntoString("/tmp/encrypted_file.txt");
        std::string contentAfterDecryption = testHelpers::readFileIntoString("/tmp/decrypted_file.txt");
        bool areSame = (contentAfterDecryption == content);
        BOOST_CHECK_EQUAL(areSame, true);
    }
    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}



BOOST_AUTO_TEST_CASE(ENCRYPT_DECRYPT_WRONG_IV)
{
    std::string content = "ewqewqeuywqoeyowqie8735614370856430857n14c78f18h8yxdhdanns7d78ae64bc07845ncg4w";
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");
    {
        std::string syscall = "echo " + content + " > /tmp/file.txt";
        system(syscall.c_str());
        system("echo 3123213213 > /tmp/file_to_encrypt.txt");
        system("echo 0123456789abcdeF > /tmp/key");
        system("echo 1234567887654321 > /tmp/iv");

        std::vector<std::string> input {
                "program",
                "encrypt-file",
                "/tmp/key",
                "/tmp/iv",
                "/tmp/file.txt",
                "/tmp/encrypted_file.txt",
                "overwrite"
        };

        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);
        executor->execute();
    }
    {
        system("echo 123 > /tmp/iv");
        std::vector<std::string> input {
                "program",
                "decrypt-file",
                "/tmp/key",
                "/tmp/iv",
                "/tmp/encrypted_file.txt",
                "/tmp/decrypted_file.txt",
                "overwrite"
        };

        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);

        std::string contentAfterDecryption = testHelpers::readFileIntoString("/tmp/decrypted_file.txt");
        bool areSame = (contentAfterDecryption == content);
        BOOST_CHECK_EQUAL(areSame, false);
    }
    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

// a long test - fills whole partition with keys
BOOST_AUTO_TEST_CASE(CHECK_SIGNATURE_INTEGRATION_TEST_8)
{
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");
    for (int i = 0; i < -1; i++) {
        {
            std::vector<std::string> input {
                    "program",
                    "create-key",
                    "/tmp/private.pem",
                    "/tmp/public.pem",
                    "2048",
                    "RSA",
                    "pass",
                    "overwrite"
            };
            TerminalEmulation terminalEmulation(input);
            auto emulatedTerminalArgs = terminalEmulation.getArgs();
            auto argc = emulatedTerminalArgs.argc;
            auto argv = emulatedTerminalArgs.argv;

            auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
            auto parser = std::make_shared<Parser>(lexicalAnalyser);
            auto executor = std::make_shared<Executor>(parser);
            executor->execute();
        }
        {
            std::vector<std::string> input {
                    "program",
                    "sign",
                    "/tmp/private.pem",
                    "/tmp/file.txt",
                    "/tmp/signature.txt",
                    "pass"
            };

            TerminalEmulation terminalEmulation(input);
            auto emulatedTerminalArgs = terminalEmulation.getArgs();
            auto argc = emulatedTerminalArgs.argc;
            auto argv = emulatedTerminalArgs.argv;

            auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
            auto parser = std::make_shared<Parser>(lexicalAnalyser);
            auto executor = std::make_shared<Executor>(parser);
            executor->execute();
        }
        {
            std::vector<std::string> input {
                    "program",
                    "check-signature",
                    "/tmp/public.pem",
                    "/tmp/file.txt",
                    "/tmp/signature.txt",
            };

            TerminalEmulation terminalEmulation(input);
            auto emulatedTerminalArgs = terminalEmulation.getArgs();
            auto argc = emulatedTerminalArgs.argc;
            auto argv = emulatedTerminalArgs.argv;

            auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
            auto parser = std::make_shared<Parser>(lexicalAnalyser);
            auto executor = std::make_shared<Executor>(parser);
            auto msg = executor->execute();

            BOOST_CHECK_EQUAL(msg, "Signature correct\n");
        }
    }
    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

// a long test - fills whole partition with keys and checks PartitionInfo
BOOST_AUTO_TEST_CASE(CHECK_SIGNATURE_INTEGRATION_TEST_9) {
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");
        {
            std::vector<std::string> input {
                    "program",
                    "create-key",
                    "/tmp/private.pem",
                    "/tmp/public.pem",
                    "2048",
                    "RSA",
                    "pass",
                    "overwrite",
            };
            TerminalEmulation terminalEmulation(input);
            auto emulatedTerminalArgs = terminalEmulation.getArgs();
            auto argc = emulatedTerminalArgs.argc;
            auto argv = emulatedTerminalArgs.argv;

            auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
            auto parser = std::make_shared<Parser>(lexicalAnalyser);
            auto executor = std::make_shared<Executor>(parser);
            executor->execute();
            auto keysInPartition = executor->getCurrentInterface()->getCurrentKeyNum();
            BOOST_CHECK_EQUAL(1, keysInPartition);
        }

    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

// a long test - fills whole partition with keys and checks PartitionInfo
BOOST_AUTO_TEST_CASE(CHECK_SIGNATURE_INTEGRATION_TEST_10) {
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");
    for (int i = 0; i < 9; i++)
    {
        std::vector<std::string> input {
                "program",
                "create-key",
                "/tmp/private.pem",
                "/tmp/public.pem",
                "2048",
                "RSA",
                "pass",
                "overwrite",
        };
        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);
        executor->execute();
        auto keysInPartition = executor->getCurrentInterface()->getCurrentKeyNum();
    }
    {
        std::vector<std::string> input {
                "program",
                "create-key",
                "/tmp/private.pem",
                "/tmp/public.pem",
                "2048",
                "RSA",
                "pass",
                "overwrite"
        };
        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);
        executor->execute();
        auto keysInPartition = executor->getCurrentInterface()->getCurrentKeyNum();
        BOOST_CHECK_EQUAL(10, keysInPartition);
    }
    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

// a long test - fills whole partition with keys and checks PartitionInfo
BOOST_AUTO_TEST_CASE(CHECK_SIGNATURE_INTEGRATION_TEST_11) {
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");

    for (int i = 0; i < 1; i++)
    {
        std::vector<std::string> input {
                "program",
                "create-key",
                "/tmp/private.pem",
                "/tmp/public.pem",
                "2048",
                "RSA",
                "pass",
                "overwrite"
        };
        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);
        executor->execute();
        auto keysInPartition = executor->getCurrentInterface()->getCurrentKeyNum();
    }
    {
        std::vector<std::string> input {
                "program",
                "delete-key",
                "/tmp/private.pem",
                "/tmp/public.pem",
        };
        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);
        executor->execute();
        auto keysInPartition = executor->getCurrentInterface()->getCurrentKeyNum();

        BOOST_CHECK_EQUAL(0, keysInPartition);
    }
    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

BOOST_AUTO_TEST_CASE(CHECK_SIGNATURE_INTEGRATION_TEST_14) {
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");
    for(int i = 0; i < 1; i++)
    {
        std::vector<std::string> input {
                "program",
                "delete-key",
                "/tmp/private_test_14.pem",
                "/tmp/public.pem",
        };
        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);

        bool caught {false};
        try {
            executor->execute();
        } catch(std::exception &e) {
            auto a = e.what();
            if(e.what() == std::string("KeyIOInterface: File is not good")) {
                caught = true;
            }
        }

        BOOST_CHECK_EQUAL(caught, true);
    }
    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

BOOST_AUTO_TEST_CASE(DELETE_KEY_NOT_EXISTING_PARTITION) {
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");
    system("touch /tmp/public.pem");
    system("echo 1 > /tmp/private_test_14.pem");
    for(int i = 0; i < 1; i++)
    {
        std::vector<std::string> input {
                "program",
                "delete-key",
                "/tmp/private_test_14.pem",
                "/tmp/public.pem",
        };
        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);

        bool caught {false};
        try {
            executor->execute();
        } catch(std::exception &e) {
            auto a = e.what();
            if(e.what() == std::string("KeyIOInterface: Failed to remove private key")) {
                caught = true;
            }
        }

        BOOST_CHECK_EQUAL(caught, true);
    }
    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

BOOST_AUTO_TEST_CASE(GET_KEY_NOT_EXISTING_PARTITION) {
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");
    system("touch /tmp/public.pem");
    system("echo 1 > /tmp/private.pem");
    {
        std::vector<std::string> input {
                "program",
                "get-private-key",
                "/tmp/private.pem",
                "/tmp/saved_key",
                "overwrite"
        };
        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);

        bool caught {false};
        try {
            executor->execute();
        } catch(std::exception &e) {
            auto a = e.what();
            if(e.what() == std::string("KeyIOInterface: Cannot get private key")) {
                caught = true;
            }
        }

        BOOST_CHECK_EQUAL(caught, true);
    }
    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

BOOST_AUTO_TEST_CASE(SIGN_NOT_EXISTING_PARTITION) {
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");
    system("touch /tmp/public.pem");
    system("echo 1 > /tmp/sign_error_1_private.pem");
    system("echo 1 > /tmp/sign_error_1_file.txt");

    {
        std::vector<std::string> input {
                "program",
                "sign",
                "/tmp/sign_error_1_private.pem",
                "/tmp/sign_error_1_file.txt",
                "/tmp/sign_error_1_signature.txt",
                "overwrite",
                "pass"
        };
        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);

        bool caught {false};
        try {
            executor->execute();
        } catch(std::exception &e) {
            auto a = e.what();
            if(e.what() == std::string("KeyIOInterface: Cannot get private key")) {
                caught = true;
            }
        }

        BOOST_CHECK_EQUAL(caught, true);
    }
    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

BOOST_AUTO_TEST_CASE(DELETE_KEY_NOT_EXISTING_ID) {
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");
    system("touch /tmp/public.pem");
    system("echo 1 > /tmp/private_test_14.pem");
    {
        std::vector<std::string> input {
                "program",
                "create-key",
                "/tmp/private.pem",
                "/tmp/public.pem",
                "2048",
                "RSA",
                "pass",
                "overwrite",
        };
        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);
        executor->execute();
        auto keysInPartition = executor->getCurrentInterface()->getCurrentKeyNum();
    }
    {
        std::vector<std::string> input {
                "program",
                "delete-key",
                "/tmp/private_test_14.pem",
                "/tmp/public.pem",
        };
        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);

        bool caught {false};
        try {
            executor->execute();
        } catch(std::exception &e) {
            auto a = e.what();
            if(e.what() == std::string("KeyIOInterface: Failed to remove private key")) {
                caught = true;
            }
        }

        BOOST_CHECK_EQUAL(caught, true);
    }
    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

BOOST_AUTO_TEST_CASE(CHECK_SIGNATURE_INTEGRATION_TEST_15) {

    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");
    for (int i = 0; i < 1; i++)
    {
        std::vector<std::string> input {
                "program",
                "create-key",
                "/tmp/private.pem",
                "/tmp/public.pem",
                "2048",
                "RSA",
                "pass",
                "overwrite",
        };
        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);
        executor->execute();
        auto keysInPartition = executor->getCurrentInterface()->getCurrentKeyNum();
    }

    system("rm /tmp/public.pem");

    for(int i = 0; i < 1; i++)
    {
        std::vector<std::string> input {
                "program",
                "delete-key",
                "/tmp/private.pem",
                "/tmp/public.pem",
        };
        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);

        bool caught {false};
        try {
            executor->execute();
        } catch(std::exception &e) {
            auto dummy = e.what();
            if(e.what() == std::string("KeyIOInterface: File is not good")) {
                caught = true;
            }
        }

        BOOST_CHECK_EQUAL(caught, true);
    }
    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

BOOST_AUTO_TEST_CASE(WRONG_INPUT_1)
{
    bool errorFound = false;
    std::vector<std::string> input {
            "program",
            "unknown_command",
            "RSA",
            "pass",
            "2048",
            "/tmp/public.pem",
            "/tmp/private.pem",
    };
    TerminalEmulation terminalEmulation(input);
    auto emulatedTerminalArgs = terminalEmulation.getArgs();
    auto argc = emulatedTerminalArgs.argc;
    auto argv = emulatedTerminalArgs.argv;

    auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
    auto parser = std::make_shared<Parser>(lexicalAnalyser);
    auto executor = std::make_shared<Executor>(parser);

    try {
        executor->execute();
    } catch (std::exception &e) {
        errorFound = true;
    }
    BOOST_CHECK_EQUAL(errorFound, true);
}

BOOST_AUTO_TEST_CASE(WRONG_INPUT_2)
{
    bool errorFound = false;
    std::vector<std::string> input {
            "program",
            "create-key",
            "/tmp/private.pem",
            "/tmp/public.pem",
            "2048",
            "NO-ALGORITHM",

    };
    TerminalEmulation terminalEmulation(input);
    auto emulatedTerminalArgs = terminalEmulation.getArgs();
    auto argc = emulatedTerminalArgs.argc;
    auto argv = emulatedTerminalArgs.argv;

    auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
    auto parser = std::make_shared<Parser>(lexicalAnalyser);
    auto executor = std::make_shared<Executor>(parser);

    try {
        executor->execute();
    } catch (std::exception &e) {
        errorFound = true;
    }

    BOOST_CHECK_EQUAL(errorFound, true);
}

BOOST_AUTO_TEST_CASE(WRONG_INPUT_3)
{
    bool errorFound = false;
    std::vector<std::string> input {
            "program",
            "create-key",
            "/tmp/private.pem",
            "/tmp/public.pem",
            "-2048",
            "RSA",
            "pass",
    };
    TerminalEmulation terminalEmulation(input);
    auto emulatedTerminalArgs = terminalEmulation.getArgs();
    auto argc = emulatedTerminalArgs.argc;
    auto argv = emulatedTerminalArgs.argv;

    auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
    auto parser = std::make_shared<Parser>(lexicalAnalyser);
    auto executor = std::make_shared<Executor>(parser);

    try {
        executor->execute();
    } catch (std::exception &e) {
        errorFound = true;
    }

    BOOST_CHECK_EQUAL(errorFound, true);
}

BOOST_AUTO_TEST_CASE(WRONG_INPUT_4)
{
    bool errorFound = false;
    std::vector<std::string> input {
            "program",
            "create-key",
            "/tmp/private.pem",
            "/tmp/public.pem",
            "0",
            "RSA",
            "pass",
    };
    TerminalEmulation terminalEmulation(input);
    auto emulatedTerminalArgs = terminalEmulation.getArgs();
    auto argc = emulatedTerminalArgs.argc;
    auto argv = emulatedTerminalArgs.argv;

    auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
    auto parser = std::make_shared<Parser>(lexicalAnalyser);
    auto executor = std::make_shared<Executor>(parser);

    try {
        executor->execute();
    } catch (std::exception &e) {
        errorFound = true;
    }

    BOOST_CHECK_EQUAL(errorFound, true);
}
BOOST_AUTO_TEST_CASE(WRONG_INPUT_5)
{
    bool errorFound = false;
    std::vector<std::string> input {
            "program",
            "create-key",
            "/tmp/private.pem",
            "/tmp/public.pem",
            "10000000",
            "RSA",
            "pass",
    };
    TerminalEmulation terminalEmulation(input);
    auto emulatedTerminalArgs = terminalEmulation.getArgs();
    auto argc = emulatedTerminalArgs.argc;
    auto argv = emulatedTerminalArgs.argv;

    auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
    auto parser = std::make_shared<Parser>(lexicalAnalyser);
    auto executor = std::make_shared<Executor>(parser);

    try {
        executor->execute();
    } catch (std::exception &e) {
        errorFound = true;
    }

    BOOST_CHECK_EQUAL(errorFound, true);
}
BOOST_AUTO_TEST_CASE(WRONG_INPUT_6)
{
    bool errorFound = false;
    std::vector<std::string> input {
            "program",
            "create-key",
            "/tmp/private.pem",
            "/tmp/public.pem",
            "100000000000000000000",
            "RSA",
            "pass",
    };
    TerminalEmulation terminalEmulation(input);
    auto emulatedTerminalArgs = terminalEmulation.getArgs();
    auto argc = emulatedTerminalArgs.argc;
    auto argv = emulatedTerminalArgs.argv;

    auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
    auto parser = std::make_shared<Parser>(lexicalAnalyser);
    auto executor = std::make_shared<Executor>(parser);

    try {
        executor->execute();
    } catch (std::exception &e) {
        errorFound = true;
    }

    BOOST_CHECK_EQUAL(errorFound, true);
}

BOOST_AUTO_TEST_CASE(WRONG_INPUT_7)
{
    bool errorFound = false;
    std::vector<std::string> input {
            "program",
            "create-key",
            "/tmp/private.pem",
            "qwertyuio",
            "2W0W1",
            "RSA",
            "pass",
            "overwrite"
    };
    TerminalEmulation terminalEmulation(input);
    auto emulatedTerminalArgs = terminalEmulation.getArgs();
    auto argc = emulatedTerminalArgs.argc;
    auto argv = emulatedTerminalArgs.argv;

    auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
    auto parser = std::make_shared<Parser>(lexicalAnalyser);
    auto executor = std::make_shared<Executor>(parser);

    bool caught {false};
    try {
        executor->execute();
    } catch (std::exception &e) {

        auto msg = e.what();
        if(e.what() == std::string("Parser: Error in keyLen - unacceptable symbol")) {
            caught = true;
        }
    }

    BOOST_CHECK_EQUAL(caught, true);
}

BOOST_AUTO_TEST_CASE(SIGN_ERROR_1) {

    {
        system("touch /tmp/sign_error_1_file.txt");
        system("rm /tmp/sign_error_1_signature.txt");
        bool errorFound = false;
        std::vector<std::string> input{
                "program",
                "sign",
                "/tmp/sign_error_1_private.pem",
                "/tmp/sign_error_1_file.txt",
                "/tmp/sign_error_1_signature.txt",
                "pass"
        };
        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);

        bool caught {false};
        try {
            executor->execute();
        } catch(std::exception &e) {
            if(e.what() == std::string("KeyIOInterface: Cannot get private key")) {
                caught = true;
            }
        }

        BOOST_CHECK_EQUAL(caught, true);
    }
}

BOOST_AUTO_TEST_CASE(SIGN_ERROR_NOT_EXISTING_DATA_TO_SIGN) {
    {
        system("rm /tmp/sign_error_not_existing_file.txt");
        bool errorFound = false;
        std::vector<std::string> input{
                "program",
                "sign",
                "/tmp/sign_error_1_private.pem",
                "/tmp/sign_error_not_existing_file.txt",
                "/tmp/sign_error_1_signature.txt",
                "overwrite",
                "pass"
        };
        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);

        bool caught {false};
        try {
            executor->execute();
        } catch(std::exception &e) {
            if(e.what() == std::string("KeyIOInterface: Failed to read file")) {
                caught = true;
            }
        }

        BOOST_CHECK_EQUAL(caught, true);
    }
}

BOOST_AUTO_TEST_CASE(SIGN_ERROR_2)
{
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");
    {
        std::vector<std::string> input{
                "program",
                "create-key",
                "/tmp/sign_error_2_private.pem",
                "/tmp/sign_error_2_public.pem",
                "2048",
                "RSA",
                "pass",
                "overwrite"
        };
        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);
        executor->execute();
        auto keysInPartition = executor->getCurrentInterface()->getCurrentKeyNum();
    }
    {
        system("echo a >> /tmp/sign_error_2_file.txt");
        bool errorFound = false;
        std::vector<std::string> input{
                "program",
                "sign",
                "/tmp/sign_error_2_private.pem",
                "/tmp/sign_error_2_file.txt",
                "/tmp/sign_error_2_signature.txt",
                "pass",
                "overwrite",
        };
        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);
        auto msg = executor->execute();

        BOOST_CHECK_EQUAL(msg, "File signed");
    }
    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}
BOOST_AUTO_TEST_CASE(SIGN_ERROR_3)
{
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");
    {
        std::vector<std::string> input{
                "program",
                "create-key",
                "/tmp/sign_error_3_private.pem",
                "/tmp/sign_error_3_public.pem",
                "2048",
                "RSA",
                "pass",
                "overwrite"
        };
        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);
        executor->execute();
        auto keysInPartition = executor->getCurrentInterface()->getCurrentKeyNum();
    }
    {
        system("touch /tmp/sign_error_3_file.txt");
        system("echo 2 >> /tmp/sign_error_3_private.pem");
        bool errorFound = false;
        std::vector<std::string> input{
                "program",
                "sign",
                "/tmp/sign_error_3_private.pem",
                "/tmp/sign_error_3_file.txt",
                "/tmp/sign_error_3_signature.txt",
                "pass",
        };
        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);

        bool caught {false};
        try {
            executor->execute();
        } catch(std::exception &e) {
            if(e.what() == std::string("KeyIOInterface: Cannot get private key")) {
                caught = true;
            }
        }

        BOOST_CHECK_EQUAL(caught, true);
    }
    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

BOOST_AUTO_TEST_CASE(SIGN_ERROR_4)
{
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");
    {
        std::vector<std::string> input{
                "program",
                "create-key",
                "/tmp/sign_error_4_private.pem",
                "/tmp/sign_error_4_public.pem",
                "2048",
                "RSA",
                "pass",
                "overwrite"
        };
        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);
        executor->execute();
        auto keysInPartition = executor->getCurrentInterface()->getCurrentKeyNum();
    }
    {
        system("touch /tmp/sign_error_4_file.txt");
        system("echo a > /tmp/sign_error_4_private.pem");
        bool errorFound = false;
        std::vector<std::string> input{
                "program",
                "sign",
                "/tmp/sign_error_4_private.pem",
                "/tmp/sign_error_4_file.txt",
                "/tmp/sign_error_4_signature.txt",
                "pass",
                "overwrite"
        };
        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);

        bool caught {false};
        try {
            executor->execute();
        } catch(std::exception &e) {
            auto tmp = e.what();
            if(e.what() == std::string("KeyIOInterface: Cannot get private key")) {
                caught = true;
            }
        }

        BOOST_CHECK_EQUAL(caught, true);
    }
    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

BOOST_AUTO_TEST_CASE(CHECK_SIGNATURE_WRONG_FILE)
{
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");
    for (int i = 0; i < 1; i++) {
        {
            std::vector<std::string> input{
                    "program",
                    "create-key",
                    "/tmp/CHECK_SIGNATURE_WRONG_FILE_private.pem",
                    "/tmp/CHECK_SIGNATURE_WRONG_FILE_public.pem",
                    "2048",
                    "RSA",
                    "pass",
                    "overwrite"
            };
            TerminalEmulation terminalEmulation(input);
            auto emulatedTerminalArgs = terminalEmulation.getArgs();
            auto argc = emulatedTerminalArgs.argc;
            auto argv = emulatedTerminalArgs.argv;

            auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
            auto parser = std::make_shared<Parser>(lexicalAnalyser);
            auto executor = std::make_shared<Executor>(parser);
            executor->execute();
        }
        {
            system("echo a > /tmp/CHECK_SIGNATURE_WRONG_FILE_file.txt");
            std::vector<std::string> input{
                    "program",
                    "sign",
                    "/tmp/CHECK_SIGNATURE_WRONG_FILE_private.pem",
                    "/tmp/CHECK_SIGNATURE_WRONG_FILE_file.txt",
                    "/tmp/CHECK_SIGNATURE_WRONG_FILE_signature.txt",
                    "pass",
                    "overwrite"
            };

            TerminalEmulation terminalEmulation(input);
            auto emulatedTerminalArgs = terminalEmulation.getArgs();
            auto argc = emulatedTerminalArgs.argc;
            auto argv = emulatedTerminalArgs.argv;

            auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
            auto parser = std::make_shared<Parser>(lexicalAnalyser);
            auto executor = std::make_shared<Executor>(parser);
            executor->execute();
        }
        {
            system("echo a >> /tmp/CHECK_SIGNATURE_WRONG_FILE_file.txt");
            std::vector<std::string> input{
                    "program",
                    "check-signature",
                    "/tmp/CHECK_SIGNATURE_WRONG_FILE_public.pem",
                    "/tmp/CHECK_SIGNATURE_WRONG_FILE_file.txt",
                    "/tmp/CHECK_SIGNATURE_WRONG_FILE_signature.txt",
            };

            TerminalEmulation terminalEmulation(input);
            auto emulatedTerminalArgs = terminalEmulation.getArgs();
            auto argc = emulatedTerminalArgs.argc;
            auto argv = emulatedTerminalArgs.argv;

            auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
            auto parser = std::make_shared<Parser>(lexicalAnalyser);
            auto executor = std::make_shared<Executor>(parser);
            auto msg = executor->execute();

            BOOST_CHECK_EQUAL(msg, "Signature not correct");
        }
    }
    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

BOOST_AUTO_TEST_CASE(CHECK_SIGNATURE_NOT_EXISTING_FILE)
{
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");
    for (int i = 0; i < 1; i++) {
        {
            std::vector<std::string> input{
                    "program",
                    "create-key",
                    "/tmp/CHECK_SIGNATURE_WRONG_FILE_private.pem",
                    "/tmp/CHECK_SIGNATURE_WRONG_FILE_public.pem",
                    "2048",
                    "RSA",
                    "pass",
                    "overwrite"
            };
            TerminalEmulation terminalEmulation(input);
            auto emulatedTerminalArgs = terminalEmulation.getArgs();
            auto argc = emulatedTerminalArgs.argc;
            auto argv = emulatedTerminalArgs.argv;

            auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
            auto parser = std::make_shared<Parser>(lexicalAnalyser);
            auto executor = std::make_shared<Executor>(parser);
            executor->execute();
        }
        {
            system("echo a > /tmp/CHECK_SIGNATURE_WRONG_FILE_file.txt");
            std::vector<std::string> input{
                    "program",
                    "sign",
                    "/tmp/CHECK_SIGNATURE_WRONG_FILE_private.pem",
                    "/tmp/CHECK_SIGNATURE_WRONG_FILE_file.txt",
                    "/tmp/CHECK_SIGNATURE_WRONG_FILE_signature.txt",
                    "pass",
                    "overwrite"
            };

            TerminalEmulation terminalEmulation(input);
            auto emulatedTerminalArgs = terminalEmulation.getArgs();
            auto argc = emulatedTerminalArgs.argc;
            auto argv = emulatedTerminalArgs.argv;

            auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
            auto parser = std::make_shared<Parser>(lexicalAnalyser);
            auto executor = std::make_shared<Executor>(parser);
            executor->execute();
        }
        {
            system("rm /tmp/CHECK_SIGNATURE_WRONG_FILE_file.txt");
            std::vector<std::string> input{
                    "program",
                    "check-signature",
                    "/tmp/CHECK_SIGNATURE_WRONG_FILE_public.pem",
                    "/tmp/CHECK_SIGNATURE_WRONG_FILE_file.txt",
                    "/tmp/CHECK_SIGNATURE_WRONG_FILE_signature.txt",
            };

            TerminalEmulation terminalEmulation(input);
            auto emulatedTerminalArgs = terminalEmulation.getArgs();
            auto argc = emulatedTerminalArgs.argc;
            auto argv = emulatedTerminalArgs.argv;

            auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
            auto parser = std::make_shared<Parser>(lexicalAnalyser);
            auto executor = std::make_shared<Executor>(parser);

            bool caught {false};
            try {
                executor->execute();
            } catch(std::exception &e) {
                if(e.what() == std::string("KeyIOInterface: Failed to read file")) {
                    caught = true;
                }
            }

            BOOST_CHECK_EQUAL(caught, true);
        }
    }
    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

BOOST_AUTO_TEST_CASE(CHECK_SIGNATURE_EXISTING_BUT_WRONG_KEY)
{
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");
    for (int i = 0; i < 1; i++) {
        {
            std::vector<std::string> input{
                    "program",
                    "create-key",
                    "/tmp/CHECK_SIGNATURE_WRONG_FILE_private.pem",
                    "/tmp/CHECK_SIGNATURE_WRONG_FILE_public.pem",
                    "2048",
                    "RSA",
                    "pass",
                    "overwrite"
            };
            TerminalEmulation terminalEmulation(input);
            auto emulatedTerminalArgs = terminalEmulation.getArgs();
            auto argc = emulatedTerminalArgs.argc;
            auto argv = emulatedTerminalArgs.argv;

            auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
            auto parser = std::make_shared<Parser>(lexicalAnalyser);
            auto executor = std::make_shared<Executor>(parser);
            executor->execute();
        }
        {
            std::vector<std::string> input{
                    "program",
                    "create-key",
                    "/tmp/CHECK_SIGNATURE_WRONG_FILE_private2.pem",
                    "/tmp/CHECK_SIGNATURE_WRONG_FILE_public2.pem",
                    "2048",
                    "RSA",
                    "pass",
                    "overwrite"
            };
            TerminalEmulation terminalEmulation(input);
            auto emulatedTerminalArgs = terminalEmulation.getArgs();
            auto argc = emulatedTerminalArgs.argc;
            auto argv = emulatedTerminalArgs.argv;

            auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
            auto parser = std::make_shared<Parser>(lexicalAnalyser);
            auto executor = std::make_shared<Executor>(parser);
            executor->execute();
        }
        {
            system("touch /tmp/CHECK_SIGNATURE_WRONG_FILE_file.txt");
            std::vector<std::string> input{
                    "program",
                    "sign",
                    "/tmp/CHECK_SIGNATURE_WRONG_FILE_private.pem",
                    "/tmp/CHECK_SIGNATURE_WRONG_FILE_file.txt",
                    "/tmp/CHECK_SIGNATURE_WRONG_FILE_signature.txt",
                    "pass",
                    "overwrite"
            };

            TerminalEmulation terminalEmulation(input);
            auto emulatedTerminalArgs = terminalEmulation.getArgs();
            auto argc = emulatedTerminalArgs.argc;
            auto argv = emulatedTerminalArgs.argv;

            auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
            auto parser = std::make_shared<Parser>(lexicalAnalyser);
            auto executor = std::make_shared<Executor>(parser);
            executor->execute();
        }
        {
            std::vector<std::string> input{
                    "program",
                    "check-signature",
                    "/tmp/CHECK_SIGNATURE_WRONG_FILE_public2.pem",
                    "/tmp/CHECK_SIGNATURE_WRONG_FILE_file.txt",
                    "/tmp/CHECK_SIGNATURE_WRONG_FILE_signature.txt",
            };

            TerminalEmulation terminalEmulation(input);
            auto emulatedTerminalArgs = terminalEmulation.getArgs();
            auto argc = emulatedTerminalArgs.argc;
            auto argv = emulatedTerminalArgs.argv;

            auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
            auto parser = std::make_shared<Parser>(lexicalAnalyser);
            auto executor = std::make_shared<Executor>(parser);
            auto msg = executor->execute();

            BOOST_CHECK_EQUAL(msg, "Signature not correct");

        }
    }
    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

BOOST_AUTO_TEST_CASE(CHECK_SIGNATURE_CONTENT_CHANGE)
{
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");
    for (int i = 0; i < 1; i++) {
        {
            std::vector<std::string> input{
                    "program",
                    "create-key",
                    "/tmp/CHECK_SIGNATURE_WRONG_PUB_private.pem",
                    "/tmp/CHECK_SIGNATURE_WRONG_PUB_public.pem",
                    "2048",
                    "RSA",
                    "pass",
                    "overwrite"
            };
            TerminalEmulation terminalEmulation(input);
            auto emulatedTerminalArgs = terminalEmulation.getArgs();
            auto argc = emulatedTerminalArgs.argc;
            auto argv = emulatedTerminalArgs.argv;

            auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
            auto parser = std::make_shared<Parser>(lexicalAnalyser);
            auto executor = std::make_shared<Executor>(parser);
            executor->execute();
        }
        {
            system("touch /tmp/CHECK_SIGNATURE_WRONG_PUB_file.txt");
            std::vector<std::string> input{
                    "program",
                    "sign",
                    "/tmp/CHECK_SIGNATURE_WRONG_PUB_private.pem",
                    "/tmp/CHECK_SIGNATURE_WRONG_PUB_file.txt",
                    "/tmp/CHECK_SIGNATURE_WRONG_PUB_signature.txt",
                    "pass",
                    "overwrite"
            };

            TerminalEmulation terminalEmulation(input);
            auto emulatedTerminalArgs = terminalEmulation.getArgs();
            auto argc = emulatedTerminalArgs.argc;
            auto argv = emulatedTerminalArgs.argv;

            auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
            auto parser = std::make_shared<Parser>(lexicalAnalyser);
            auto executor = std::make_shared<Executor>(parser);
            executor->execute();
        }
        {
            std::string fileToChange = "/tmp/CHECK_SIGNATURE_WRONG_PUB_public.pem";
            struct stat st;
            stat(fileToChange.c_str(), &st);
            size_t fileSize = st.st_size;

            int fd = open(fileToChange.c_str(), O_RDWR, 0);
            if(fd < 0) {
                std::cout << "File cannot be changed" << std::endl;
            }

            void* mappedPartition = mmap(nullptr, fileSize, PROT_WRITE | PROT_READ,  MAP_SHARED, fd, 0);

            memset((uint8_t* )mappedPartition + 50, 0x41, sizeof(char) * 10);

            if(mappedPartition == MAP_FAILED) {
                close(fd);
            }

            int ret = munmap(mappedPartition, fileSize);
            close(fd);
            assert(ret == 0);

            std::vector<std::string> input{
                    "program",
                    "check-signature",
                    "/tmp/CHECK_SIGNATURE_WRONG_PUB_public.pem",
                    "/tmp/CHECK_SIGNATURE_WRONG_PUB_file.txt",
                    "/tmp/CHECK_SIGNATURE_WRONG_PUB_signature.txt",
            };

            TerminalEmulation terminalEmulation(input);
            auto emulatedTerminalArgs = terminalEmulation.getArgs();
            auto argc = emulatedTerminalArgs.argc;
            auto argv = emulatedTerminalArgs.argv;

            auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
            auto parser = std::make_shared<Parser>(lexicalAnalyser);
            auto executor = std::make_shared<Executor>(parser);

            bool caught {false};
            try {
                executor->execute();
            } catch (std::exception &e) {
                if(e.what() == std::string("KeyIOInterface: Could not read pubkey from file")) {
                    caught = true;
                }
            }

            BOOST_CHECK_EQUAL(caught, true);
        }
    }
    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

BOOST_AUTO_TEST_CASE(CHECK_SIGNATURE_WRONG_SIGNATURE)
{
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");
    for (int i = 0; i < 1; i++) {
        {
            std::vector<std::string> input{
                    "program",
                    "create-key",
                    "/tmp/CHECK_SIGNATURE_WRONG_SIGNATURE_private.pem",
                    "/tmp/CHECK_SIGNATURE_WRONG_SIGNATURE_public.pem",
                    "2048",
                    "RSA",
                    "pass",
                    "overwrite"
            };
            TerminalEmulation terminalEmulation(input);
            auto emulatedTerminalArgs = terminalEmulation.getArgs();
            auto argc = emulatedTerminalArgs.argc;
            auto argv = emulatedTerminalArgs.argv;

            auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
            auto parser = std::make_shared<Parser>(lexicalAnalyser);
            auto executor = std::make_shared<Executor>(parser);
            executor->execute();
        }
        {
            system("touch /tmp/CHECK_SIGNATURE_WRONG_SIGNATURE_file.txt");
            std::vector<std::string> input{
                    "program",
                    "sign",
                    "/tmp/CHECK_SIGNATURE_WRONG_SIGNATURE_private.pem",
                    "/tmp/CHECK_SIGNATURE_WRONG_SIGNATURE_file.txt",
                    "/tmp/CHECK_SIGNATURE_WRONG_SIGNATURE_signature.txt",
                    "pass",
                    "overwrite"
            };

            TerminalEmulation terminalEmulation(input);
            auto emulatedTerminalArgs = terminalEmulation.getArgs();
            auto argc = emulatedTerminalArgs.argc;
            auto argv = emulatedTerminalArgs.argv;

            auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
            auto parser = std::make_shared<Parser>(lexicalAnalyser);
            auto executor = std::make_shared<Executor>(parser);
            executor->execute();
        }
        {
            system("echo a >> /tmp/CHECK_SIGNATURE_WRONG_SIGNATURE_signature.txt");
            std::vector<std::string> input{
                    "program",
                    "check-signature",
                    "/tmp/CHECK_SIGNATURE_WRONG_SIGNATURE_public.pem",
                    "/tmp/CHECK_SIGNATURE_WRONG_SIGNATURE_file.txt",
                    "/tmp/CHECK_SIGNATURE_WRONG_SIGNATURE_signature.txt",
            };

            TerminalEmulation terminalEmulation(input);
            auto emulatedTerminalArgs = terminalEmulation.getArgs();
            auto argc = emulatedTerminalArgs.argc;
            auto argv = emulatedTerminalArgs.argv;

            auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
            auto parser = std::make_shared<Parser>(lexicalAnalyser);
            auto executor = std::make_shared<Executor>(parser);
            auto msg = executor->execute();
            BOOST_CHECK_EQUAL(msg, "Signature not correct");
        }
    }
    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

BOOST_AUTO_TEST_CASE(GET_PRIVATE_KEY_NOT_EXISTING_ID)
{
    system("echo 1 > /tmp/key_not_existing_id");
    std::vector<std::string> input {
            "program",
            "get-private-key",
            "/tmp/key_not_existing_id",
            "/tmp/saved_key",
            "overwrite"
    };
    TerminalEmulation terminalEmulation(input);
    auto args = terminalEmulation.getArgs();
    auto argc = args.argc;
    auto argv = args.argv;

    auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
    auto parser = std::make_shared<Parser>(lexicalAnalyser);
    auto executor = std::make_shared<Executor>(parser);

    bool caught {false};
    try {
        executor->execute();
    } catch(std::exception &e) {
        auto tmp = e.what();
        if(e.what() == std::string("KeyIOInterface: Cannot get private key")) {
            caught = true;
        }
    }

    BOOST_CHECK_EQUAL(caught, true);
}

BOOST_AUTO_TEST_CASE(GET_PRIVATE_KEY_NOT_EXISTING_FILE)
{
    std::vector<std::string> input {
            "program",
            "get-private-key",
            "not_existing_file",
            "/tmp/key",
            "pass",
            "overwrite"
    };
    TerminalEmulation terminalEmulation(input);
    auto args = terminalEmulation.getArgs();
    auto argc = args.argc;
    auto argv = args.argv;

    auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
    auto parser = std::make_shared<Parser>(lexicalAnalyser);
    auto executor = std::make_shared<Executor>(parser);

    bool caught {false};
    try {
        executor->execute();
    } catch(std::exception &e) {
        auto tmp = e.what();
        if(e.what() == std::string("KeyIOInterface: Failed to read file")) {
            caught = true;
        }
    }

    BOOST_CHECK_EQUAL(caught, true);
}

BOOST_AUTO_TEST_CASE(GET_PRIVATE_KEY_TEST)
{
    std::vector<std::string> input {
            "program",
            "get-private-key",
            "11111111111111111",
            "/tmp/get-private-key",
            "overwrite"
    };
    TerminalEmulation terminalEmulation(input);
    auto args = terminalEmulation.getArgs();
    auto argc = args.argc;
    auto argv = args.argv;

    auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
    auto parser = std::make_shared<Parser>(lexicalAnalyser);
    auto executor = std::make_shared<Executor>(parser);

    bool caught {false};
    try {
        executor->execute();
    } catch(std::exception &e) {
        auto tmp = e.what();
        if(e.what() == std::string("KeyIOInterface: Failed to read file")) {
            caught = true;
        }
    }

    BOOST_CHECK_EQUAL(caught, true);
}

BOOST_AUTO_TEST_CASE(GET_PRIVATE_KEY_NO_OVERWRITE)
{
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");
    {
        std::vector<std::string> input {
                "program",
                "create-key",
                "/tmp/prvkey.pem",
                "/tmp/pubkey.pem",
                "2048",
                "RSA",
                "pass",
                "overwrite",
        };
        TerminalEmulation terminalEmulation(input);
        auto args = terminalEmulation.getArgs();
        auto argc = args.argc;
        auto argv = args.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);
        executor->execute();
    }
    system("touch /tmp/get_prv_no_overwrite");
    std::vector<std::string> input {
            "program",
            "get-private-key",
            "/tmp/prvkey.pem",
            "/tmp/get_prv_no_overwrite",
    };
    TerminalEmulation terminalEmulation(input);
    auto args = terminalEmulation.getArgs();
    auto argc = args.argc;
    auto argv = args.argv;

    auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
    auto parser = std::make_shared<Parser>(lexicalAnalyser);
    auto executor = std::make_shared<Executor>(parser);

    bool error_caught {false};
    try {
        executor->execute();
    } catch (std::exception &e) {
        error_caught = true;
    }

    BOOST_CHECK_EQUAL(error_caught, true);
    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

BOOST_AUTO_TEST_CASE(OVERWRITE_FLAG_CREATE_KEY_TEST)
{
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");
    std::vector<std::string> input {
            "program",
            "create-key",
            "/home/r.ostoja",
            "/home/r.ostoja",
            "2048",
            "SHA",
            "overwrite",
    };
    TerminalEmulation terminalEmulation(input);
    auto args = terminalEmulation.getArgs();
    auto argc = args.argc;
    auto argv = args.argv;

    auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
    auto parser = std::make_unique<Parser>(lexicalAnalyser);
    parser->parse();
    auto statement = parser->getCurrentParsedStatementStr();
    auto serialisedInput = testHelpers::toString(input);
    BOOST_CHECK_EQUAL(statement, serialisedInput);
    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}
BOOST_AUTO_TEST_CASE(OVERWRITE_FLAG_GET_PRV_TEST)
{
    std::vector<std::string> input {
            "program",
            "get-private-key",
            "413243",
            "/home/abc",
            "overwrite",
    };
    TerminalEmulation terminalEmulation(input);
    auto args = terminalEmulation.getArgs();
    auto argc = args.argc;
    auto argv = args.argv;

    auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
    auto parser = std::make_unique<Parser>(lexicalAnalyser);
    parser->parse();
    auto statement = parser->getCurrentParsedStatementStr();
    auto serialisedInput = testHelpers::toString(input);
    BOOST_CHECK_EQUAL(statement, serialisedInput);
}

BOOST_AUTO_TEST_CASE(OVERWRITE_FLAG_SIGN_TEST)
{
    std::vector<std::string> input {
            "program_sign",
            "sign",
            "outpath",
            "/home/abc",
            "312321",
            "pass",
            "overwrite",
    };
    TerminalEmulation terminalEmulation(input);
    auto args = terminalEmulation.getArgs();
    auto argc = args.argc;
    auto argv = args.argv;

    auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
    auto parser = std::make_unique<Parser>(lexicalAnalyser);
    parser->parse();
    auto statement = parser->getCurrentParsedStatementStr();
    auto serialisedInput = testHelpers::toString(input);
    BOOST_CHECK_EQUAL(statement, serialisedInput);
}

BOOST_AUTO_TEST_CASE(OVERWRITE_FLAG_ENCRYPT_FILE_TEST)
{
    std::vector<std::string> input {
            "program",
            "encrypt-file",
            "/home/file-to-encrypt",
            "output",
            "413243",
            "overwrite",
    };
    TerminalEmulation terminalEmulation(input);
    auto args = terminalEmulation.getArgs();
    auto argc = args.argc;
    auto argv = args.argv;

    auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
    auto parser = std::make_unique<Parser>(lexicalAnalyser);
    parser->parse();
    auto statement = parser->getCurrentParsedStatementStr();
    auto serialisedInput = testHelpers::toString(input);
    BOOST_CHECK_EQUAL(statement, serialisedInput);
}
BOOST_AUTO_TEST_CASE(OVERWRITE_FLAG_DECRYPT_FILE_TEST)
{
    std::vector<std::string> input {
            "program",
            "decrypt-file",
            "/home/file-to-decrypt",
            "output",
            "413243",
            "overwrite",
    };
    TerminalEmulation terminalEmulation(input);
    auto args = terminalEmulation.getArgs();
    auto argc = args.argc;
    auto argv = args.argv;

    auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
    auto parser = std::make_unique<Parser>(lexicalAnalyser);
    parser->parse();
    auto statement = parser->getCurrentParsedStatementStr();
    auto serialisedInput = testHelpers::toString(input);
    BOOST_CHECK_EQUAL(statement, serialisedInput);
}

BOOST_AUTO_TEST_CASE(OVERWRITE_FLAG_CREATE_KEY_TEST_PUB_POS) {
    std::string contentBeforeOverwrite;
    std::string contentAfterOverwrite;

    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");
    {
        std::vector<std::string> input{
                "program",
                "create-key",
                "/tmp/to_overwrite_prv.pem",
                "/tmp/to_overwrite.pem",
                "2048",
                "RSA",
                "pass",
                "overwrite",
        };
        TerminalEmulation terminalEmulation(input);
        auto args = terminalEmulation.getArgs();
        auto argc = args.argc;
        auto argv = args.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);
        executor->execute();
        contentBeforeOverwrite = testHelpers::readFileIntoString("/tmp/to_overwrite.pem");
    }
    {
        std::vector<std::string> input{
                "program",
                "create-key",
                "/tmp/to_overwrite_prv.pem",
                "/tmp/to_overwrite.pem",
                "2048",
                "RSA",
                "pass",
                "overwrite",
        };
        TerminalEmulation terminalEmulation(input);
        auto args = terminalEmulation.getArgs();
        auto argc = args.argc;
        auto argv = args.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);
        executor->execute();

        contentAfterOverwrite = testHelpers::readFileIntoString("/tmp/to_overwrite.pem");
        bool areSame = (contentBeforeOverwrite == contentAfterOverwrite);

        BOOST_CHECK_EQUAL(areSame, false);
    }
    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

BOOST_AUTO_TEST_CASE(OVERWRITE_FLAG_CREATE_KEY_TEST_PRV_POS) {
    std::string contentBeforeOverwrite;
    std::string contentAfterOverwrite;

    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");
    {
        std::vector<std::string> input{
                "program",
                "create-key",
                "/tmp/to_overwrite_prv.pem",
                "/tmp/to_overwrite.pem",
                "2048",
                "RSA",
                "pass",
                "overwrite",
        };
        TerminalEmulation terminalEmulation(input);
        auto args = terminalEmulation.getArgs();
        auto argc = args.argc;
        auto argv = args.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);
        executor->execute();
        contentBeforeOverwrite = testHelpers::readFileIntoString("/tmp/to_overwrite_prv.pem");
    }
    {
        std::vector<std::string> input{
                "program",
                "create-key",
                "/tmp/to_overwrite_prv.pem",
                "/tmp/to_overwrite.pem",
                "2048",
                "RSA",
                "pass",
                "overwrite",
        };
        TerminalEmulation terminalEmulation(input);
        auto args = terminalEmulation.getArgs();
        auto argc = args.argc;
        auto argv = args.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);
        executor->execute();

        contentAfterOverwrite = testHelpers::readFileIntoString("/tmp/to_overwrite_prv.pem");
        bool areSame = (contentBeforeOverwrite == contentAfterOverwrite);

        BOOST_CHECK_EQUAL(areSame, false);
    }
    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}


BOOST_AUTO_TEST_CASE(OVERWRITE_FLAG_CREATE_KEY_TEST_PRV_NEG) {
    std::string contentBeforeOverwrite;
    std::string contentAfterOverwrite;

    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");
    {
        std::vector<std::string> input{
                "program",
                "create-key",
                "/tmp/to_overwrite_prv.pem",
                "/tmp/to_overwrite.pem",
                "2048",
                "RSA",
                "pass",
                "overwrite",
        };
        TerminalEmulation terminalEmulation(input);
        auto args = terminalEmulation.getArgs();
        auto argc = args.argc;
        auto argv = args.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);
        executor->execute();
        contentBeforeOverwrite = testHelpers::readFileIntoString("/tmp/to_overwrite_prv.pem");
    }
    {
        std::vector<std::string> input{
                "program",
                "create-key",
                "/tmp/to_overwrite_prv.pem",
                "/tmp/to_overwrite.pem",
                "2048",
                "RSA",
                "pass",
        };
        TerminalEmulation terminalEmulation(input);
        auto args = terminalEmulation.getArgs();
        auto argc = args.argc;
        auto argv = args.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);

        try {
            executor->execute();
        } catch(std::exception &e) {
            /*dummy*/
        }

        contentAfterOverwrite = testHelpers::readFileIntoString("/tmp/to_overwrite_prv.pem");
        bool areSame = (contentBeforeOverwrite == contentAfterOverwrite);

        BOOST_CHECK_EQUAL(areSame, true);
    }
    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}


BOOST_AUTO_TEST_CASE(OVERWRITE_FLAG_CREATE_KEY_TEST_PUB_NEG) {
    std::string contentBeforeOverwrite;
    std::string contentAfterOverwrite;
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");

    {
        std::vector<std::string> input{
                "program",
                "create-key",
                "/tmp/to_overwrite_prv.pem",
                "/tmp/to_overwrite.pem",
                "2048",
                "RSA",
                "pass",
                "overwrite",
        };
        TerminalEmulation terminalEmulation(input);
        auto args = terminalEmulation.getArgs();
        auto argc = args.argc;
        auto argv = args.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);
        executor->execute();
        contentBeforeOverwrite = testHelpers::readFileIntoString("/tmp/to_overwrite.pem");
    }
    {
        std::vector<std::string> input{
                "program",
                "create-key",
                "/tmp/to_overwrite_prv.pem",
                "/tmp/to_overwrite.pem",
                "2048",
                "RSA",
                "pass",
        };
        TerminalEmulation terminalEmulation(input);
        auto args = terminalEmulation.getArgs();
        auto argc = args.argc;
        auto argv = args.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);

        try {
            executor->execute();
        } catch(std::exception &e) {
            /*dummy*/
        }

        contentAfterOverwrite = testHelpers::readFileIntoString("/tmp/to_overwrite.pem");
        bool areSame = (contentBeforeOverwrite == contentAfterOverwrite);

        BOOST_CHECK_EQUAL(areSame, true);
    }
    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

BOOST_AUTO_TEST_CASE(OVERWRITE_FLAG_CREATE_KEY_TEST_PRV_NEG_THROW) {
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");
    {
        std::vector<std::string> input{
                "program",
                "create-key",
                "/tmp/to_overwrite_prv.pem",
                "/tmp/to_overwrite.pem",
                "2048",
                "RSA",
                "pass",
                "overwrite",
        };
        TerminalEmulation terminalEmulation(input);
        auto args = terminalEmulation.getArgs();
        auto argc = args.argc;
        auto argv = args.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);
        executor->execute();
    }
    {
        std::vector<std::string> input{
                "program",
                "create-key",
                "/tmp/to_overwrite_prv.pem",
                "/tmp/to_overwrite.pem",
                "2048",
                "RSA",
                "pass",
        };
        TerminalEmulation terminalEmulation(input);
        auto args = terminalEmulation.getArgs();
        auto argc = args.argc;
        auto argv = args.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);

        bool caught {false};
        try {
            executor->execute();
        } catch(std::exception &e) {
            caught = true;
        }

        BOOST_CHECK_EQUAL(caught, true);
    }
    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}


BOOST_AUTO_TEST_CASE(OVERWRITE_FLAG_CREATE_KEY_TEST_PUB_NEG_THROW) {
    std::string contentBeforeOverwrite;
    std::string contentAfterOverwrite;
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");
    {
        std::vector<std::string> input{
                "program",
                "create-key",
                "/tmp/to_overwrite_prv.pem",
                "/tmp/to_overwrite.pem",
                "2048",
                "RSA",
                "pass",
                "overwrite",
        };
        TerminalEmulation terminalEmulation(input);
        auto args = terminalEmulation.getArgs();
        auto argc = args.argc;
        auto argv = args.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);
        executor->execute();
        contentBeforeOverwrite = testHelpers::readFileIntoString("/tmp/to_overwrite.pem");
    }
    {
        std::vector<std::string> input{
                "program",
                "create-key",
                "/tmp/to_overwrite_prv.pem",
                "/tmp/to_overwrite.pem",
                "2048",
                "RSA",
                "pass",
        };
        TerminalEmulation terminalEmulation(input);
        auto args = terminalEmulation.getArgs();
        auto argc = args.argc;
        auto argv = args.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);

        bool caught {false};
        try {
            executor->execute();
        } catch(std::exception &e) {
            caught = true;
        }

        BOOST_CHECK_EQUAL(caught, true);
    }
    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

BOOST_AUTO_TEST_CASE(OVERWRITE_FLAG_SIGN_TEST_PUB_POS) {
    std::string afterOverwrite;
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");
    system("echo a > /tmp/result");
    {
        std::vector<std::string> input{
                "program",
                "create-key",
                "/tmp/sign_test_prv.pem",
                "/tmp/sign_test.pem",
                "2048",
                "RSA",
                "pass",
                "overwrite",
        };
        TerminalEmulation terminalEmulation(input);
        auto args = terminalEmulation.getArgs();
        auto argc = args.argc;
        auto argv = args.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);
        executor->execute();
    }
    {
        system("touch /tmp/to_check");
        std::vector<std::string> input {
                "program_sign",
                "sign",
                "/tmp/sign_test_prv.pem",
                "/tmp/to_check",
                "/tmp/result",
                "pass",
                "overwrite"
        };
        TerminalEmulation terminalEmulation(input);
        auto args = terminalEmulation.getArgs();
        auto argc = args.argc;
        auto argv = args.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);
        executor->execute();
        afterOverwrite = testHelpers::readFileIntoString("/tmp/result");
    }

    bool areSame = ("a" == afterOverwrite);
    BOOST_CHECK_EQUAL(areSame, false);
    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

BOOST_AUTO_TEST_CASE(OVERWRITE_FLAG_SIGN_TEST_PUB_NEG) {
    std::string afterOverwrite;
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");
    system("echo a > /tmp/result");
    {
        std::vector<std::string> input{
                "program",
                "create-key",
                "/tmp/sign_test_prv.pem",
                "/tmp/sign_test.pem",
                "2048",
                "RSA",
                "pass",
                "overwrite",
        };
        TerminalEmulation terminalEmulation(input);
        auto args = terminalEmulation.getArgs();
        auto argc = args.argc;
        auto argv = args.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);
        executor->execute();
    }
    {
        std::vector<std::string> input {
                "program_sign",
                "sign",
                "/tmp/sign_test_prv.pem",
                "/tmp/to_check",
                "/tmp/result",
        };
        TerminalEmulation terminalEmulation(input);
        auto args = terminalEmulation.getArgs();
        auto argc = args.argc;
        auto argv = args.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);

        try {
            executor->execute();
        } catch(std::exception &e) {
            /* dummy */
        }

        afterOverwrite = testHelpers::readFileIntoString("/tmp/result");
    }

    bool areSame = ("a" == afterOverwrite);
    BOOST_CHECK_EQUAL(areSame, true);
    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

BOOST_AUTO_TEST_CASE(OVERWRITE_FLAG_SIGN_TEST_PUB_NEG_THROW) {
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");
    system("echo a > /tmp/result");
    {
        std::vector<std::string> input{
                "program",
                "create-key",
                "/tmp/sign_test_prv.pem",
                "/tmp/sign_test.pem",
                "2048",
                "RSA",
                "pass",
                "overwrite",
        };
        TerminalEmulation terminalEmulation(input);
        auto args = terminalEmulation.getArgs();
        auto argc = args.argc;
        auto argv = args.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);
        executor->execute();
    }
    {
        std::vector<std::string> input {
                "program_sign",
                "sign",
                "/tmp/sign_test_prv.pem",
                "/tmp/to_check",
                "/tmp/result",
                "pass",
        };
        TerminalEmulation terminalEmulation(input);
        auto args = terminalEmulation.getArgs();
        auto argc = args.argc;
        auto argv = args.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);

        bool caught {false};
        try {
            executor->execute();
        } catch(std::exception &e) {
            caught = true;
        }

        BOOST_CHECK_EQUAL(caught, true);
    }
    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

BOOST_AUTO_TEST_CASE(CREATE_KEY_SAME_PATHS_THROW) {

    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");
    std::vector<std::string> input{
            "program",
            "create-key",
            "/tmp/same_path.pem",
            "/tmp/same_path.pem",
            "2048",
            "RSA",
            "pass",
            "overwrite",
    };
    TerminalEmulation terminalEmulation(input);
    auto emulatedTerminalArgs = terminalEmulation.getArgs();
    auto argc = emulatedTerminalArgs.argc;
    auto argv = emulatedTerminalArgs.argv;

    auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
    auto parser = std::make_shared<Parser>(lexicalAnalyser);
    auto executor = std::make_shared<Executor>(parser);

    bool caught {false};
    try {
        executor->execute();
    } catch (std::exception &e) {
        caught = true;
    }

    BOOST_CHECK_EQUAL(caught, true);
    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

BOOST_AUTO_TEST_CASE(CREATE_KEY_SAME_PATHS) {

    system("echo a > /tmp/same_path.pem");
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");
    std::vector<std::string> input{
            "program",
            "create-key",
            "/tmp/same_path.pem",
            "/tmp/same_path.pem",
            "2048",
            "RSA",
            "pass",
            "overwrite",
    };
    TerminalEmulation terminalEmulation(input);
    auto emulatedTerminalArgs = terminalEmulation.getArgs();
    auto argc = emulatedTerminalArgs.argc;
    auto argv = emulatedTerminalArgs.argv;

    auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
    auto parser = std::make_shared<Parser>(lexicalAnalyser);
    auto executor = std::make_shared<Executor>(parser);

    try {
        executor->execute();
    } catch (std::exception &e) {
        /* dummy */
    }

    std::string afterPotentialCreate = testHelpers::readFileIntoString("/tmp/same_path.pem");
    bool areSame = (afterPotentialCreate == "a");
    BOOST_CHECK_EQUAL(areSame, true);
    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

#if(RAW_PARTITION) 
BOOST_AUTO_TEST_CASE(RAW_PARTITION_EMULATION_TEST_WRITE_OK) {
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");

    const char* key = "abcd";
    uint64_t id = 0;
    auto ret = write_key(key, 4, "dummy", 5, &id, KEY_TYPE_CUSTOM);

    BOOST_CHECK_EQUAL(ret, 0);

    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

BOOST_AUTO_TEST_CASE(RAW_PARTITION_EMULATION_TEST_WRITE_KEY_NULL) {
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");

    const char* key = NULL;
    uint64_t id = 0;
    auto ret = write_key(key, 3, "dummy", 3, &id, KEY_TYPE_CUSTOM);

    BOOST_CHECK_EQUAL(ret, -1);

    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

BOOST_AUTO_TEST_CASE(RAW_PARTITION_EMULATION_TEST_WRITE_PWD_NULL) {
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");

    const char* key = "eqwwqeqw";
    const char* pwd = NULL;
    uint64_t id = 0;
    auto ret = write_key(key, 3, pwd, 3, &id, KEY_TYPE_CUSTOM);

    BOOST_CHECK_EQUAL(ret, -1);

    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

BOOST_AUTO_TEST_CASE(RAW_PARTITION_EMULATION_TEST_WRITE_KEY_LEN_0) {
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");

    const char* key = "eqwwqeqw";
    const char* pwd = "eqweq";
    uint64_t id = 0;
    auto ret = write_key(key, 0, pwd, 0, &id, KEY_TYPE_CUSTOM);

    BOOST_CHECK_EQUAL(ret, -1);

    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

BOOST_AUTO_TEST_CASE(RAW_PARTITION_EMULATION_TEST_WRITE_KEY_PASS_LEN_0) {
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");

    const char* key = "eqwwqeqw";
    const char* pwd = "eqweq";
    uint64_t id = 0;
    auto ret = write_key(key, 3, pwd, 0, &id, KEY_TYPE_CUSTOM);

    BOOST_CHECK_EQUAL(ret, -1);

    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

BOOST_AUTO_TEST_CASE(RAW_PARTITION_EMULATION_TEST_WRITE_KEY_TYPE_NONE) {
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");

    const char* key = "eqwwqeqw";
    const char* pwd = "eqweq";
    uint64_t id = 0;
    auto ret = write_key(key, 4, pwd, 123, &id, 123);

    BOOST_CHECK_EQUAL(ret, -1);

    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}


BOOST_AUTO_TEST_CASE(RAW_PARTITION_EMULATION_TEST_WRITE_TOO_LONG) {
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");

    const char* key = "abcd";
    uint64_t id = 0;
    auto ret = write_key(key, 100000000, "dummy", 5, &id, KEY_TYPE_CUSTOM);

    BOOST_CHECK_EQUAL(ret, 0);

    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

BOOST_AUTO_TEST_CASE(RAW_PARTITION_EMULATION_TEST_WRITE_TOO_LONG_PASS) {
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");

    const char* key = "abcd";
    uint64_t id = 0;
    auto ret = write_key(key, 4, "dummy", 100000000, &id, KEY_TYPE_CUSTOM);

    BOOST_CHECK_EQUAL(ret, 0);

    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

BOOST_AUTO_TEST_CASE(RAW_PARTITION_EMULATION_TEST_WRITE_READ) {
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");

    const char* key = "abcd";
    uint64_t id = 0;
    auto ret = write_key(key, 4, "dummy", 5, &id, KEY_TYPE_CUSTOM);

    char *buf = NULL;
    buf = (char* )malloc(5);
    auto readRet = read_key(buf, id, "dummy", 5, 4);
    BOOST_CHECK_EQUAL(key, buf);

    free(buf);

    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}
BOOST_AUTO_TEST_CASE(RAW_PARTITION_EMULATION_TEST_WRITE_READ_2) {
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");

    const char* key = "abcd";
    uint64_t id = 0;
    auto ret = write_key(key, 4, "dummy", 5, &id, KEY_TYPE_CUSTOM);

    char *buf = NULL;
    buf = (char* )malloc(5);
    auto readRet = read_key(buf, id, "dummy", 5, 3);
    BOOST_CHECK_EQUAL(buf, std::string{"abc"});

    free(buf);

    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}
BOOST_AUTO_TEST_CASE(RAW_PARTITION_EMULATION_TEST_WRITE_READ_3) {
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");

    const char* key = "abcd";
    uint64_t id = 0;
    auto ret = write_key(key, 4, "dummy", 5, &id, KEY_TYPE_CUSTOM);

    char *buf = NULL;
    buf = (char* )malloc(3);
    auto readRet = read_key(buf, id, "dummy", 5, 3);
    BOOST_CHECK_EQUAL(buf, std::string{"abc"});

    free(buf);

    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

BOOST_AUTO_TEST_CASE(RAW_PARTITION_EMULATION_TEST_WRITE_READ_NO_PASS) {
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");

    const char* key = "abcd";
    uint64_t id = 0;
    auto ret = write_key(key, 4, "dummy", 5, &id, KEY_TYPE_CUSTOM);

    char *buf = NULL;
    buf = (char* )malloc(5);
    auto readRet = read_key(buf, id, NULL, 5, 4);
    BOOST_CHECK_EQUAL(readRet, -1);

    free(buf);

    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

BOOST_AUTO_TEST_CASE(RAW_PARTITION_EMULATION_TEST_WRITE_READ_PASS_0) {
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");

    const char* key = "abcd";
    uint64_t id = 0;
    auto ret = write_key(key, 4, "dummy", 5, &id, KEY_TYPE_CUSTOM);

    char *buf = NULL;
    buf = (char* )malloc(5);
    auto readRet = read_key(buf, id, "dummy", 0, 4);
    BOOST_CHECK_EQUAL(readRet, -1);

    free(buf);

    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}


BOOST_AUTO_TEST_CASE(RAW_PARTITION_EMULATION_TEST_WRITE_READ_TOO_LONG) {
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");

    const char* key = "abcd";
    uint64_t id = 0;
    auto ret = write_key(key, 4, "dummy", 5, &id, KEY_TYPE_CUSTOM);

    char *buf = NULL;
    buf = (char* )malloc(5);
    auto readRet = read_key(buf, id, "dummy", 100000, 4);
    BOOST_CHECK_EQUAL(buf, std::string{"abcd"});

    free(buf);

    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

BOOST_AUTO_TEST_CASE(RAW_PARTITION_EMULATION_TEST_WRITE_READ_PARTIAL_PASS) {
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");

    const char* key = "abcd";
    uint64_t id = 0;
    auto ret = write_key(key, 4, "dummy_DUDUDUD", 5, &id, KEY_TYPE_CUSTOM);

    char *buf = NULL;
    buf = (char* )malloc(5);
    auto readRet = read_key(buf, id, "dummy_EDEDEDEDED", 5, 4);
    BOOST_CHECK_EQUAL(buf, std::string{"abcd"});

    free(buf);

    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

BOOST_AUTO_TEST_CASE(RAW_PARTITION_EMULATION_TEST_WRITE_READ_KEY_NULL) {
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");

    const char* key = "abcd";
    uint64_t id = 0;
    auto ret = write_key(key, 4, "dummy_DUDUDUD", 5, &id, KEY_TYPE_CUSTOM);

    char *buf = NULL;
    buf = (char* )malloc(5);
    auto readRet = read_key(NULL, id, "dummy_EDEDEDEDED", 5, 4);
    BOOST_CHECK_EQUAL(readRet, -1);

    free(buf);

    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

BOOST_AUTO_TEST_CASE(RAW_PARTITION_EMULATION_TEST_WRITE_READ_DIFF_PASS) {
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");

    const char* key = "abcd";
    uint64_t id = 0;
    auto ret = write_key(key, 4, "dummy", 5, &id, KEY_TYPE_CUSTOM);

    char *buf = NULL;
    buf = (char* )malloc(5);
    auto readRet = read_key(buf, id, "xxxxx", 5, 4);
    BOOST_CHECK_EQUAL(readRet, 0);
    BOOST_CHECK_NE(buf, std::string{"abcd"});

    free(buf);

    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

BOOST_AUTO_TEST_CASE(RAW_PARTITION_EMULATION_TEST_WRITE_READ_DIFF_PASS_LEN) {
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");

    const char* key = "abcd";
    uint64_t id = 0;
    auto ret = write_key(key, 4, "dummy", 5, &id, KEY_TYPE_CUSTOM);

    char *buf = NULL;
    buf = (char* )malloc(5);
    auto readRet = read_key(buf, id, "dummy", 3, 4);
    BOOST_CHECK_EQUAL(readRet, 0);
    BOOST_CHECK_NE(buf, std::string{"abcd"});

    free(buf);

    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

BOOST_AUTO_TEST_CASE(RAW_PARTITION_EMULATION_TEST_READ_CUT) {
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");

    const char* key = "abcd";
    uint64_t id = 0;
    auto ret = write_key(key, 4, "dummy", 4, &id, KEY_TYPE_CUSTOM);

    char *buf = NULL;
    buf = (char* )malloc(3);
    auto readRet = read_key(buf, id, "dummy", 4, 3);
    BOOST_CHECK_EQUAL(buf, std::string{"abc"});

    free(buf);

    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}
BOOST_AUTO_TEST_CASE(RAW_PARTITION_EMULATION_TEST_READ_0_ID) {
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");

    const char* key = "abcd";
    uint64_t id = 0;
    auto ret = write_key(key, 4, "dummy", 5, &id, KEY_TYPE_CUSTOM);

    char *buf = NULL;
    buf = (char* )malloc(3);
    auto readRet = read_key(buf, 0, "dummy", 5, 3);
    BOOST_CHECK_EQUAL(readRet, -10);

    free(buf);

    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}
BOOST_AUTO_TEST_CASE(RAW_PARTITION_EMULATION_TEST_GET_SIZE_0_ID) {
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");

    const char* key = "abcd";
    uint64_t id = 0;
    auto ret = write_key(key, 4, "dummy", 5, &id, KEY_TYPE_CUSTOM);

    uint64_t keySize = 0;
    auto readRet = get_key_size(0, &keySize);
    BOOST_CHECK_EQUAL(readRet, -10);

    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}
BOOST_AUTO_TEST_CASE(RAW_PARTITION_EMULATION_TEST_REMOVE_0_ID) {
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");

    const char* key = "abcd";
    uint64_t id = 0;
    auto ret = write_key(key, 4, "dummy", 5, &id, KEY_TYPE_CUSTOM);

    auto removeRet = remove_key(0);
    BOOST_CHECK_EQUAL(removeRet, -10);

    uint64_t keyNum = 0;
    auto getKeyNumRet = get_key_num(&keyNum);
    BOOST_CHECK_EQUAL(removeRet, -10);
    BOOST_CHECK_EQUAL(keyNum, 1);

    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

BOOST_AUTO_TEST_CASE(RAW_PARTITION_EMULATION_TEST_GET_MODE_0_ID) {
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");

    const char* key = "abcd";
    uint64_t id = 0;
    auto ret = write_key(key, 4, "dummy", 5, &id, KEY_TYPE_CUSTOM);

    int mode = 123123123;
    int inititalMode = mode;
    auto getModesRet = get_mode(0, &mode);
    BOOST_CHECK_EQUAL(getModesRet, -10);
    BOOST_CHECK_EQUAL(mode, inititalMode);

    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}
BOOST_AUTO_TEST_CASE(RAW_PARTITION_EMULATION_TEST_SET_MODE_0_ID) {
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");

    const char* key = "abcd";
    uint64_t id = 0;
    auto ret = write_key(key, 4, "dummy", 5, &id, KEY_TYPE_CUSTOM);

    int mode = 660;
    auto setModeRet = set_mode(0, mode);
    BOOST_CHECK_EQUAL(setModeRet, -10);

    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

BOOST_AUTO_TEST_CASE(RAW_PARTITION_EMULATION_TEST_READ_NO_ID) {
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");

    const char* key = "abcd";
    uint64_t id = 0;
    auto ret = write_key(key, 4, "dummy", 5, &id, KEY_TYPE_CUSTOM);

    char *buf = NULL;
    buf = (char* )malloc(3);
    auto readRet = read_key(buf, 1000, "dummy", 5, 3);
    BOOST_CHECK_EQUAL(readRet, -10);

    free(buf);

    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}
BOOST_AUTO_TEST_CASE(RAW_PARTITION_EMULATION_GET_MODE) {
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");

    const char* key = "abcd";
    uint64_t id = 0;
    auto ret = write_key(key, 4, "dummy", 5, &id, KEY_TYPE_CUSTOM);

    int mode = 0;
    auto readRet = get_mode(id, &mode);
    BOOST_CHECK_EQUAL(mode, 600);

    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

BOOST_AUTO_TEST_CASE(RAW_PARTITION_EMULATION_GET_MODE_NO_PART) {
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");

    int mode = 0;
    auto readRet = get_mode(1000, &mode);
    BOOST_CHECK_EQUAL(readRet, -10);

    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

BOOST_AUTO_TEST_CASE(RAW_PARTITION_EMULATION_GET_MODE_WRONG_ID) {
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");

    const char* key = "abcd";
    uint64_t id = 0;
    auto ret = write_key(key, 4, "dummy", 5, &id, KEY_TYPE_CUSTOM);

    int mode = 0;
    auto readRet = get_mode(1000, &mode);
    BOOST_CHECK_EQUAL(readRet, -10);

    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

BOOST_AUTO_TEST_CASE(RAW_PARTITION_EMULATION_SET_MODE_NO_ID) {
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");

    const char* key = "abcd";
    uint64_t id = 0;
    auto ret = write_key(key, 4, "dummy", 5, &id, KEY_TYPE_CUSTOM);

    int mode = 600;
    auto readRet = set_mode(1000, mode);
    BOOST_CHECK_EQUAL(readRet, -10);

    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

BOOST_AUTO_TEST_CASE(RAW_PARTITION_EMULATION_SET_MODE_NO_PARTITION) {
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");

    int mode = 660;
    auto readRet = set_mode(1000, mode);
    BOOST_CHECK_EQUAL(readRet, -10);

    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

BOOST_AUTO_TEST_CASE(RAW_PARTITION_EMULATION_SET_MODE_GET) {
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");

    const char* key = "abcd";
    uint64_t id = 0;
    auto ret = write_key(key, 4, "dummy", 5, &id, KEY_TYPE_CUSTOM);

    int mode = 600;
    auto setRet = set_mode(id, mode);

    int newMode = 0;
    auto getRet = get_mode(id, &newMode);

    BOOST_CHECK_EQUAL(setRet, 0);
    BOOST_CHECK_EQUAL(getRet, 0);
    BOOST_CHECK_EQUAL(mode, newMode);

    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

BOOST_AUTO_TEST_CASE(RAW_PARTITION_EMULATION_SET_GET_MODE_MULT) {
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");

    auto isProperMode = [](int mode) {
        if(mode > 777 || mode < 600) {
            return false;
        }
        auto hDigit = (mode / 100);
        auto dDigit = (mode / 10) % 10; 
        auto digit = mode % 10;

        return hDigit <= 7 && dDigit <= 7 && digit <= 7;
    };

    const char* key = "abcd";
    uint64_t id = 0;
    auto ret = write_key(key, 4, "dummy", 5, &id, KEY_TYPE_CUSTOM);

    int lastProperMode;
    auto getRet = get_mode(id, &lastProperMode);

    for(int i = 600; i < 1000; i++) {

        auto setRet = set_mode(id, i);

        int newMode = 0;
        auto getRet = get_mode(id, &newMode);

        if(isProperMode(i)) {
            BOOST_CHECK_EQUAL(setRet, 0);
            BOOST_CHECK_EQUAL(getRet, 0);
            BOOST_CHECK_EQUAL(i, newMode);
            lastProperMode = i;
        }
    }   
    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

BOOST_AUTO_TEST_CASE(RAW_PARTITION_EMULATION_KEY_NUM) {
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");

    const char* key = "abc777777d";
    uint64_t id = 0;
    uint64_t keyNum = 0;
    auto ret = write_key(key, 10, "dummy", 5, &id, KEY_TYPE_CUSTOM);
    auto keyNumRet = get_key_num(&keyNum);

    BOOST_CHECK_EQUAL(ret, 0);
    BOOST_CHECK_EQUAL(keyNumRet, 0);
    BOOST_CHECK_EQUAL(keyNum, 1);

    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

BOOST_AUTO_TEST_CASE(RAW_PARTITION_EMULATION_KEY_NUM_MAGIC) {
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");
    system("echo eqwewqewqeqwewqeqwewqeqwewqeqweqwqweqweqweqweqweqweqw >> ~/.keyPartitionV2/meta");

    const char* key = "abc777777d";
    uint64_t id = 0;
    uint64_t keyNum = 0;
    auto ret = write_key(key, 10, "dummy", 5, &id, KEY_TYPE_CUSTOM);
    auto keyNumRet = get_key_num(&keyNum);

    BOOST_CHECK_EQUAL(ret, 0);
    BOOST_CHECK_EQUAL(keyNumRet, 0);
    BOOST_CHECK_EQUAL(keyNum, 1);

    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

BOOST_AUTO_TEST_CASE(RAW_PARTITION_EMULATION_KEY_NUM_MAGIC_NO_PART) {
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");
    system("echo eqwewqewqeqwewqeqwewqeqwewqeqweqwqweqweqweqweqweqweqw >> ~/.keyPartitionV2/meta");

    uint64_t keyNum = 0;
    auto keyNumRet = get_key_num(&keyNum);

    BOOST_CHECK_EQUAL(keyNum, 0);
    BOOST_CHECK_EQUAL(keyNumRet, 0);

    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

BOOST_AUTO_TEST_CASE(RAW_PARTITION_EMULATION_KEY_SIZE) {
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");

    const char* key = "abc777777d";
    uint64_t id = 0;
    auto ret = write_key(key, 10, "dummy", 5, &id, KEY_TYPE_CUSTOM);
    BOOST_CHECK_EQUAL(ret, 0);

    uint64_t size = 0;
    auto getSizeRet = get_key_size(id, &size);
    BOOST_CHECK_EQUAL(getSizeRet, 0);

    BOOST_CHECK_EQUAL(size, 10);

    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

BOOST_AUTO_TEST_CASE(RAW_PARTITION_EMULATION_KEY_SIZE_NO_PARTITION) {
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");

    uint64_t size = 0;
    auto getSizeRet = get_key_size(1000, &size);
    BOOST_CHECK_EQUAL(getSizeRet, -10);

    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

BOOST_AUTO_TEST_CASE(RAW_PARTITION_EMULATION_KEY_SIZE_WRONG_ID) {
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");

    const char* key = "abc777777d";
    uint64_t id = 0;
    auto ret = write_key(key, 10, "dummy", 5, &id, KEY_TYPE_CUSTOM);
    BOOST_CHECK_EQUAL(ret, 0);

    uint64_t size = 0;
    auto getSizeRet = get_key_size(1000, &size);
    BOOST_CHECK_EQUAL(getSizeRet, -10);

    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

BOOST_AUTO_TEST_CASE(RAW_PARTITION_EMULATION_KEY_NUM_NO_PART) {
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");

    uint64_t keyNum = 0;
    auto keyNumRet = get_key_num(&keyNum);
    BOOST_CHECK_EQUAL(keyNumRet, 0);
    BOOST_CHECK_EQUAL(keyNum, 0);

    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

BOOST_AUTO_TEST_CASE(RAW_PARTITION_EMULATION_KEY_NUM_MULT) {
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");

    for(int i = 1; i < 100; i ++) {
        const char* key = "abc777777d";
        uint64_t id = 0;
        auto ret = write_key(key, 10, "dummy", 5, &id, KEY_TYPE_CUSTOM);

        uint64_t keyNum =0;
        auto keyNumRet = get_key_num(&keyNum);

        BOOST_CHECK_EQUAL(ret, 0);
        BOOST_CHECK_EQUAL(keyNumRet, 0);
        BOOST_CHECK_EQUAL(keyNum, i);
        
    }

    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

BOOST_AUTO_TEST_CASE(RAW_PARTITION_EMULATION_TEST_WRITE_RMV_READ) {
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");

    const char* key = "abcd";
    uint64_t id = 0;
    auto ret = write_key(key, 10, "dummy", 5, &id, KEY_TYPE_CUSTOM);

    BOOST_CHECK_EQUAL(ret, 0);

    auto rmvRet = remove_key(id);

    char *buf = NULL;
    buf = (char* )malloc(5);
    auto readRet = read_key(buf, id, "dummy", 5, 4);

    BOOST_CHECK_EQUAL(ret, 0);
    BOOST_CHECK_EQUAL(rmvRet, 0);
    BOOST_CHECK_EQUAL(readRet, -10);

    free(buf);

    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

BOOST_AUTO_TEST_CASE(RAW_PARTITION_EMULATION_TEST_WRITE_RMV_GET_KEY_SIZE) {
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");

    const char* key = "abcd";
    uint64_t id = 0;
    auto ret = write_key(key, 10, "dummy", 5, &id, KEY_TYPE_CUSTOM);

    auto rmvRet = remove_key(id);

    uint64_t size = 1000000;
    auto initialSize = size;
    auto getKeySizeRet = get_key_size(id, &size);

    BOOST_CHECK_EQUAL(ret, 0);
    BOOST_CHECK_EQUAL(rmvRet, 0);
    BOOST_CHECK_EQUAL(getKeySizeRet, -10);
    BOOST_CHECK_EQUAL(size, initialSize);

    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

BOOST_AUTO_TEST_CASE(RAW_PARTITION_EMULATION_TEST_WRITE_RMV_GET_MODE) {
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");

    const char* key = "abcd";
    uint64_t id = 0;
    auto ret = write_key(key, 10, "dummy", 5, &id, KEY_TYPE_CUSTOM);

    auto rmvRet = remove_key(id);
    
    int modes = 10000;
    auto initialModes = modes;
    auto getModesRet = get_mode(id, &modes);

    BOOST_CHECK_EQUAL(ret, 0);
    BOOST_CHECK_EQUAL(rmvRet, 0);
    BOOST_CHECK_EQUAL(getModesRet, -10);
    BOOST_CHECK_EQUAL(modes, initialModes);

    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

BOOST_AUTO_TEST_CASE(RAW_PARTITION_EMULATION_TEST_WRITE_RMV_SET_MODE) {
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");

    const char* key = "abcd";
    uint64_t id = 0;
    auto ret = write_key(key, 10, "dummy", 5, &id, KEY_TYPE_CUSTOM);

    auto rmvRet = remove_key(id);
    
    int modes = 660;
    auto setModesRet = set_mode(id, modes);

    BOOST_CHECK_EQUAL(ret, 0);
    BOOST_CHECK_EQUAL(rmvRet, 0);
    BOOST_CHECK_EQUAL(setModesRet, -10);

    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

BOOST_AUTO_TEST_CASE(RAW_PARTITION_EMULATION_TEST_OPTIMISED_KEY_STORAGE_HEAVY_LOAD) {
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");

    int readRet;
    int getSizeRet;
    int getKeyNumRet;
    uint64_t size = 0;
    uint64_t num;

    for(int i = 0; i < 20; i++) {

        std::cout << i << std::endl;
        // create 3 keys
        const char* key = "xxxxxxxxxx";
        uint64_t id = 0;
        auto ret1 = write_key(key, 10, "dummy", 5, &id, KEY_TYPE_CUSTOM);
        BOOST_CHECK_EQUAL(ret1, 0);

        const char* key2 = "ooooooooooo";
        uint64_t id2 = 0;
        auto ret2 = write_key(key2, 11, "dummy", 5, &id2, KEY_TYPE_CUSTOM);
        BOOST_CHECK_EQUAL(ret2, 0);

        const char* key3 = "cccccccccccc";
        uint64_t id3 = 0;
        auto ret3 = write_key(key3, 12, "dummy", 5, &id3, KEY_TYPE_CUSTOM);
        BOOST_CHECK_EQUAL(ret3, 0);

        // remove middles
        auto rmvRet = remove_key(id2);
        BOOST_CHECK_EQUAL(rmvRet, 0);

        getKeyNumRet = get_key_num(&num);
        BOOST_CHECK_EQUAL(getKeyNumRet, 0);
        BOOST_CHECK_EQUAL(num, 2);

        // algorithm will add 3rd key between first and last
        const char* key4 = "tttttttttt";
        uint64_t id4;
        auto ret4 = write_key(key4, 10, "dummy", 5, &id4, KEY_TYPE_CUSTOM);
        BOOST_CHECK_EQUAL(ret4, 0);

        getKeyNumRet = get_key_num(&num);
        BOOST_CHECK_EQUAL(getKeyNumRet, 0);
        BOOST_CHECK_EQUAL(num, 3);
      
        // reading 3 keys by get size
        getSizeRet = get_key_size(id, &size);
        BOOST_CHECK_EQUAL(getSizeRet, 0);
        char* buf = (char* )malloc(size + 1);
        readRet = read_key(buf, id, "dummy", 5, size);
        BOOST_CHECK_EQUAL(readRet, 0);
        BOOST_CHECK_EQUAL(buf, key);
        free(buf);

        getSizeRet = get_key_size(id3, &size);
        BOOST_CHECK_EQUAL(getSizeRet, 0);
        char* buf3 = (char* )malloc(size + 1);
        readRet = read_key(buf3, id3, "dummy", 5, size);
        BOOST_CHECK_EQUAL(readRet, 0);
        BOOST_CHECK_EQUAL(buf3, key3);
        free(buf3);

        getSizeRet = get_key_size(id4, &size);
        BOOST_CHECK_EQUAL(getSizeRet, 0);
        char* buf4 = (char* )malloc(size + 1);
        readRet = read_key(buf4, id4, "dummy", 5, size);
        BOOST_CHECK_EQUAL(readRet, 0);
        BOOST_CHECK_EQUAL(buf4, key4);
        free(buf4);

        auto rmvRet1 = remove_key(id);
        auto rmvRet2 = remove_key(id3);
        auto rmvRet3 = remove_key(id4);

        BOOST_CHECK_EQUAL(rmvRet1, 0);
        BOOST_CHECK_EQUAL(rmvRet2, 0);
        BOOST_CHECK_EQUAL(rmvRet3, 0);

        getKeyNumRet = get_key_num(&num);
        BOOST_CHECK_EQUAL(getKeyNumRet, 0);
        BOOST_CHECK_EQUAL(num, 0);
    }

    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

BOOST_AUTO_TEST_CASE(RAW_PARTITION_EMULATION_TEST_OPTIMISED_KEY_STORAGE_HEAVY_LOAD_NEVER_EMPTY) {
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");

    int readRet;
    int getSizeRet;
    int getKeyNumRet;
    uint64_t size = 0;
    uint64_t num;

    const char* key_init = "initxxxxxxxxxxxxxxxxxxxxxxxkey";
    uint64_t id_init;
    auto ret_init = write_key(key_init, 30, "dummy", 5, &id_init, KEY_TYPE_CUSTOM);
    BOOST_CHECK_EQUAL(ret_init, 0);

    for(int i = 0; i < 20; i++) {

        // create 3 keys
        const char* key = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
        uint64_t id = 0;
        auto ret1 = write_key(key, 30, "dummy", 5, &id, KEY_TYPE_CUSTOM);
        BOOST_CHECK_EQUAL(ret1, 0);

        const char* key2 = "ooooooooooooooooooooooooooooooooo";
        uint64_t id2;
        auto ret2 = write_key(key2, 33, "dummy", 5, &id2, KEY_TYPE_CUSTOM);
        BOOST_CHECK_EQUAL(ret2, 0);

        const char* key3 = "cccccccccccccccccccccccccccccccccccc";
        uint64_t id3;
        auto ret3 = write_key(key3, 36, "dummy", 5, &id3, KEY_TYPE_CUSTOM);
        BOOST_CHECK_EQUAL(ret3, 0);

        // remove middles
        auto rmvRet = remove_key(id2);
        BOOST_CHECK_EQUAL(rmvRet, 0);

        getKeyNumRet = get_key_num(&num);
        BOOST_CHECK_EQUAL(num, 3);

        // algorithm will add 3rd key between first and last
        const char* key4 = "tttttttttttttttttttttttttttttt";
        uint64_t id4;
        auto ret4 = write_key(key4, 30, "dummy", 5, &id4, KEY_TYPE_CUSTOM);
        BOOST_CHECK_EQUAL(ret4, 0);

        getKeyNumRet = get_key_num(&num);
        BOOST_CHECK_EQUAL(num, 4);
      
        // reading 3 keys by get size
        getSizeRet = get_key_size(id, &size);
        BOOST_CHECK_EQUAL(getSizeRet, 0);
        char* buf = (char* )malloc(size + 1);
        readRet = read_key(buf, id, "dummy", 5, size);
        BOOST_CHECK_EQUAL(readRet, 0);
        BOOST_CHECK_EQUAL(buf, key);
        free(buf);

        getSizeRet = get_key_size(id3, &size);
        BOOST_CHECK_EQUAL(getSizeRet, 0);
        char* buf3 = (char* )malloc(size + 1);
        readRet = read_key(buf3, id3, "dummy", 5, size);
        BOOST_CHECK_EQUAL(readRet, 0);
        BOOST_CHECK_EQUAL(buf3, key3);
        free(buf3);

        getSizeRet = get_key_size(id4, &size);
        BOOST_CHECK_EQUAL(getSizeRet, 0);
        char* buf4 = (char* )malloc(size + 1);
        readRet = read_key(buf4, id4, "dummy", 5, size);
        BOOST_CHECK_EQUAL(readRet, 0);
        BOOST_CHECK_EQUAL(buf4, key4);
        free(buf4);

        auto rmvRet1 = remove_key(id);
        auto rmvRet2 = remove_key(id3);
        auto rmvRet3 = remove_key(id4);

        BOOST_CHECK_EQUAL(rmvRet1, 0);
        BOOST_CHECK_EQUAL(rmvRet2, 0);
        BOOST_CHECK_EQUAL(rmvRet3, 0);

        getKeyNumRet = get_key_num(&num);
        BOOST_CHECK_EQUAL(num, 1);
    }

    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

#endif

#if(LONG_RUN)
BOOST_AUTO_TEST_CASE(RAW_PARTITION_EMULATION_TEST_OPTIMISED_KEY_STORAGE_HEAVY_LOAD_L) {
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");

    int readRet;
    int getSizeRet;
    int getKeyNum;
    uint64_t size = 0;
    uint64_t num;

    for(int i = 0; i < 200; i++) {

        // create 3 keys
        const char* key = "xxxxxxxxxx";
        uint64_t id = 0;
        auto ret1 = write_key(key, 10, "dummy", 5, &id, KEY_TYPE_CUSTOM);
        BOOST_CHECK_EQUAL(ret1, 0);

        const char* key2 = "ooooooooooo";
        uint64_t id2;
        auto ret2 = write_key(key2, 11, "dummy", 5, &id2, KEY_TYPE_CUSTOM);
        BOOST_CHECK_EQUAL(ret2, 0);

        const char* key3 = "cccccccccccc";
        uint64_t id3;
        auto ret3 = write_key(key3, 12, "dummy", 5, &id3, KEY_TYPE_CUSTOM);
        BOOST_CHECK_EQUAL(ret3, 0);

        // remove middles
        auto rmvRet = remove_key(id2);
        BOOST_CHECK_EQUAL(rmvRet, 0);

        getKeyNum = get_key_num(&num);
        BOOST_CHECK_EQUAL(num, 2);

        // algorithm will add 3rd key between first and last
        const char* key4 = "tttttttttt";
        uint64_t id4;
        auto ret4 = write_key(key4, 10, "dummy", 5, &id4, KEY_TYPE_CUSTOM);
        BOOST_CHECK_EQUAL(ret4, 0);

        getKeyNum = get_key_num(&num);
        BOOST_CHECK_EQUAL(num, 3);
      
        // reading 3 keys by get size
        getSizeRet = get_key_size(id, &size);
        BOOST_CHECK_EQUAL(getSizeRet, 0);
        char* buf = (char* )malloc(size + 1);
        readRet = read_key(buf, id, "dummy", 5, size);
        BOOST_CHECK_EQUAL(readRet, 0);
        BOOST_CHECK_EQUAL(buf, key);
        free(buf);

        getSizeRet = get_key_size(id3, &size);
        BOOST_CHECK_EQUAL(getSizeRet, 0);
        char* buf3 = (char* )malloc(size + 1);
        readRet = read_key(buf3, id3, "dummy", 5, size);
        BOOST_CHECK_EQUAL(readRet, 0);
        BOOST_CHECK_EQUAL(buf3, key3);
        free(buf3);

        getSizeRet = get_key_size(id4, &size);
        BOOST_CHECK_EQUAL(getSizeRet, 0);
        char* buf4 = (char* )malloc(size + 1);
        readRet = read_key(buf4, id4, "dummy", 5, size);
        BOOST_CHECK_EQUAL(readRet, 0);
        BOOST_CHECK_EQUAL(buf4, key4);
        free(buf4);

        auto rmvRet1 = remove_key(id);
        auto rmvRet2 = remove_key(id3);
        auto rmvRet3 = remove_key(id4);

        BOOST_CHECK_EQUAL(rmvRet1, 0);
        BOOST_CHECK_EQUAL(rmvRet2, 0);
        BOOST_CHECK_EQUAL(rmvRet3, 0);

        getKeyNum = get_key_num(&num);
        BOOST_CHECK_EQUAL(num, 0);
    }

    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

BOOST_AUTO_TEST_CASE(RAW_PARTITION_EMULATION_TEST_OPTIMISED_KEY_STORAGE_HEAVY_LOAD_NEVER_EMPTY_L) {
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");

    int readRet;
    int getSizeRet;
    int getKeyNumRet;
    uint64_t size = 0;
    uint64_t num;

    const char* key_init = "initxxxxxxxxxxxxxxxxxxxxxxxkey";
    uint64_t id_init;
    auto ret_init = write_key(key_init, 30, "dummy", 5, &id_init, KEY_TYPE_CUSTOM);
    BOOST_CHECK_EQUAL(ret_init, 0);

    for(int i = 0; i < 200; i++) {

        // create 3 keys
        const char* key = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
        uint64_t id = 0;
        auto ret1 = write_key(key, 30, "dummy", 5, &id, KEY_TYPE_CUSTOM);
        BOOST_CHECK_EQUAL(ret1, 0);

        const char* key2 = "ooooooooooooooooooooooooooooooooo";
        uint64_t id2;
        auto ret2 = write_key(key2, 33, "dummy", 5, &id2, KEY_TYPE_CUSTOM);
        BOOST_CHECK_EQUAL(ret2, 0);

        const char* key3 = "cccccccccccccccccccccccccccccccccccc";
        uint64_t id3;
        auto ret3 = write_key(key3, 36, "dummy", 5, &id3, KEY_TYPE_CUSTOM);
        BOOST_CHECK_EQUAL(ret3, 0);

        // remove middles
        auto rmvRet = remove_key(id2);
        BOOST_CHECK_EQUAL(rmvRet, 0);

        getKeyNumRet = get_key_num(&num);
        BOOST_CHECK_EQUAL(num, 3);

        // algorithm will add 3rd key between first and last
        const char* key4 = "tttttttttttttttttttttttttttttt";
        uint64_t id4;
        auto ret4 = write_key(key4, 30, "dummy", 5, &id4, KEY_TYPE_CUSTOM);
        BOOST_CHECK_EQUAL(ret4, 0);
        getKeyNumRet = get_key_num(&num);
        BOOST_CHECK_EQUAL(num, 4);
      
        // reading 3 keys by get size
        getSizeRet = get_key_size(id, &size);
        BOOST_CHECK_EQUAL(getSizeRet, 0);
        char* buf = (char* )malloc(size + 1);
        readRet = read_key(buf, id, "dummy", 5, size);
        BOOST_CHECK_EQUAL(readRet, 0);
        BOOST_CHECK_EQUAL(buf, key);
        free(buf);

        getSizeRet = get_key_size(id3, &size);
        BOOST_CHECK_EQUAL(getSizeRet, 0);
        char* buf3 = (char* )malloc(size + 1);
        readRet = read_key(buf3, id3, "dummy", 5, size);
        BOOST_CHECK_EQUAL(readRet, 0);
        BOOST_CHECK_EQUAL(buf3, key3);
        free(buf3);

        getSizeRet = get_key_size(id4, &size);
        BOOST_CHECK_EQUAL(getSizeRet, 0);
        char* buf4 = (char* )malloc(size + 1);
        readRet = read_key(buf4, id4, "dummy", 5, size);
        BOOST_CHECK_EQUAL(readRet, 0);
        BOOST_CHECK_EQUAL(buf4, key4);
        free(buf4);

        auto rmvRet1 = remove_key(id);
        auto rmvRet2 = remove_key(id3);
        auto rmvRet3 = remove_key(id4);

        BOOST_CHECK_EQUAL(rmvRet1, 0);
        BOOST_CHECK_EQUAL(rmvRet2, 0);
        BOOST_CHECK_EQUAL(rmvRet3, 0);

        getKeyNumRet = get_key_num(&num);
        BOOST_CHECK_EQUAL(num, 1);
    }

    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}
BOOST_AUTO_TEST_CASE(ENCRYPT_DECRYPT_CONTENT_CHECK_HUGE)
{

    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");
    std::string base = "1234567890";
    std::string content;

    for(int i = 0; i < 1000; i++) {
        content+=base;
    }

    {
        std::string syscall = "echo " + content + " > /tmp/file.txt";
        system(syscall.c_str());
        system("echo 3123213213 > /tmp/file_to_encrypt.txt");
        system("echo 0123456789abcdeF0123456789abcdeF > /tmp/key");
        system("echo 1234567887654321 > /tmp/iv");

        std::vector<std::string> input {
                "program",
                "encrypt-file",
                "/tmp/key",
                "/tmp/iv",
                "/tmp/file.txt",
                "/tmp/encrypted_file.txt",
                "overwrite"
        };

        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);
        executor->execute();
    }
    {
        std::string contentAfterEncryption = testHelpers::readFileIntoString("/tmp/encrypted_file.txt");
        std::vector<std::string> input {
                "program",
                "decrypt-file",
                "/tmp/key",
                "/tmp/iv",
                "/tmp/encrypted_file.txt",
                "/tmp/decrypted_file.txt",
                "overwrite"
        };

        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);
        auto msg = executor->execute();

        std::string contentAfterEncryption2 = testHelpers::readFileIntoString("/tmp/encrypted_file.txt");
        std::string contentAfterDecryption = testHelpers::readFileIntoString("/tmp/decrypted_file.txt");
        bool areSame = (contentAfterDecryption == content);
        BOOST_CHECK_EQUAL(areSame, true);
    }
    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}
BOOST_AUTO_TEST_CASE(ENCRYPT_DECRYPT_HUGE)
{
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");
    {
        system("echo 3123213213 > /tmp/file_to_encrypt.txt");
        for(int i = 0; i < 1000; i++) {
            system("echo 3123213213 >> /tmp/file_to_encrypt.txt");
        }
        system("echo 0123456789abcdeF0123456789abcdeF > /tmp/key");
        system("echo 1234567887654321 > /tmp/iv");

        std::vector<std::string> input {
                "program",
                "encrypt-file",
                "/tmp/key",
                "/tmp/iv",
                "/tmp/file.txt",
                "/tmp/encrypted_file.txt",
                "overwrite"
        };

        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);
        executor->execute();
    }
    {
        std::vector<std::string> input {
                "program",
                "decrypt-file",
                "/tmp/key",
                "/tmp/iv",
                "/tmp/encrypted_file.txt",
                "/tmp/decrypted_file.txt",
                "overwrite"
        };

        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);
        auto msg = executor->execute();

        BOOST_CHECK_EQUAL(msg, "File decrypted");
    }
    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}



BOOST_AUTO_TEST_CASE(ENCRYPT_DECRYPT_WRONG_KEY)
{
    std::string content = "ewqewqeuywqoeyowqie8735614370856430857n14c78f18h8yxdhdanns7d78ae64bc07845ncg4w8X7MR0fd9830w";
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");
    {
        std::string syscall = "echo " + content + " > /tmp/file.txt";
        system(syscall.c_str());
        system("echo 3123213213 > /tmp/file_to_encrypt.txt");
        system("echo 0123456789abcdeF > /tmp/key");
        system("echo 1234567887654321 > /tmp/iv");

        std::vector<std::string> input {
                "program",
                "encrypt-file",
                "/tmp/key",
                "/tmp/iv",
                "/tmp/file.txt",
                "/tmp/encrypted_file.txt",
                "overwrite"
        };

        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);
        executor->execute();
    }
    {
        system("echo 123 > /tmp/key");
        std::vector<std::string> input {
                "program",
                "decrypt-file",
                "/tmp/key",
                "/tmp/iv",
                "/tmp/encrypted_file.txt",
                "/tmp/decrypted_file.txt",
                "overwrite"
        };

        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);

        std::string contentAfterDecryption = testHelpers::readFileIntoString("/tmp/decrypted_file.txt");
        bool areSame = (contentAfterDecryption == content);
        BOOST_CHECK_EQUAL(areSame, false);
    }
    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}
BOOST_AUTO_TEST_CASE(POSITIVE_TEST_ENCRYPT_FILE_HUGE)
{
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");

    system("echo 3123213213 > /tmp/file_to_encrypt.txt");

    for(int i = 0; i < 1000; i++) {
        system("echo 3123213213 >> /tmp/file_to_encrypt.txt");
    }
    system("echo 0123456789abcdeF0123456789abcdeF > /tmp/key");
    system("echo 1234567887654321 > /tmp/iv");
    {
        std::vector<std::string> input {
                "program",
                "encrypt-file",
                "/tmp/key",
                "/tmp/iv",
                "/tmp/file_to_encrypt.txt",
                "/tmp/encrypted_file.txt",
                "overwrite"
        };

        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);
        auto msg = executor->execute();

        BOOST_CHECK_EQUAL(msg, "File encrypted");
    }
    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

BOOST_AUTO_TEST_CASE(POSITIVE_TEST_DECRYPT_FILE)
{
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");
    system("echo 3123213213 > /tmp/file_to_decrypt.txt");
    system("echo 0123456789abcdeF0123456789abcdeF > /tmp/key");
    system("echo 1234567887654321 > /tmp/iv");

    {
        std::vector<std::string> input {
                "program",
                "decrypt-file",
                "/tmp/key",
                "/tmp/iv",
                "/tmp/file_to_decrypt.txt",
                "/tmp/decrypted_file.txt",
                "overwrite"
        };

        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);

        bool caught {false};
        try {
            executor->execute();
        } catch(std::exception &e) {
            if(e.what() == std::string("OpenSSLHandler: cannot encrypt last block of file")) {
                caught = true;
            }
        }

        BOOST_CHECK_EQUAL(caught, true);
    }
    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}
BOOST_AUTO_TEST_CASE(CHECK_SIGNATURE_INTEGRATION_TEST_12) {
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");

    for (int i = 0; i < 128; i++)
    {
        auto str = std::to_string(i);
        std::vector<std::string> input {
                "program",
                "create-key",
                "/tmp/private" + str + ".pem",
                "/tmp/public" + str + ".pem",
                "2048",
                "RSA",
                "pass",
                "overwrite"
        };
        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);
        executor->execute();
        auto keysInPartition = executor->getCurrentInterface()->getCurrentKeyNum();
    }

    int keysInPartitionAfterAdd = 128;
    for(int i = 0; i < 128; i++)
    {
        keysInPartitionAfterAdd--;

        auto str = std::to_string(i);
        std::vector<std::string> input {
                "program",
                "delete-key",
                "/tmp/private" + str + ".pem",
                "/tmp/public" + str + ".pem",
        };
        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);

        executor->execute();
        auto keysInPartition = executor->getCurrentInterface()->getCurrentKeyNum();
        BOOST_CHECK_EQUAL(keysInPartitionAfterAdd, keysInPartition);
    }

    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

BOOST_AUTO_TEST_CASE(CHECK_SIGNATURE_INTEGRATION_TEST_13) {
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");

    for (int i = 0; i < 128; i++)
    {
        auto str = std::to_string(i);
//        std::cout << i << std::endl;
        std::vector<std::string> input {
                "program",
                "create-key",
                "/tmp/private" + str + ".pem",
                "/tmp/public" + str + ".pem",
                "2048",
                "RSA",
                "pass",
                "overwrite"
        };
        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);
        executor->execute();
        auto keysInPartition = executor->getCurrentInterface()->getCurrentKeyNum();
//        std::cout << "There are: " << keysInPartition << " keys in partition " << std::endl;
    }

    for(int i = 0; i < 1; i++)
    {
//        std::cout << "delete\n";
        std::vector<std::string> input {
                "program",
                "delete-key",
                "/tmp/private0.pem",
                "/tmp/public0.pem",
        };
        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);
        executor->execute();
        auto keysInPartition = executor->getCurrentInterface()->getCurrentKeyNum();

        BOOST_CHECK_EQUAL(127, keysInPartition);
    }
    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

BOOST_AUTO_TEST_CASE(TOO_MANY_KEYS_IN_PARTITION) {
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");

    for (int i = 0; i < 128; i++)
    {
        auto str = std::to_string(i);
//        std::cout << i << std::endl;
        std::vector<std::string> input {
                "program",
                "create-key",
                "/tmp/private" + str + ".pem",
                "/tmp/public" + str + ".pem",
                "2048",
                "RSA",
                "pass",
                "overwrite"
        };
        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);
        executor->execute();
    }
    {
        std::vector<std::string> input {
                "program",
                "create-key",
                "/tmp/private128.pem",
                "/tmp/public128.pem",
                "2048",
                "RSA",
                "pass",
                "overwrite"
        };
        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);

        bool caught {false};
        try {
            executor->execute();
        } catch(std::exception &e) {
            if(e.what() == std::string("KeyIOInterface: Partition full")) {
                caught = true;
            }
        }
        BOOST_CHECK_EQUAL(caught, true);
    }
    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

BOOST_AUTO_TEST_CASE(PARTITION_DEFRAGMENTATION_CREATE_DELETE_GET) {
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");

    for (int i = 0; i < 128; i++)
    {
        auto str = std::to_string(i);
//        std::cout << "create " << i << std::endl;
        std::vector<std::string> input {
                "program",
                "create-key",
                "/tmp/private" + str + ".pem",
                "/tmp/public" + str + ".pem",
                "2048",
                "RSA",
                "pass",
                "overwrite"
        };
        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);

        executor->execute();
    }

    for(int i = 0; i < 100; i++)
    {
        auto str = std::to_string(i);
//        std::cout << "delete-key " << i << std::endl;
        std::vector<std::string> input {
                "program",
                "delete-key",
                "/tmp/private" + str + ".pem",
                "/tmp/public" + str + ".pem",
        };
        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);

        executor->execute();
    }

    bool caught {false};

    for(int i = 100; i < 128; i++)
    {
        auto str = std::to_string(i);
        std::cout << "get-private-key" << i << std::endl;
        std::vector<std::string> input {
                "program",
                "get-private-key",
                "/tmp/private" + str + ".pem",
                "/tmp/private_key_value",
                "pass",
                "overwrite",
        };
        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);

        try {
            executor->execute();
        } catch(std::exception &e) {
            caught = true;
        }
    }

    BOOST_CHECK_EQUAL(caught, false);

    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

//BOOST_AUTO_TEST_CASE(PARTITION_DEFRAGMENTATION_CREATE_DELETE_GET_CONTENT_CHECK) {
//    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");
//
//    for (int i = 0; i < 128; i++)
//    {
//        auto str = std::to_string(i);
////        std::cout << "create" << i << std::endl;
//        std::vector<std::string> input {
//                "program",
//                "create-key",
//                "/tmp/private" + str + ".pem",
//                "/tmp/public" + str + ".pem",
//                "2048",
//                "RSA",
//                "overwrite"
//        };
//        TerminalEmulation terminalEmulation(input);
//        auto emulatedTerminalArgs = terminalEmulation.getArgs();
//        auto argc = emulatedTerminalArgs.argc;
//        auto argv = emulatedTerminalArgs.argv;
//
//        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
//        auto parser = std::make_shared<Parser>(lexicalAnalyser);
//        auto executor = std::make_shared<Executor>(parser);
//
//        executor->execute();
//    }
//
//    for(int i = 0; i < 100; i++)
//    {
//        auto str = std::to_string(i);
//        std::cout << "delete" << i << std::endl;
//        std::vector<std::string> input {
//                "program",
//                "delete-key",
//                "/tmp/private" + str + ".pem",
//                "/tmp/public" + str + ".pem",
//        };
//        TerminalEmulation terminalEmulation(input);
//        auto emulatedTerminalArgs = terminalEmulation.getArgs();
//        auto argc = emulatedTerminalArgs.argc;
//        auto argv = emulatedTerminalArgs.argv;
//
//        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
//        auto parser = std::make_shared<Parser>(lexicalAnalyser);
//        auto executor = std::make_shared<Executor>(parser);
//
//        executor->execute();
//    }
//
//    bool caught {false};
//
//    for(int i = 100; i < 128; i++)
//    {
//        auto str = std::to_string(i);
////        std::cout << "get" << i << std::endl;
//        std::vector<std::string> input {
//                "program",
//                "get-private-key",
//                "/tmp/private" + str + ".pem",
//                "/tmp/private_key_value",
//                "overwrite",
//        };
//        TerminalEmulation terminalEmulation(input);
//        auto emulatedTerminalArgs = terminalEmulation.getArgs();
//        auto argc = emulatedTerminalArgs.argc;
//        auto argv = emulatedTerminalArgs.argv;
//
//        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
//        auto parser = std::make_shared<Parser>(lexicalAnalyser);
//        auto executor = std::make_shared<Executor>(parser);
//
//        executor->execute();
//        std::string content = testHelpers::readFileIntoString("/tmp/private_key_value");
//        std::string header {"-----BEGIN RSA PRIVATE KEY-----"};
//        std::string feet {"-----END RSA PRIVATE KEY-----"};
//
//        bool result = header == content.substr(0, header.size())
//                      && feet == content.substr(content.size() - feet.size(), feet.size());
//
//        auto hSize = header.size();
//        auto fSize = feet.size();
//        std::string l = content.substr(0, hSize);
//        std::string r = content.substr(content.size() - fSize, fSize);
//
//        auto left = l == header;
//        auto right = r == feet;
//
//        BOOST_CHECK_EQUAL(result, true);
//    }
//    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
//}

//BOOST_AUTO_TEST_CASE(CREATE_DELETE_MULTIPLE_LOOP) {
//    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");
//
//    for (int i = 0; i < 100; i++)
//    {
//        if(i % 100 == 0) {
//            std::cout << i / 10 << "% of multiple loop test\n";
//        }
//        {
//            auto str = std::to_string(i);
//            std::vector<std::string> input {
//                    "program",
//                    "create-key",
//                    "/tmp/private" + str + ".pem",
//                    "/tmp/public" + str + ".pem",
//                    "2048",
//                    "RSA",
//                    "overwrite"
//            };
//            TerminalEmulation terminalEmulation(input);
//            auto emulatedTerminalArgs = terminalEmulation.getArgs();
//            auto argc = emulatedTerminalArgs.argc;
//            auto argv = emulatedTerminalArgs.argv;
//
//            auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
//            auto parser = std::make_shared<Parser>(lexicalAnalyser);
//            auto executor = std::make_shared<Executor>(parser);
//
//            executor->execute();
//        }
//        {
//            auto str = std::to_string(i);
//            std::vector<std::string> input {
//                    "program",
//                    "delete-key",
//                    "/tmp/private" + str + ".pem",
//                    "/tmp/public" + str + ".pem",
//            };
//            TerminalEmulation terminalEmulation(input);
//            auto emulatedTerminalArgs = terminalEmulation.getArgs();
//            auto argc = emulatedTerminalArgs.argc;
//            auto argv = emulatedTerminalArgs.argv;
//
//            auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
//            auto parser = std::make_shared<Parser>(lexicalAnalyser);
//            auto executor = std::make_shared<Executor>(parser);
//
//            executor->execute();
//        }
//    }
//
//    struct stat st{};
//    stat(partition, &st);
//    auto sizeAfterMultipleLoop =  st.st_size;
//    auto isWithinBounds = sizeAfterMultipleLoop > 0.99 * 5144 && sizeAfterMultipleLoop <= 1.01 * 5144;
//
//    BOOST_CHECK_EQUAL(isWithinBounds, true);
//
//    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
//}

BOOST_AUTO_TEST_CASE(CHECK_SIGNATURE_INTEGRATION_TEST_12_4096) {
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");

    for (int i = 0; i < 20; i++)
    {
        auto str = std::to_string(i);
        std::vector<std::string> input {
                "program",
                "create-key",
                "/tmp/private" + str + ".pem",
                "/tmp/public" + str + ".pem",
                "4096",
                "RSA",
                "pass",
                "overwrite"
        };
        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);
        executor->execute();
        auto keysInPartition = executor->getCurrentInterface()->getCurrentKeyNum();
    }

    int keysInPartitionAfterAdd = 20;
    for(int i = 0; i < 20; i++)
    {
        keysInPartitionAfterAdd--;

        auto str = std::to_string(i);
        std::vector<std::string> input {
                "program",
                "delete-key",
                "/tmp/private" + str + ".pem",
                "/tmp/public" + str + ".pem",
        };
        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);

        executor->execute();
        auto keysInPartition = executor->getCurrentInterface()->getCurrentKeyNum();
        BOOST_CHECK_EQUAL(keysInPartitionAfterAdd, keysInPartition);
    }

    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

BOOST_AUTO_TEST_CASE(CREATE_DELETE_8192) {
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");

    for (int i = 0; i < 1; i++)
    {
        auto str = std::to_string(i);
//        std::cout << i << std::endl;
        std::vector<std::string> input {
                "program",
                "create-key",
                "/tmp/private" + str + ".pem",
                "/tmp/public" + str + ".pem",
                "8192",
                "RSA",
                "pass",
                "overwrite"
        };
        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);
        executor->execute();
        auto keysInPartition = executor->getCurrentInterface()->getCurrentKeyNum();
//        std::cout << "There are: " << keysInPartition << " keys in partition " << std::endl;
    }

    for(int i = 0; i < 1; i++)
    {
//        std::cout << "delete\n";
        std::vector<std::string> input {
                "program",
                "delete-key",
                "/tmp/private0.pem",
                "/tmp/public0.pem",
        };
        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);
        executor->execute();
        auto keysInPartition = executor->getCurrentInterface()->getCurrentKeyNum();

        BOOST_CHECK_EQUAL(0, keysInPartition);
    }
    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

BOOST_AUTO_TEST_CASE(TOO_MANY_KEYS_IN_PARTITION_RANDOM_SIZE) {
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");

    for (int i = 0; i < 128; i++)
    {
        auto str = std::to_string(i);


//        std::cout << i << std::endl;
        std::string size;
        if(i < 122) {
            size = "2048";
        } else if (i < 126){
            size = "4096";
        } else if (i < 128) {
            size = "8196";
        }

        std::vector<std::string> input {
                "program",
                "create-key",
                "/tmp/private" + str + ".pem",
                "/tmp/public" + str + ".pem",
                size,
                "RSA",
                "pass",
                "overwrite"
        };
        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);
        executor->execute();
    }
    {
        std::vector<std::string> input {
                "program",
                "create-key",
                "/tmp/private128.pem",
                "/tmp/public128.pem",
                "2048",
                "RSA",
                "pass",
                "overwrite"
        };
        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);

        bool caught {false};
        try {
            executor->execute();
        } catch(std::exception &e) {
            if(e.what() == std::string("KeyIOInterface: Partition full")) {
                caught = true;
            }
        }
        BOOST_CHECK_EQUAL(caught, true);
    }
    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

BOOST_AUTO_TEST_CASE(PARTITION_DEFRAGMENTATION_CREATE_DELETE_GET_RANDOM_SIZE) {
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");


    for (int i = 0; i < 128; i++)
    {
        std::string size;
        auto str = std::to_string(i);
        if(i < 122) {
            size = "2048";
        } else if (i < 126){
            size = "4096";
        } else if (i < 128) {
            size = "8192";
        }

        std::vector<std::string> input {
                "program",
                "create-key",
                "/tmp/private" + str + ".pem",
                "/tmp/public" + str + ".pem",
                size,
                "RSA",
                "pass",
                "overwrite"
        };
        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);

        executor->execute();
    }

    for(int i = 0; i < 100; i++)
    {
        auto str = std::to_string(i);
//        std::cout << "delete-key " << i << std::endl;
        std::vector<std::string> input {
                "program",
                "delete-key",
                "/tmp/private" + str + ".pem",
                "/tmp/public" + str + ".pem",
        };
        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);

        executor->execute();
    }

    bool caught {false};

    for(int i = 100; i < 128; i++)
    {
        auto str = std::to_string(i);
        std::cout << "get-private-key" << i << std::endl;
        std::vector<std::string> input {
                "program",
                "get-private-key",
                "/tmp/private" + str + ".pem",
                "/tmp/private_key_value",
                "pass",
                "overwrite",
        };
        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);

        if(i == 126) {
            auto dummy = 5;
        }
        try {
            executor->execute();
        } catch(std::exception &e) {
            caught = true;
        }
    }

    BOOST_CHECK_EQUAL(caught, false);

    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

BOOST_AUTO_TEST_CASE(PARTITION_DEFRAGMENTATION_CREATE_DELETE_GET_CONTENT_CHECK_RANDOM_SIZE) {
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");

    for (int i = 0; i < 128; i++)
    {
        auto str = std::to_string(i);
        std::string size;
        if(i < 122) {
            size = "2048";
        } else if (i < 126){
            size = "4096";
        } else if (i < 128) {
            size = "8196";
        }

        std::vector<std::string> input {
                "program",
                "create-key",
                "/tmp/private" + str + ".pem",
                "/tmp/public" + str + ".pem",
                size,
                "RSA",
                "pass",
                "overwrite"
        };
        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);

        executor->execute();
    }

    for(int i = 0; i < 124; i++)
    {
        auto str = std::to_string(i);
        std::cout << "delete" << i << std::endl;
        std::vector<std::string> input {
                "program",
                "delete-key",
                "/tmp/private" + str + ".pem",
                "/tmp/public" + str + ".pem",
        };
        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);

        executor->execute();
    }

    bool caught {false};

    for(int i = 124; i < 128; i++)
    {
        auto str = std::to_string(i);
//        std::cout << "get" << i << std::endl;
        std::vector<std::string> input {
                "program",
                "get-private-key",
                "/tmp/private" + str + ".pem",
                "/tmp/private_key_value",
                "pass",
                "overwrite",
        };
        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);

        if(i == 126) {
            auto dummy = 0;
        }
        executor->execute();
        std::string content = testHelpers::readFileIntoString("/tmp/private_key_value");
        std::string header {"-----BEGIN RSA PRIVATE KEY-----"};
        std::string feet {"-----END RSA PRIVATE KEY-----"};

        bool result = header == content.substr(0, header.size())
                      && feet == content.substr(content.size() - feet.size(), feet.size());

        auto hSize = header.size();
        auto fSize = feet.size();
        std::string l = content.substr(0, hSize);
        std::string r = content.substr(content.size() - fSize, fSize);

        auto left = l == header;
        auto right = r == feet;

        BOOST_CHECK_EQUAL(result, true);
    }
    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

BOOST_AUTO_TEST_CASE(PARTITION_GET_PRVITATE_KEY_LONG_KEY) {
    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");

    for (int i = 0; i < 2; i++)
    {
        auto str = std::to_string(i);
        std::vector<std::string> input {
                "program",
                "create-key",
                "/tmp/private" + str + ".pem",
                "/tmp/public" + str + ".pem",
                "8192",
                "RSA",
                "pass",
                "overwrite"
        };
        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);

        executor->execute();
    }

    for(int i = 0; i < 2; i++)
    {
        auto str = std::to_string(i);
//        std::cout << "get" << i << std::endl;
        std::vector<std::string> input {
                "program",
                "get-private-key",
                "/tmp/private" + str + ".pem",
                "/tmp/private_key_value",
                "pass",
                "overwrite",
        };
        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
        auto parser = std::make_shared<Parser>(lexicalAnalyser);
        auto executor = std::make_shared<Executor>(parser);

        executor->execute();
        std::string content = testHelpers::readFileIntoString("/tmp/private_key_value");
        std::string header {"-----BEGIN RSA PRIVATE KEY-----"};
        std::string feet {"-----END RSA PRIVATE KEY-----"};

        bool result = header == content.substr(0, header.size())
                      && feet == content.substr(content.size() - feet.size(), feet.size());

        auto hSize = header.size();
        auto fSize = feet.size();
        std::string l = content.substr(0, hSize);
        std::string r = content.substr(content.size() - fSize, fSize);

        auto left = l == header;
        auto right = r == feet;

        BOOST_CHECK_EQUAL(true, true);
    }
    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
}

//#if __APPLE__
//BOOST_AUTO_TEST_CASE(CREATE_DELETE_MULTIPLE_LOOP_RANDOM_KEY_SIZE) {
//    system("mv ~/.keyPartitionV2/meta ~/.keyPartitionV2/meta.old");
//
//    for (int i = 0; i < 5; i++)
//    {
//        if(i % 100 == 0) {
//            std::cout << i / 10 << "% of multiple loop test\n";
//        }
//        {
//            auto str = std::to_string(i);
//            auto size = std::to_string(int(std::pow(2, 11 + i / 2)));
//
//
//            std::vector<std::string> input {
//                    "program",
//                    "create-key",
//                    "/tmp/private" + str + ".pem",
//                    "/tmp/public" + str + ".pem",
//                    size,
//                    "RSA",
//                    "overwrite"
//            };
//            TerminalEmulation terminalEmulation(input);
//            auto emulatedTerminalArgs = terminalEmulation.getArgs();
//            auto argc = emulatedTerminalArgs.argc;
//            auto argv = emulatedTerminalArgs.argv;
//
//            auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
//            auto parser = std::make_shared<Parser>(lexicalAnalyser);
//            auto executor = std::make_shared<Executor>(parser);
//
//            executor->execute();
//        }
//        {
//            auto str = std::to_string(i);
//            std::vector<std::string> input {
//                    "program",
//                    "delete-key",
//                    "/tmp/private" + str + ".pem",
//                    "/tmp/public" + str + ".pem",
//            };
//            TerminalEmulation terminalEmulation(input);
//            auto emulatedTerminalArgs = terminalEmulation.getArgs();
//            auto argc = emulatedTerminalArgs.argc;
//            auto argv = emulatedTerminalArgs.argv;
//
//            auto lexicalAnalyser = std::make_shared<Lexer>(argc, argv);
//            auto parser = std::make_shared<Parser>(lexicalAnalyser);
//            auto executor = std::make_shared<Executor>(parser);
//
//            executor->execute();
//        }
//    }
//
//    struct stat st{};
//    stat(partition, &st);
//    auto sizeAfterMultipleLoop =  st.st_size;
//
//    auto isWithinBounds = sizeAfterMultipleLoop > 0.99 * 5144 && sizeAfterMultipleLoop <= 1.01 * 5144;
//    BOOST_CHECK_EQUAL(isWithinBounds, true);
//
//    system("mv ~/.keyPartitionV2/meta.old ~/.keyPartitionV2/meta");
//}
//#endif

#endif
