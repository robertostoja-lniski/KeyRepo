#message(FATAL_ERROR ${CMAKE_C_FLAGS})
cmake_minimum_required(VERSION 3.10)
set (Boost_USE_STATIC_LIBS OFF)
find_package (Boost REQUIRED COMPONENTS unit_test_framework)
include_directories (${Boost_INCLUDE_DIRS})
add_executable(KeyRepo main.cpp Lexer.cpp Parser.cpp Statement.cpp Executor.cpp KeyIOInterfaces.cpp OpenSSLHandler.cpp TimeCalc.cpp KernelEmulation.c KeyRepoSyscallWrapper.c)

target_link_libraries (KeyRepo ${Boost_LIBRARIES})


