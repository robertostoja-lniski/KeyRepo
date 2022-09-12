cmake_minimum_required(VERSION 3.10)
project(KeyRepo)

set(CMAKE_CXX_STANDARD 17)

if(APPLE)
    set(CMAKE_SHARED_LINKER_FLAGS "-L/usr/local/opt/openssl@1.1/lib -I/usr/local/opt/openssl@1.1/include -lcrypto")
    set(CMAKE_CXX_STANDARD_LIBRARIES "-L/usr/local/opt/openssl@1.1/lib -I/usr/local/opt/openssl@1.1/include -lcrypto")
else()
    set(CMAKE_SHARED_LINKER_FLAGS "-lcrypto -lstdc++fs -lboost_system")
    set(CMAKE_CXX_STANDARD_LIBRARIES "-lstdc++fs -lcrypto -lboost_system")
endif()
#set(CMAKE_CXX_FLAGS "-lcrypto")
#set(CMAKE_C_FLAGS "-lcrypto")
#message(WARNING ${CMAKE_LINKER)
add_subdirectory(source)
add_subdirectory(tests)
#add_executable(KeyRepo main.cpp)

add_library(key_repo_lib SHARED
        source/KernelEmulation.c
        source/KeyRepoSyscallWrapper.c
        )
# For real partition -- change in include file EMULATION to 0
# add_library(key_repo_lib SHARED
#         source/KeyRepoSyscallWrapper.c
#         )

set_target_properties(key_repo_lib PROPERTIES SOVERSION 1)
include(GNUInstallDirs)

install(TARGETS key_repo_lib
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

add_library(lib_repo_static STATIC
        source/KernelEmulation.c
        source/KeyRepoSyscallWrapper.c
        )



