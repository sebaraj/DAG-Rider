# DAG-Rider

An non-production-grade implementation of [DAG-Rider](https://arxiv.org/pdf/2102.08325), an asynchronous Byzantine Atomic Broadcast protocol, written in modern C++23, compiled with clang/llvm 18.1.8. At the time of writing and to the best of my knowledge, this is the only publically available C++ implementation.

The goal of this initial implementation was to practice implementing a modern consensus protocol directly from a white paper, rapidly improve my C++ skills, and explore the nuances of this BAB protocol. I plan to drastically refactor this codebase into a performant, production-grade BAB layer after completing CPSC 426 - Building Distributed Systems at Yale this December.

### Build + Run

- To build and run a sample application which utilizes DAG-Rider for BAB, run the following commands in the root directory of this project:
- `mkdir src/node/build/`
- `cmake src/node/ src/node/build/`
- `cmake --build ./src/node/build/`
- `src/node/build/Node --id 1 --address 127.0.0.1 --port 12345`

### Dependencies

- [Boost.Asio](https://www.boost.org/doc/libs/1_85_0/doc/html/boost_asio.html)
- [Boost.Beast](https://www.boost.org/doc/libs/1_85_0/libs/beast/doc/html/index.html)
- [Boost.System](https://www.boost.org/doc/libs/1_62_0/libs/system/doc/index.html)
- [Boost.Program_options](https://www.boost.org/doc/libs/1_85_0/doc/html/program_options.html)
- [nlohmann/json](https://github.com/nlohmann/json)
- [blake3](https://github.com/BLAKE3-team/BLAKE3)
- [ed25519-donna](https://github.com/floodyberry/ed25519-donna/tree/master)

### To-Do

- Drastic refactor about CPSC 426
- Switch from json serialization/deserialization to gRPC/protobufs
