cmake_minimum_required(VERSION 3.10)

project(Consensus VERSION 1.0)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -g -O3 -stdlib=libc++")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++ -L/opt/homebrew/Cellar/llvm/18.1.8/lib/c++")

set(CMAKE_OSX_ARCHITECTURES "arm64")

find_package(Boost REQUIRED COMPONENTS system filesystem thread)

include_directories(${Boost_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/../../dependencies)  # Adjust this path if necessary
include_directories(/opt/homebrew/opt/openssl@3/include)
include_directories(${PROJECT_SOURCE_DIR}/../model)
include_directories(${PROJECT_SOURCE_DIR}/../network)

set(SOURCES
    dag.cpp
    consensus.cpp
    state.cpp
    main.cpp
    ../model/committee.cpp  # Include the model sources
    ../model/vertex.cpp
    ../model/block.cpp
    ../model/validator.cpp
)

add_executable(consensus_executable ${SOURCES})

target_link_libraries(consensus_executable
    pthread
    dl
    ${PROJECT_SOURCE_DIR}/../../lib/libblake3.a
    ${PROJECT_SOURCE_DIR}/../../lib/libed25519.a
    /opt/homebrew/opt/openssl@3/lib/libcrypto.a
    Boost::system
    Boost::filesystem
    Boost::thread
    c++
    c++abi
)

cmake_policy(SET CMP0167 NEW)

