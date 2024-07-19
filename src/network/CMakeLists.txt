set(CMAKE_C_COMPILER "/opt/homebrew/Cellar/llvm/18.1.8/bin/clang")
set(CMAKE_CXX_COMPILER "/opt/homebrew/Cellar/llvm/18.1.8/bin/clang++")

cmake_minimum_required(VERSION 3.10)

project(Network VERSION 1.0)

#set(CMAKE_C_COMPILER "/opt/homebrew/Cellar/gcc/14.1.0_1/bin/gcc-14")
#set(CMAKE_CXX_COMPILER "/opt/homebrew/Cellar/gcc/14.1.0_1/bin/g++-14")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -g -O3 -stdlib=libc++") # -fsanitize=address 
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++ -L/opt/homebrew/Cellar/llvm/18.1.8/lib/c++")

set(CMAKE_OSX_ARCHITECTURES "arm64")

find_package(Boost REQUIRED COMPONENTS system)

set(SOURCES
    main.cpp
    error.cpp
    broadcaster.cpp
)

add_executable(network ${SOURCES})

target_link_libraries(network
    pthread
    Boost::system
    c++
    c++abi
)

cmake_policy(SET CMP0167 NEW)

