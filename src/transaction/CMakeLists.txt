set(CMAKE_C_COMPILER "/opt/homebrew/Cellar/llvm/18.1.8/bin/clang")
set(CMAKE_CXX_COMPILER "/opt/homebrew/Cellar/llvm/18.1.8/bin/clang++")

cmake_minimum_required(VERSION 3.10)

project(Coordinator VERSION 1.0)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -g -O3 -stdlib=libc++")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++ -L/opt/homebrew/Cellar/llvm/18.1.8/lib/c++")

set(CMAKE_OSX_ARCHITECTURES "arm64")

find_package(Boost REQUIRED COMPONENTS system filesystem thread)

include_directories(${Boost_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/../../dependencies)
include_directories(${CMAKE_SOURCE_DIR}/../model)
include_directories(${CMAKE_SOURCE_DIR}/../network)
include_directories(${CMAKE_SOURCE_DIR}/../consensus)

set(SOURCES
    coordinator.cpp
    blockbuilder.cpp
    main.cpp
    ../model/block.cpp
    ../model/committee.cpp
    ../model/validator.cpp
    ../model/vertex.cpp
    ../network/broadcaster.cpp
)

add_executable(coordinator_executable ${SOURCES})

target_link_libraries(coordinator_executable
    pthread
    dl
    Boost::system
    Boost::filesystem
    Boost::thread
    c++
    c++abi
)

cmake_policy(SET CMP0167 NEW)
