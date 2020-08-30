#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN  // in only one cpp file
#include <boost/test/unit_test.hpp>
#include "../include/TerminalEmulation.h"
#include "../include/Executor.h"
#include <memory>

#define LONG_RUN 0

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
            "/home/r.ostoja",
    };
    TerminalEmulation terminalEmulation(input);
    auto args = terminalEmulation.getArgs();
    auto argc = args.argc;
    auto argv = args.argv;

    auto syntaxAnalyser = std::make_shared<SyntaxAnalyser>(argc, argv);
    auto parser = std::make_unique<Parser>(syntaxAnalyser);
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
    auto statement = parser->getCurrentParsedStatementStr();
    auto serialisedInput = testHelpers::toString(input);
    BOOST_CHECK_EQUAL(statement, serialisedInput);
}
BOOST_AUTO_TEST_CASE(INTEGRATION_TEST_2)
{
    std::vector<std::string> input {
            "program",
            "delete-key",
            "413243",
            "/home/pubKeyPath"
    };
    TerminalEmulation terminalEmulation(input);
    auto args = terminalEmulation.getArgs();
    auto argc = args.argc;
    auto argv = args.argv;

    auto syntaxAnalyser = std::make_shared<SyntaxAnalyser>(argc, argv);
    auto parser = std::make_unique<Parser>(syntaxAnalyser);
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
            "/home/abc",
            "312321",
            "outpath",
    };
    TerminalEmulation terminalEmulation(input);
    auto args = terminalEmulation.getArgs();
    auto argc = args.argc;
    auto argv = args.argv;

    auto syntaxAnalyser = std::make_shared<SyntaxAnalyser>(argc, argv);
    auto parser = std::make_unique<Parser>(syntaxAnalyser);
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
            "/home/abc",
            "/home/pubKeyPath",
            "signature_path"
    };
    TerminalEmulation terminalEmulation(input);
    auto args = terminalEmulation.getArgs();
    auto argc = args.argc;
    auto argv = args.argv;

    auto syntaxAnalyser = std::make_shared<SyntaxAnalyser>(argc, argv);
    auto parser = std::make_unique<Parser>(syntaxAnalyser);
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
            "/home/file-to-encrypt",
            "output",
            "413243",
    };
    TerminalEmulation terminalEmulation(input);
    auto args = terminalEmulation.getArgs();
    auto argc = args.argc;
    auto argv = args.argv;

    auto syntaxAnalyser = std::make_shared<SyntaxAnalyser>(argc, argv);
    auto parser = std::make_unique<Parser>(syntaxAnalyser);
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
            "/home/file-to-decrypt",
            "output",
            "413243",
    };
    TerminalEmulation terminalEmulation(input);
    auto args = terminalEmulation.getArgs();
    auto argc = args.argc;
    auto argv = args.argv;

    auto syntaxAnalyser = std::make_shared<SyntaxAnalyser>(argc, argv);
    auto parser = std::make_unique<Parser>(syntaxAnalyser);
    parser->parse();
    auto statement = parser->getCurrentParsedStatementStr();
    auto serialisedInput = testHelpers::toString(input);
    BOOST_CHECK_EQUAL(statement, serialisedInput);
}
BOOST_AUTO_TEST_CASE(CHECK_SIGNATURE_INTEGRATION_TEST_6)
{
    {
        std::vector<std::string> input {
                "program",
                "create-key",
                "RSA",
                "2048",
                "/home/robert/Desktop/public.pem",
                "/home/robert/Desktop/private.pem",
        };
        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto syntaxAnalyser = std::make_shared<SyntaxAnalyser>(argc, argv);
        auto parser = std::make_shared<Parser>(syntaxAnalyser);
        auto executor = std::make_shared<Executor>(parser);
        executor->execute();
    }
    {
        std::vector<std::string> input {
                "program",
                "sign",
                "/home/robert/Desktop/file.txt",
                "/home/robert/Desktop/private.pem",
                "/home/robert/Desktop/signature.txt",
        };

        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto syntaxAnalyser = std::make_shared<SyntaxAnalyser>(argc, argv);
        auto parser = std::make_shared<Parser>(syntaxAnalyser);
        auto executor = std::make_shared<Executor>(parser);
        executor->execute();
    }
    {
        std::vector<std::string> input {
                "program",
                "check-signature",
                "/home/robert/Desktop/file.txt",
                "/home/robert/Desktop/public.pem",
                "/home/robert/Desktop/signature.txt",
        };

        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto syntaxAnalyser = std::make_shared<SyntaxAnalyser>(argc, argv);
        auto parser = std::make_shared<Parser>(syntaxAnalyser);
        auto executor = std::make_shared<Executor>(parser);
        executor->execute();

        int executorResult = int(executor->getResult());
        BOOST_CHECK_EQUAL(executorResult, int(CallResult::SIGNATURE_THE_SAME));
    }
}

BOOST_AUTO_TEST_CASE(CHECK_SIGNATURE_INTEGRATION_TEST_7)
{
    system("mv ~/.keyPartition ~/.keyPartition.old");
    {
        std::vector<std::string> input {
                "program",
                "create-key",
                "RSA",
                "2048",
                "/home/robert/Desktop/public.pem",
                "/home/robert/Desktop/private.pem",
        };
        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto syntaxAnalyser = std::make_shared<SyntaxAnalyser>(argc, argv);
        auto parser = std::make_shared<Parser>(syntaxAnalyser);
        auto executor = std::make_shared<Executor>(parser);
        executor->execute();
    }
    {
        std::vector<std::string> input {
                "program",
                "sign",
                "/home/robert/Desktop/file.txt",
                "/home/robert/Desktop/private.pem",
                "/home/robert/Desktop/signature.txt",
        };

        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto syntaxAnalyser = std::make_shared<SyntaxAnalyser>(argc, argv);
        auto parser = std::make_shared<Parser>(syntaxAnalyser);
        auto executor = std::make_shared<Executor>(parser);
        executor->execute();
    }
    {
        std::vector<std::string> input {
                "program",
                "check-signature",
                "/home/robert/Desktop/file.txt",
                "/home/robert/Desktop/public.pem",
                "/home/robert/Desktop/signature.txt",
        };

        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto syntaxAnalyser = std::make_shared<SyntaxAnalyser>(argc, argv);
        auto parser = std::make_shared<Parser>(syntaxAnalyser);
        auto executor = std::make_shared<Executor>(parser);
        executor->execute();

        int executorResult = int(executor->getResult());
        BOOST_CHECK_EQUAL(executorResult, int(CallResult::SIGNATURE_THE_SAME));
    }
    system("mv ~/.keyPartition.old ~/.keyPartition");
}

// a long test - fills whole partition with keys
BOOST_AUTO_TEST_CASE(CHECK_SIGNATURE_INTEGRATION_TEST_8)
{
    system("mv ~/.keyPartition ~/.keyPartition.old");
    for (int i = 0; i < -1; i++) {
        {
            std::vector<std::string> input {
                    "program",
                    "create-key",
                    "RSA",
                    "2048",
                    "/home/robert/Desktop/public.pem",
                    "/home/robert/Desktop/private.pem",
            };
            TerminalEmulation terminalEmulation(input);
            auto emulatedTerminalArgs = terminalEmulation.getArgs();
            auto argc = emulatedTerminalArgs.argc;
            auto argv = emulatedTerminalArgs.argv;

            auto syntaxAnalyser = std::make_shared<SyntaxAnalyser>(argc, argv);
            auto parser = std::make_shared<Parser>(syntaxAnalyser);
            auto executor = std::make_shared<Executor>(parser);
            executor->execute();
        }
        {
            std::vector<std::string> input {
                    "program",
                    "sign",
                    "/home/robert/Desktop/file.txt",
                    "/home/robert/Desktop/private.pem",
                    "/home/robert/Desktop/signature.txt",
            };

            TerminalEmulation terminalEmulation(input);
            auto emulatedTerminalArgs = terminalEmulation.getArgs();
            auto argc = emulatedTerminalArgs.argc;
            auto argv = emulatedTerminalArgs.argv;

            auto syntaxAnalyser = std::make_shared<SyntaxAnalyser>(argc, argv);
            auto parser = std::make_shared<Parser>(syntaxAnalyser);
            auto executor = std::make_shared<Executor>(parser);
            executor->execute();
        }
        {
            std::vector<std::string> input {
                    "program",
                    "check-signature",
                    "/home/robert/Desktop/file.txt",
                    "/home/robert/Desktop/public.pem",
                    "/home/robert/Desktop/signature.txt",
            };

            TerminalEmulation terminalEmulation(input);
            auto emulatedTerminalArgs = terminalEmulation.getArgs();
            auto argc = emulatedTerminalArgs.argc;
            auto argv = emulatedTerminalArgs.argv;

            auto syntaxAnalyser = std::make_shared<SyntaxAnalyser>(argc, argv);
            auto parser = std::make_shared<Parser>(syntaxAnalyser);
            auto executor = std::make_shared<Executor>(parser);
            executor->execute();

            int executorResult = int(executor->getResult());
            BOOST_CHECK_EQUAL(executorResult, int(CallResult::SIGNATURE_THE_SAME));
        }
    }
    system("mv ~/.keyPartition.old ~/.keyPartition");
}

// a long test - fills whole partition with keys and checks PartitionInfo
BOOST_AUTO_TEST_CASE(CHECK_SIGNATURE_INTEGRATION_TEST_9) {
    system("mv ~/.keyPartition ~/.keyPartition.old");
        {
            std::vector<std::string> input {
                    "program",
                    "create-key",
                    "RSA",
                    "2048",
                    "/home/robert/Desktop/public.pem",
                    "/home/robert/Desktop/private.pem",
            };
            TerminalEmulation terminalEmulation(input);
            auto emulatedTerminalArgs = terminalEmulation.getArgs();
            auto argc = emulatedTerminalArgs.argc;
            auto argv = emulatedTerminalArgs.argv;

            auto syntaxAnalyser = std::make_shared<SyntaxAnalyser>(argc, argv);
            auto parser = std::make_shared<Parser>(syntaxAnalyser);
            auto executor = std::make_shared<Executor>(parser);
            executor->execute();
            auto keysInPartition = executor->getCurrentInterface()->getCurrentKeyNum();
            BOOST_CHECK_EQUAL(1, keysInPartition);
        }

    system("mv ~/.keyPartition.old ~/.keyPartition");
}

// a long test - fills whole partition with keys and checks PartitionInfo
BOOST_AUTO_TEST_CASE(CHECK_SIGNATURE_INTEGRATION_TEST_10) {
    system("mv ~/.keyPartition ~/.keyPartition.old");
    for (int i = 0; i < 9; i++)
    {
        std::vector<std::string> input {
                "program",
                "create-key",
                "RSA",
                "2048",
                "/home/robert/Desktop/public.pem",
                "/home/robert/Desktop/private.pem",
        };
        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto syntaxAnalyser = std::make_shared<SyntaxAnalyser>(argc, argv);
        auto parser = std::make_shared<Parser>(syntaxAnalyser);
        auto executor = std::make_shared<Executor>(parser);
        executor->execute();
        auto keysInPartition = executor->getCurrentInterface()->getCurrentKeyNum();
    }
    {
        std::vector<std::string> input {
                "program",
                "create-key",
                "RSA",
                "2048",
                "/home/robert/Desktop/public.pem",
                "/home/robert/Desktop/private.pem",
        };
        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto syntaxAnalyser = std::make_shared<SyntaxAnalyser>(argc, argv);
        auto parser = std::make_shared<Parser>(syntaxAnalyser);
        auto executor = std::make_shared<Executor>(parser);
        executor->execute();
        auto keysInPartition = executor->getCurrentInterface()->getCurrentKeyNum();
        BOOST_CHECK_EQUAL(10, keysInPartition);
    }
    system("mv ~/.keyPartition.old ~/.keyPartition");
}

// a long test - fills whole partition with keys and checks PartitionInfo
BOOST_AUTO_TEST_CASE(CHECK_SIGNATURE_INTEGRATION_TEST_11) {
    system("mv ~/.keyPartition ~/.keyPartition.old");

    for (int i = 0; i < 1; i++)
    {
        std::vector<std::string> input {
                "program",
                "create-key",
                "RSA",
                "2048",
                "/home/robert/Desktop/public.pem",
                "/home/robert/Desktop/private.pem",
        };
        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto syntaxAnalyser = std::make_shared<SyntaxAnalyser>(argc, argv);
        auto parser = std::make_shared<Parser>(syntaxAnalyser);
        auto executor = std::make_shared<Executor>(parser);
        executor->execute();
        auto keysInPartition = executor->getCurrentInterface()->getCurrentKeyNum();
    }
    {
        std::vector<std::string> input {
                "program",
                "delete-key",
                "/home/robert/Desktop/private.pem",
                "/home/robert/Desktop/public.pem",
        };
        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto syntaxAnalyser = std::make_shared<SyntaxAnalyser>(argc, argv);
        auto parser = std::make_shared<Parser>(syntaxAnalyser);
        auto executor = std::make_shared<Executor>(parser);
        executor->execute();
        auto keysInPartition = executor->getCurrentInterface()->getCurrentKeyNum();

        BOOST_CHECK_EQUAL(0, keysInPartition);
    }
    system("mv ~/.keyPartition.old ~/.keyPartition");
}

#if(LONG_RUN)
BOOST_AUTO_TEST_CASE(CHECK_SIGNATURE_INTEGRATION_TEST_12) {
    system("mv ~/.keyPartition ~/.keyPartition.old");

    for (int i = 0; i < 128; i++)
    {
        auto str = std::to_string(i);
        std::vector<std::string> input {
                "program",
                "create-key",
                "RSA",
                "2048",
                "/tmp/public" + str + ".pem",
                "/tmp/private" + str + ".pem",
        };
        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto syntaxAnalyser = std::make_shared<SyntaxAnalyser>(argc, argv);
        auto parser = std::make_shared<Parser>(syntaxAnalyser);
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

        auto syntaxAnalyser = std::make_shared<SyntaxAnalyser>(argc, argv);
        auto parser = std::make_shared<Parser>(syntaxAnalyser);
        auto executor = std::make_shared<Executor>(parser);
        executor->execute();
        auto keysInPartition = executor->getCurrentInterface()->getCurrentKeyNum();
        BOOST_CHECK_EQUAL(keysInPartitionAfterAdd, keysInPartition);
    }

    system("mv ~/.keyPartition.old ~/.keyPartition");
}

BOOST_AUTO_TEST_CASE(CHECK_SIGNATURE_INTEGRATION_TEST_13) {
    system("mv ~/.keyPartition ~/.keyPartition.old");

    for (int i = 0; i < 128; i++)
    {
        auto str = std::to_string(i);
        std::vector<std::string> input {
                "program",
                "create-key",
                "RSA",
                "2048",
                "/tmp/public" + str + ".pem",
                "/tmp/private" + str + ".pem",
        };
        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto syntaxAnalyser = std::make_shared<SyntaxAnalyser>(argc, argv);
        auto parser = std::make_shared<Parser>(syntaxAnalyser);
        auto executor = std::make_shared<Executor>(parser);
        executor->execute();
        auto keysInPartition = executor->getCurrentInterface()->getCurrentKeyNum();
//        std::cout << "There are: " << keysInPartition << " keys in partition " << std::endl;
    }

    for(int i = 0; i < 1; i++)
    {
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

        auto syntaxAnalyser = std::make_shared<SyntaxAnalyser>(argc, argv);
        auto parser = std::make_shared<Parser>(syntaxAnalyser);
        auto executor = std::make_shared<Executor>(parser);
        executor->execute();
        auto keysInPartition = executor->getCurrentInterface()->getCurrentKeyNum();

        BOOST_CHECK_EQUAL(127, keysInPartition);
    }
    system("mv ~/.keyPartition.old ~/.keyPartition");
}
#endif

BOOST_AUTO_TEST_CASE(CHECK_SIGNATURE_INTEGRATION_TEST_14) {
    system("mv ~/.keyPartition ~/.keyPartition.old");
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

        auto syntaxAnalyser = std::make_shared<SyntaxAnalyser>(argc, argv);
        auto parser = std::make_shared<Parser>(syntaxAnalyser);
        auto executor = std::make_shared<Executor>(parser);
        executor->execute();
        int remove_return = int(executor->getResult());

        BOOST_CHECK_EQUAL(remove_return, int(CallResult::NO_PRV_KEY_TO_REMOVE));
    }
    system("mv ~/.keyPartition.old ~/.keyPartition");
}

BOOST_AUTO_TEST_CASE(CHECK_SIGNATURE_INTEGRATION_TEST_15) {

    system("mv ~/.keyPartition ~/.keyPartition.old");
    for (int i = 0; i < 1; i++)
    {
        std::vector<std::string> input {
                "program",
                "create-key",
                "RSA",
                "2048",
                "/tmp/public.pem",
                "/tmp/private.pem",
        };
        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto syntaxAnalyser = std::make_shared<SyntaxAnalyser>(argc, argv);
        auto parser = std::make_shared<Parser>(syntaxAnalyser);
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

        auto syntaxAnalyser = std::make_shared<SyntaxAnalyser>(argc, argv);
        auto parser = std::make_shared<Parser>(syntaxAnalyser);
        auto executor = std::make_shared<Executor>(parser);
        executor->execute();
        int remove_return = int(executor->getResult());

        BOOST_CHECK_EQUAL(remove_return, int(CallResult::NO_PUB_KEY_TO_REMOVE));
    }
    system("mv ~/.keyPartition.old ~/.keyPartition");
}

BOOST_AUTO_TEST_CASE(CHECK_SIGNATURE_INTEGRATION_TEST_16) {
    system("mv ~/.keyPartition ~/.keyPartition.old");
    for (int i = 0; i < 1; i++)
    {
        std::vector<std::string> input {
                "program",
                "create-key",
                "RSA",
                "2048",
                "/home/robert/Desktop/public.pem",
                "/home/robert/Desktop/private.pem",
        };
        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto syntaxAnalyser = std::make_shared<SyntaxAnalyser>(argc, argv);
        auto parser = std::make_shared<Parser>(syntaxAnalyser);
        auto executor = std::make_shared<Executor>(parser);
        executor->execute();
        auto keysInPartition = executor->getCurrentInterface()->getCurrentKeyNum();
    }

    for(int i = 0; i < 1; i++)
    {
        std::vector<std::string> input {
                "program",
                "delete-key",
                "/home/robert/Desktop/private.pem",
                "/home/robert/Desktop/public.pem",
        };
        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto syntaxAnalyser = std::make_shared<SyntaxAnalyser>(argc, argv);
        auto parser = std::make_shared<Parser>(syntaxAnalyser);
        auto executor = std::make_shared<Executor>(parser);
        executor->execute();
        int remove_return = int(executor->getResult());

        BOOST_CHECK_EQUAL(remove_return, int(CallResult::KEY_REMOVE_SUCCESS));
    }
    system("mv ~/.keyPartition.old ~/.keyPartition");
}

BOOST_AUTO_TEST_CASE(WRONG_INPUT_1)
{
    bool errorFound = false;
    std::vector<std::string> input {
            "program",
            "unknown_command",
            "RSA",
            "2048",
            "/home/robert/Desktop/public.pem",
            "/home/robert/Desktop/private.pem",
    };
    TerminalEmulation terminalEmulation(input);
    auto emulatedTerminalArgs = terminalEmulation.getArgs();
    auto argc = emulatedTerminalArgs.argc;
    auto argv = emulatedTerminalArgs.argv;

    auto syntaxAnalyser = std::make_shared<SyntaxAnalyser>(argc, argv);
    auto parser = std::make_shared<Parser>(syntaxAnalyser);
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
            "NO-ALGORITHM",
            "2048",
            "/home/robert/Desktop/public.pem",
            "/home/robert/Desktop/private.pem",
    };
    TerminalEmulation terminalEmulation(input);
    auto emulatedTerminalArgs = terminalEmulation.getArgs();
    auto argc = emulatedTerminalArgs.argc;
    auto argv = emulatedTerminalArgs.argv;

    auto syntaxAnalyser = std::make_shared<SyntaxAnalyser>(argc, argv);
    auto parser = std::make_shared<Parser>(syntaxAnalyser);
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
            "RSA",
            "-2048",
            "/home/robert/Desktop/public.pem",
            "/home/robert/Desktop/private.pem",
    };
    TerminalEmulation terminalEmulation(input);
    auto emulatedTerminalArgs = terminalEmulation.getArgs();
    auto argc = emulatedTerminalArgs.argc;
    auto argv = emulatedTerminalArgs.argv;

    auto syntaxAnalyser = std::make_shared<SyntaxAnalyser>(argc, argv);
    auto parser = std::make_shared<Parser>(syntaxAnalyser);
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
            "RSA",
            "0",
            "/home/robert/Desktop/public.pem",
            "/home/robert/Desktop/private.pem",
    };
    TerminalEmulation terminalEmulation(input);
    auto emulatedTerminalArgs = terminalEmulation.getArgs();
    auto argc = emulatedTerminalArgs.argc;
    auto argv = emulatedTerminalArgs.argv;

    auto syntaxAnalyser = std::make_shared<SyntaxAnalyser>(argc, argv);
    auto parser = std::make_shared<Parser>(syntaxAnalyser);
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
            "RSA",
            "10000000",
            "/home/robert/Desktop/public.pem",
            "/home/robert/Desktop/private.pem",
    };
    TerminalEmulation terminalEmulation(input);
    auto emulatedTerminalArgs = terminalEmulation.getArgs();
    auto argc = emulatedTerminalArgs.argc;
    auto argv = emulatedTerminalArgs.argv;

    auto syntaxAnalyser = std::make_shared<SyntaxAnalyser>(argc, argv);
    auto parser = std::make_shared<Parser>(syntaxAnalyser);
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
            "RSA",
            "100000000000000000000",
            "/home/robert/Desktop/public.pem",
            "/home/robert/Desktop/private.pem",
    };
    TerminalEmulation terminalEmulation(input);
    auto emulatedTerminalArgs = terminalEmulation.getArgs();
    auto argc = emulatedTerminalArgs.argc;
    auto argv = emulatedTerminalArgs.argv;

    auto syntaxAnalyser = std::make_shared<SyntaxAnalyser>(argc, argv);
    auto parser = std::make_shared<Parser>(syntaxAnalyser);
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
            "RSA",
            "204w8",
            "qwertyuio",
            "/home/robert/Desktop/private.pem",
    };
    TerminalEmulation terminalEmulation(input);
    auto emulatedTerminalArgs = terminalEmulation.getArgs();
    auto argc = emulatedTerminalArgs.argc;
    auto argv = emulatedTerminalArgs.argv;

    auto syntaxAnalyser = std::make_shared<SyntaxAnalyser>(argc, argv);
    auto parser = std::make_shared<Parser>(syntaxAnalyser);
    auto executor = std::make_shared<Executor>(parser);

    try {
        executor->execute();
    } catch (std::exception &e) {
        errorFound = true;
    }

    BOOST_CHECK_EQUAL(errorFound, true);
}

BOOST_AUTO_TEST_CASE(SIGN_ERROR_1) {

    {
        bool errorFound = false;
        std::vector<std::string> input{
                "program",
                "sign",
                "/tmp/sign_error_1_file.txt",
                "/tmp/sign_error_1_private.pem",
                "/tmp/sign_error_1_signature.txt",
        };
        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto syntaxAnalyser = std::make_shared<SyntaxAnalyser>(argc, argv);
        auto parser = std::make_shared<Parser>(syntaxAnalyser);
        auto executor = std::make_shared<Executor>(parser);

        executor->execute();
        int remove_return = int(executor->getResult());
        BOOST_CHECK_EQUAL(remove_return, int(CallResult::TRIED_TO_READ_NOT_EXISTING_PRV_KEY));
    }
}

BOOST_AUTO_TEST_CASE(SIGN_ERROR_2)
{
    system("mv ~/.keyPartition ~/.keyPartition.old");
    {
        std::vector<std::string> input{
                "program",
                "create-key",
                "RSA",
                "2048",
                "/tmp/sign_error_2_public.pem",
                "/tmp/sign_error_2_private.pem",
        };
        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto syntaxAnalyser = std::make_shared<SyntaxAnalyser>(argc, argv);
        auto parser = std::make_shared<Parser>(syntaxAnalyser);
        auto executor = std::make_shared<Executor>(parser);
        executor->execute();
        auto keysInPartition = executor->getCurrentInterface()->getCurrentKeyNum();
    }
    {
        system("echo a >> /home/robert/Desktop/sign_error_2_file.txt");
        bool errorFound = false;
        std::vector<std::string> input{
                "program",
                "sign",
                "/tmp/sign_error_2_file.txt",
                "/tmp/sign_error_2_private.pem",
                "/tmp/sign_error_2_signature.txt",
        };
        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto syntaxAnalyser = std::make_shared<SyntaxAnalyser>(argc, argv);
        auto parser = std::make_shared<Parser>(syntaxAnalyser);
        auto executor = std::make_shared<Executor>(parser);

        executor->execute();
        int remove_return = int(executor->getResult());
        BOOST_CHECK_EQUAL(remove_return, int(CallResult::SIGN_SUCCESS));
    }
    system("mv ~/.keyPartition.old ~/.keyPartition");
}

BOOST_AUTO_TEST_CASE(SIGN_ERROR_3)
{
    system("mv ~/.keyPartition ~/.keyPartition.old");
    {
        std::vector<std::string> input{
                "program",
                "create-key",
                "RSA",
                "2048",
                "/tmp/sign_error_3_public.pem",
                "/tmp/sign_error_3_private.pem",
        };
        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto syntaxAnalyser = std::make_shared<SyntaxAnalyser>(argc, argv);
        auto parser = std::make_shared<Parser>(syntaxAnalyser);
        auto executor = std::make_shared<Executor>(parser);
        executor->execute();
        auto keysInPartition = executor->getCurrentInterface()->getCurrentKeyNum();
    }
    {
        system("echo 2 >> /tmp/sign_error_3_private.pem");
        bool errorFound = false;
        std::vector<std::string> input{
                "program",
                "sign",
                "/tmp/sign_error_3_file.txt",
                "/tmp/sign_error_3_private.pem",
                "/tmp/sign_error_3_signature.txt",
        };
        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto syntaxAnalyser = std::make_shared<SyntaxAnalyser>(argc, argv);
        auto parser = std::make_shared<Parser>(syntaxAnalyser);
        auto executor = std::make_shared<Executor>(parser);

        executor->execute();
        int remove_return = int(executor->getResult());
        BOOST_CHECK_EQUAL(remove_return, int(CallResult::TRIED_TO_READ_NOT_EXISTING_PRV_KEY));
    }
    system("mv ~/.keyPartition.old ~/.keyPartition");
}

BOOST_AUTO_TEST_CASE(SIGN_ERROR_4)
{
    system("mv ~/.keyPartition ~/.keyPartition.old");
    {
        std::vector<std::string> input{
                "program",
                "create-key",
                "RSA",
                "2048",
                "/tmp/sing_error_4_public.pem",
                "/tmp/sing_error_4_private.pem",
        };
        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto syntaxAnalyser = std::make_shared<SyntaxAnalyser>(argc, argv);
        auto parser = std::make_shared<Parser>(syntaxAnalyser);
        auto executor = std::make_shared<Executor>(parser);
        executor->execute();
        auto keysInPartition = executor->getCurrentInterface()->getCurrentKeyNum();
    }
    {
        system("echo a >> /tmp/sign_error_4_signature.txt");
        bool errorFound = false;
        std::vector<std::string> input{
                "program",
                "sign",
                "/tmp/sign_error_4_file.txt",
                "/tmp/sign_error_4_private.pem",
                "/tmp/sign_error_4_signature.txt",
        };
        TerminalEmulation terminalEmulation(input);
        auto emulatedTerminalArgs = terminalEmulation.getArgs();
        auto argc = emulatedTerminalArgs.argc;
        auto argv = emulatedTerminalArgs.argv;

        auto syntaxAnalyser = std::make_shared<SyntaxAnalyser>(argc, argv);
        auto parser = std::make_shared<Parser>(syntaxAnalyser);
        auto executor = std::make_shared<Executor>(parser);

        executor->execute();
        int remove_return = int(executor->getResult());
        BOOST_CHECK_EQUAL(remove_return, int(CallResult::TRIED_TO_READ_NOT_EXISTING_PRV_KEY));
    }
    system("mv ~/.keyPartition.old ~/.keyPartition");
}

BOOST_AUTO_TEST_CASE(CHECK_SIGNATURE_WRONG_FILE)
{
    system("mv ~/.keyPartition ~/.keyPartition.old");
    for (int i = 0; i < 1; i++) {
        {
            std::vector<std::string> input{
                    "program",
                    "create-key",
                    "RSA",
                    "2048",
                    "/tmp/CHECK_SIGNATURE_WRONG_FILE_public.pem",
                    "/tmp/CHECK_SIGNATURE_WRONG_FILE_private.pem",
            };
            TerminalEmulation terminalEmulation(input);
            auto emulatedTerminalArgs = terminalEmulation.getArgs();
            auto argc = emulatedTerminalArgs.argc;
            auto argv = emulatedTerminalArgs.argv;

            auto syntaxAnalyser = std::make_shared<SyntaxAnalyser>(argc, argv);
            auto parser = std::make_shared<Parser>(syntaxAnalyser);
            auto executor = std::make_shared<Executor>(parser);
            executor->execute();
        }
        {
            std::vector<std::string> input{
                    "program",
                    "sign",
                    "/tmp/CHECK_SIGNATURE_WRONG_FILE_file.txt",
                    "/tmp/CHECK_SIGNATURE_WRONG_FILE_private.pem",
                    "/tmp/CHECK_SIGNATURE_WRONG_FILE_signature.txt",
            };

            TerminalEmulation terminalEmulation(input);
            auto emulatedTerminalArgs = terminalEmulation.getArgs();
            auto argc = emulatedTerminalArgs.argc;
            auto argv = emulatedTerminalArgs.argv;

            auto syntaxAnalyser = std::make_shared<SyntaxAnalyser>(argc, argv);
            auto parser = std::make_shared<Parser>(syntaxAnalyser);
            auto executor = std::make_shared<Executor>(parser);
            executor->execute();
        }
        {
            system("echo a >> /tmp/CHECK_SIGNATURE_WRONG_FILE_file.txt");
            std::vector<std::string> input{
                    "program",
                    "check-signature",
                    "/tmp/CHECK_SIGNATURE_WRONG_FILE_file.txt",
                    "/tmp/CHECK_SIGNATURE_WRONG_FILE_public.pem",
                    "/tmp/CHECK_SIGNATURE_WRONG_FILE_signature.txt",
            };

            TerminalEmulation terminalEmulation(input);
            auto emulatedTerminalArgs = terminalEmulation.getArgs();
            auto argc = emulatedTerminalArgs.argc;
            auto argv = emulatedTerminalArgs.argv;

            auto syntaxAnalyser = std::make_shared<SyntaxAnalyser>(argc, argv);
            auto parser = std::make_shared<Parser>(syntaxAnalyser);
            auto executor = std::make_shared<Executor>(parser);
            executor->execute();

            int executorResult = int(executor->getResult());
            BOOST_CHECK_EQUAL(executorResult, int(CallResult::SIGNATURE_NOT_THE_SAME));
        }
    }
    system("mv ~/.keyPartition.old ~/.keyPartition");
}

BOOST_AUTO_TEST_CASE(CHECK_SIGNATURE_WRONG_PUB)
{
    system("mv ~/.keyPartition ~/.keyPartition.old");
    for (int i = 0; i < 1; i++) {
        {
            std::vector<std::string> input{
                    "program",
                    "create-key",
                    "RSA",
                    "2048",
                    "/tmp/CHECK_SIGNATURE_WRONG_PUB_public.pem",
                    "/tmp/CHECK_SIGNATURE_WRONG_PUB_private.pem",
            };
            TerminalEmulation terminalEmulation(input);
            auto emulatedTerminalArgs = terminalEmulation.getArgs();
            auto argc = emulatedTerminalArgs.argc;
            auto argv = emulatedTerminalArgs.argv;

            auto syntaxAnalyser = std::make_shared<SyntaxAnalyser>(argc, argv);
            auto parser = std::make_shared<Parser>(syntaxAnalyser);
            auto executor = std::make_shared<Executor>(parser);
            executor->execute();
        }
        {
            std::vector<std::string> input{
                    "program",
                    "sign",
                    "/tmp/CHECK_SIGNATURE_WRONG_PUB_file.txt",
                    "/tmp/CHECK_SIGNATURE_WRONG_PUB_private.pem",
                    "/tmp/CHECK_SIGNATURE_WRONG_PUB_signature.txt",
            };

            TerminalEmulation terminalEmulation(input);
            auto emulatedTerminalArgs = terminalEmulation.getArgs();
            auto argc = emulatedTerminalArgs.argc;
            auto argv = emulatedTerminalArgs.argv;

            auto syntaxAnalyser = std::make_shared<SyntaxAnalyser>(argc, argv);
            auto parser = std::make_shared<Parser>(syntaxAnalyser);
            auto executor = std::make_shared<Executor>(parser);
            executor->execute();
        }
        {
            std::string fileToChange = "/tmp/CHECK_SIGNATURE_WRONG_PUB_public.pem";
            size_t fileSize = getFileSize(fileToChange.c_str());
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
            assert(ret == 0);
            close(fd);

            std::vector<std::string> input{
                    "program",
                    "check-signature",
                    "/tmp/CHECK_SIGNATURE_WRONG_PUB_file.txt",
                    "/tmp/CHECK_SIGNATURE_WRONG_PUB_public.pem",
                    "/tmp/CHECK_SIGNATURE_WRONG_PUB_signature.txt",
            };

            TerminalEmulation terminalEmulation(input);
            auto emulatedTerminalArgs = terminalEmulation.getArgs();
            auto argc = emulatedTerminalArgs.argc;
            auto argv = emulatedTerminalArgs.argv;

            auto syntaxAnalyser = std::make_shared<SyntaxAnalyser>(argc, argv);
            auto parser = std::make_shared<Parser>(syntaxAnalyser);
            auto executor = std::make_shared<Executor>(parser);
            executor->execute();

            int executorResult = int(executor->getResult());
            BOOST_CHECK_EQUAL(executorResult, int(CallResult::SIGNATURE_NOT_THE_SAME));
        }
    }
    system("mv ~/.keyPartition.old ~/.keyPartition");
}

BOOST_AUTO_TEST_CASE(CHECK_SIGNATURE_WRONG_SIGNATURE)
{
    system("mv ~/.keyPartition ~/.keyPartition.old");
    for (int i = 0; i < 1; i++) {
        {
            std::vector<std::string> input{
                    "program",
                    "create-key",
                    "RSA",
                    "2048",
                    "/tmp/CHECK_SIGNATURE_WRONG_SIGNATURE_public.pem",
                    "/tmp/CHECK_SIGNATURE_WRONG_SIGNATURE_private.pem",
            };
            TerminalEmulation terminalEmulation(input);
            auto emulatedTerminalArgs = terminalEmulation.getArgs();
            auto argc = emulatedTerminalArgs.argc;
            auto argv = emulatedTerminalArgs.argv;

            auto syntaxAnalyser = std::make_shared<SyntaxAnalyser>(argc, argv);
            auto parser = std::make_shared<Parser>(syntaxAnalyser);
            auto executor = std::make_shared<Executor>(parser);
            executor->execute();
        }
        {
            std::vector<std::string> input{
                    "program",
                    "sign",
                    "/tmp/CHECK_SIGNATURE_WRONG_SIGNATURE_file.txt",
                    "/tmp/CHECK_SIGNATURE_WRONG_SIGNATURE_private.pem",
                    "/tmp/CHECK_SIGNATURE_WRONG_SIGNATURE_signature.txt",
            };

            TerminalEmulation terminalEmulation(input);
            auto emulatedTerminalArgs = terminalEmulation.getArgs();
            auto argc = emulatedTerminalArgs.argc;
            auto argv = emulatedTerminalArgs.argv;

            auto syntaxAnalyser = std::make_shared<SyntaxAnalyser>(argc, argv);
            auto parser = std::make_shared<Parser>(syntaxAnalyser);
            auto executor = std::make_shared<Executor>(parser);
            executor->execute();
        }
        {
            system("echo a >> /tmp/CHECK_SIGNATURE_WRONG_SIGNATURE_signature.txt");
            std::vector<std::string> input{
                    "program",
                    "check-signature",
                    "/tmp/CHECK_SIGNATURE_WRONG_SIGNATURE_file.txt",
                    "/tmp/CHECK_SIGNATURE_WRONG_SIGNATURE_public.pem",
                    "/tmp/CHECK_SIGNATURE_WRONG_SIGNATURE_signature.txt",
            };

            TerminalEmulation terminalEmulation(input);
            auto emulatedTerminalArgs = terminalEmulation.getArgs();
            auto argc = emulatedTerminalArgs.argc;
            auto argv = emulatedTerminalArgs.argv;

            auto syntaxAnalyser = std::make_shared<SyntaxAnalyser>(argc, argv);
            auto parser = std::make_shared<Parser>(syntaxAnalyser);
            auto executor = std::make_shared<Executor>(parser);
            executor->execute();

            int executorResult = int(executor->getResult());
            BOOST_CHECK_EQUAL(executorResult, int(CallResult::SIGNATURE_NOT_THE_SAME));
        }
    }
    system("mv ~/.keyPartition.old ~/.keyPartition");
}
BOOST_AUTO_TEST_CASE(GET_PRIVATE_KEY_WRONG_ID)
{
    std::vector<std::string> input {
            "program",
            "get-private-key",
            "11111111111111111",
            "/home/abc",
    };
    TerminalEmulation terminalEmulation(input);
    auto args = terminalEmulation.getArgs();
    auto argc = args.argc;
    auto argv = args.argv;

    auto syntaxAnalyser = std::make_shared<SyntaxAnalyser>(argc, argv);
    auto parser = std::make_shared<Parser>(syntaxAnalyser);
    auto executor = std::make_shared<Executor>(parser);

    bool error_caught = false;
    try {
        executor->execute();
    } catch (std::exception &e) {

        if(std::string(e.what()) != "Not implemented") {
            error_caught = true;
        }
    }
    BOOST_CHECK_EQUAL(error_caught, true);
}
BOOST_AUTO_TEST_CASE(OVERWRITE_FLAG_CREATE_KEY_TEST)
{
    std::vector<std::string> input {
            "program",
            "create-key",
            "SHA",
            "2048",
            "/home/r.ostoja",
            "/home/r.ostoja",
            "overwrite",
    };
    TerminalEmulation terminalEmulation(input);
    auto args = terminalEmulation.getArgs();
    auto argc = args.argc;
    auto argv = args.argv;

    auto syntaxAnalyser = std::make_shared<SyntaxAnalyser>(argc, argv);
    auto parser = std::make_unique<Parser>(syntaxAnalyser);
    parser->parse();
    auto statement = parser->getCurrentParsedStatementStr();
    auto serialisedInput = testHelpers::toString(input);
    BOOST_CHECK_EQUAL(statement, serialisedInput);
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

    auto syntaxAnalyser = std::make_shared<SyntaxAnalyser>(argc, argv);
    auto parser = std::make_unique<Parser>(syntaxAnalyser);
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
            "/home/abc",
            "312321",
            "outpath",
            "overwrite",
    };
    TerminalEmulation terminalEmulation(input);
    auto args = terminalEmulation.getArgs();
    auto argc = args.argc;
    auto argv = args.argv;

    auto syntaxAnalyser = std::make_shared<SyntaxAnalyser>(argc, argv);
    auto parser = std::make_unique<Parser>(syntaxAnalyser);
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

    auto syntaxAnalyser = std::make_shared<SyntaxAnalyser>(argc, argv);
    auto parser = std::make_unique<Parser>(syntaxAnalyser);
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

    auto syntaxAnalyser = std::make_shared<SyntaxAnalyser>(argc, argv);
    auto parser = std::make_unique<Parser>(syntaxAnalyser);
    parser->parse();
    auto statement = parser->getCurrentParsedStatementStr();
    auto serialisedInput = testHelpers::toString(input);
    BOOST_CHECK_EQUAL(statement, serialisedInput);
}