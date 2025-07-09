# Basic WebSocket + gRPC Integration (C++)

This project demonstrates a simple integration between **WebSocket** (using [uWebSockets](https://github.com/uNetworking/uWebSockets)) and **gRPC** in C++. The goal is to stream or respond to gRPC-based data via WebSocket to connected clients. This can be useful in real-time systems where backend services communicate over gRPC, but data needs to be pushed to browsers or WebSocket-enabled clients.

---

## 🔧 Requirements

- **Conan** version: 2.x  
- **CMake** version: ≥ 3.15  
- **g++** or any modern C++ compiler with C++17 support

---

## 🚀 Getting Started

Clone the repository:

```bash
git clone git@github.com:MahardikaPratama/basic-websocket-cpp.git
cd basic-websocket-cpp
```

Build and install dependencies using Conan and CMake:

```bash
mkdir build && cd build
conan install .. --build=missing
cmake .. \
  -DCMAKE_TOOLCHAIN_FILE=Release/generators/conan_toolchain.cmake \
  -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

Run the Server
After building, run the WebSocket + gRPC server:

```bash
./server
```

Run the gRPC client (for testing or streaming input):
```bash
./client
```
You can then connect to the WebSocket server using any WebSocket client (browser, Postman, or CLI tool) and observe gRPC-driven data being transmitted in real-time.

### Notes
- The project uses proto/emailverifier.proto to define the gRPC service.
- Ensure protoc and the gRPC plugin are available during build if extending the proto definitions.
- Do not modify the folder structure unless necessary, as build paths rely on the current layout.
