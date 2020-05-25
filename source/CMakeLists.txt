#message(FATAL_ERROR ${CMAKE_C_FLAGS})
add_executable(KeyRepo main.cpp SyntaxAnalyser.cpp Parser.cpp Statement.cpp Executor.cpp KeyIOInterfaces.cpp OpenSSLHandler.cpp)
