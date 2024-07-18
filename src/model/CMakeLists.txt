cmake_minimum_required(VERSION 3.10)

project(Consensus VERSION 1.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(Boost REQUIRED COMPONENTS system)

include_directories(~/Workspace/dagrider/src/model/include)  # nlohmann/json and blake3.h are here

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
    /usr/local/lib/libblake3.a
    Boost::system
)

cmake_policy(SET CMP0167 NEW)
