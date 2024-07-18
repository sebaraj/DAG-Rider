cmake_minimum_required(VERSION 3.10)

project(Consensus VERSION 1.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_OSX_ARCHITECTURES "arm64")

find_package(Boost REQUIRED COMPONENTS system)

include_directories(~/Workspace/dagrider/src/model/include)  # nlohmann/json and blake3.h are here
include_directories(/opt/homebrew/opt/openssl@3/include)

set(SOURCES
    block.cpp
    validator.cpp
    committee.cpp
    vertex.cpp
    main.cpp
)

add_executable(consensus ${SOURCES})

target_link_libraries(consensus
    pthread
    dl
    ${PROJECT_SOURCE_DIR}/lib/libblake3.a  # Path to blake3 library
    ${PROJECT_SOURCE_DIR}/lib/libed25519.a  # Path to ed25519-donna library
    /opt/homebrew/opt/openssl@3/lib/libcrypto.a    
    Boost::system
)

cmake_policy(SET CMP0167 NEW)
