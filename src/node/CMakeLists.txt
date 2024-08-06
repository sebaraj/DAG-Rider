set(CMAKE_C_COMPILER "/opt/homebrew/Cellar/llvm/18.1.8/bin/clang")
set(CMAKE_CXX_COMPILER "/opt/homebrew/Cellar/llvm/18.1.8/bin/clang++")

cmake_minimum_required(VERSION 3.10)
project(Node)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -g -O3 -stdlib=libc++")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++ -L/opt/homebrew/Cellar/llvm/18.1.8/lib/c++")

set(CMAKE_OSX_ARCHITECTURES "arm64")

find_package(Boost REQUIRED COMPONENTS program_options system filesystem thread)

include_directories(${Boost_INCLUDE_DIRS})
include_directories(/opt/homebrew/opt/openssl@3/include)
include_directories(${CMAKE_SOURCE_DIR}/../../dependencies)
include_directories(../model)
include_directories(../network)
include_directories(../transaction)
include_directories(../consensus)
include_directories(../vertex)


add_executable(Node 
    main.cpp 
    client.cpp
    ../model/block.cpp
    ../model/committee.cpp
    ../model/validator.cpp
    ../model/vertex.cpp
    ../network/broadcaster.cpp
    ../transaction/coordinator.cpp
    ../transaction/block_builder.cpp
    ../consensus/consensus.cpp
    ../consensus/state.cpp
    ../consensus/dag.cpp
    ../vertex/vertex_broadcaster.cpp
    ../vertex/vertex_coordinator.cpp
    ../vertex/vertex_message_handler.cpp
)

target_link_libraries(Node 
    dl 
    ${PROJECT_SOURCE_DIR}/../../lib/libblake3.a  # Path to blake3 library
    ${PROJECT_SOURCE_DIR}/../../lib/libed25519.a  # Path to ed25519-donna library
    /opt/homebrew/opt/openssl@3/lib/libcrypto.a     
    Boost::system
    Boost::filesystem
    Boost::thread
    Boost::program_options
    c++
    c++abi
    pthread
)

cmake_policy(SET CMP0167 NEW)

