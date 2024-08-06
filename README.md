# DAG-Rider

A C++ implementation of [DAG-Rider](https://arxiv.org/pdf/2102.08325), an asynchronous Byzantine Atomic Broadcast protocol.

Using C++23 compiled with clang/llvm 18.1.8

## Overview

## Building

## Running

- `./Node --id 1 --address 127.0.0.1 --port 12345`

## Dependencies

- [Boost.Asio](https://www.boost.org/doc/libs/1_85_0/doc/html/boost_asio.html)
- [Boost.Beast](https://www.boost.org/doc/libs/1_85_0/libs/beast/doc/html/index.html)
- [nlohmann/json](https://github.com/nlohmann/json)
- [blake3](https://github.com/BLAKE3-team/BLAKE3)
- [ed25519-donna](https://github.com/floodyberry/ed25519-donna/tree/master)
