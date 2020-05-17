set (Boost_USE_STATIC_LIBS OFF)
find_package (Boost REQUIRED COMPONENTS unit_test_framework)
include_directories (${Boost_INCLUDE_DIRS})

# 'Boost_Tests_run' is the target name
# 'test1.cpp tests2.cpp' are source files with tests
add_executable (tests IntegrationTests.cpp  ../source/Parser.cpp ../source/Statement.cpp ../source/SyntaxAnalyser.cpp)
target_link_libraries (tests ${Boost_LIBRARIES})