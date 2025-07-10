cmake_minimum_required(VERSION 3.15)
project(BasicWebsocketEmailVerifier C CXX)

# ========================
# Compiler & Conan Setup
# ========================
include(${CMAKE_BINARY_DIR}/conan_paths.cmake OPTIONAL)
list(PREPEND CMAKE_PREFIX_PATH "${CMAKE_BINARY_DIR}")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ========================
# Dependencies
# ========================
find_package(Protobuf REQUIRED)
find_package(gRPC REQUIRED)
find_package(uwebsockets REQUIRED)
find_package(websocketpp REQUIRED)
find_package(RapidJSON REQUIRED)

# ========================
# gRPC + Proto Generation
# ========================
set(PROTO_FILE ${CMAKE_CURRENT_SOURCE_DIR}/proto/email_verifier.proto)
set(GENERATED_PROTO_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/models/proto_models/email_verifier_model)
file(MAKE_DIRECTORY ${GENERATED_PROTO_DIR})

set(PROTO_SRC "${GENERATED_PROTO_DIR}/email_verifier.pb.cc")
set(PROTO_HDR "${GENERATED_PROTO_DIR}/email_verifier.pb.h")
set(GRPC_SRC "${GENERATED_PROTO_DIR}/email_verifier.grpc.pb.cc")
set(GRPC_HDR "${GENERATED_PROTO_DIR}/email_verifier.grpc.pb.h")

find_program(GRPC_CPP_PLUGIN_EXECUTABLE
  NAMES grpc_cpp_plugin
  REQUIRED
)

add_custom_command(
  OUTPUT ${PROTO_SRC} ${PROTO_HDR}
  COMMAND ${Protobuf_PROTOC_EXECUTABLE}
  ARGS --cpp_out=${GENERATED_PROTO_DIR}
       -I ${CMAKE_CURRENT_SOURCE_DIR}/proto
       ${PROTO_FILE}
  DEPENDS ${PROTO_FILE}
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
  DEPENDS ${GENERATED_SRCS} ${GENERATED_HDRS}
)

# ========================
# Include directories
# ========================
include_directories(
  ${GENERATED_PROTO_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/src/models/proto_models/email_verifier_model
)

# ========================
# Server Executable
# ========================
add_executable(server
  src/app/server_main.cpp
  src/controllers/grpc_service.cpp
  src/handlers/email_handler.cpp
  src/adapters/websocket_server/websocket_server.cpp
  src/globals/email_storage.cpp
  src/utils/json_utils.cpp
  src/threads/grpc_websocket_threads.cpp
  ${GENERATED_SRCS}
)

add_dependencies(server proto_gen)

target_link_libraries(server PRIVATE
  gRPC::grpc++
  protobuf::protobuf
  uwebsockets::uwebsockets
  websocketpp::websocketpp
  rapidjson
)

# ========================
# Client Executable
# ========================
add_executable(client
  src/app/client_main.cpp
  src/client/email_verifier_client.cpp
  ${GENERATED_SRCS}
)

add_dependencies(client proto_gen)

target_include_directories(client PRIVATE
  ${GENERATED_PROTO_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/src/client
  ${CMAKE_CURRENT_SOURCE_DIR}/src/models/proto_models/email_verifier_model
)


target_link_libraries(client PRIVATE
  gRPC::grpc++
  protobuf::protobuf
  uwebsockets::uwebsockets
  websocketpp::websocketpp
  rapidjson
)
