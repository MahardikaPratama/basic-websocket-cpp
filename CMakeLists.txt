cmake_minimum_required(VERSION 3.15)
project(BasicGrpcEmailVerifier C CXX)

# Conan path config + fix for CMake to find RapidJSON, etc.
include(${CMAKE_BINARY_DIR}/conan_paths.cmake OPTIONAL)
list(PREPEND CMAKE_PREFIX_PATH "${CMAKE_BINARY_DIR}")

# Compiler config
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Dependencies from Conan
find_package(Protobuf REQUIRED)
find_package(gRPC REQUIRED)
find_package(uwebsockets REQUIRED)
find_package(websocketpp REQUIRED)
find_package(RapidJSON REQUIRED)

# .proto file handling
set(PROTO_FILE ${CMAKE_CURRENT_SOURCE_DIR}/proto/emailverifier.proto)
set(GENERATED_PROTO_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated)
file(MAKE_DIRECTORY ${GENERATED_PROTO_DIR})

set(PROTO_SRC "${GENERATED_PROTO_DIR}/emailverifier.pb.cc")
set(PROTO_HDR "${GENERATED_PROTO_DIR}/emailverifier.pb.h")
set(GRPC_SRC "${GENERATED_PROTO_DIR}/emailverifier.grpc.pb.cc")
set(GRPC_HDR "${GENERATED_PROTO_DIR}/emailverifier.grpc.pb.h")

add_custom_command(
  OUTPUT ${PROTO_SRC} ${PROTO_HDR}
  COMMAND ${Protobuf_PROTOC_EXECUTABLE}
  ARGS --cpp_out ${GENERATED_PROTO_DIR}
       -I ${CMAKE_CURRENT_SOURCE_DIR}/proto
       ${PROTO_FILE}
  DEPENDS ${PROTO_FILE}
)

find_program(GRPC_CPP_PLUGIN_EXECUTABLE
  NAMES grpc_cpp_plugin
  HINTS ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR}
        ${CONAN_BIN_DIRS_GRPC} ${CONAN_BIN_DIRS_PROTOBUF}
  PATHS ENV PATH
  REQUIRED
)

add_custom_command(
  OUTPUT ${GRPC_SRC} ${GRPC_HDR}
  COMMAND ${Protobuf_PROTOC_EXECUTABLE}
  ARGS --grpc_out=${GENERATED_PROTO_DIR}
       --plugin=protoc-gen-grpc=${GRPC_CPP_PLUGIN_EXECUTABLE}
       -I ${CMAKE_CURRENT_SOURCE_DIR}/proto
       ${PROTO_FILE}
  DEPENDS ${PROTO_FILE}
)

set(GENERATED_SRCS ${PROTO_SRC} ${GRPC_SRC})
set(GENERATED_HDRS ${PROTO_HDR} ${GRPC_HDR})

add_custom_target(proto_gen ALL
  DEPENDS ${PROTO_SRC} ${PROTO_HDR} ${GRPC_SRC} ${GRPC_HDR}
)

# Server executable
add_executable(server src/server.cpp ${GENERATED_SRCS})
add_dependencies(server proto_gen)
target_include_directories(server PRIVATE
  ${GENERATED_PROTO_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_libraries(server PRIVATE
  gRPC::grpc++
  protobuf::protobuf
  uwebsockets::uwebsockets
  websocketpp::websocketpp
  rapidjson
)

# Client executable
add_executable(client src/client.cpp ${GENERATED_SRCS})
add_dependencies(client proto_gen)
target_include_directories(client PRIVATE
  ${GENERATED_PROTO_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_libraries(client PRIVATE
  gRPC::grpc++
  protobuf::protobuf
  uwebsockets::uwebsockets
  websocketpp::websocketpp
  rapidjson
)
