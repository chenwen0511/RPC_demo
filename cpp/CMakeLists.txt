cmake_minimum_required(VERSION 3.10)
project(grpc_demo CXX)

# C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找 gRPC
find_package(Protobuf REQUIRED)
find_package(gRPC CONFIG REQUIRED)

# proto 文件路径
set(PROTO_FILE ${CMAKE_SOURCE_DIR}/../proto/inference.proto)

# 生成的 proto 输出（放到 build 目录）
set(PROTO_SRC ${CMAKE_CURRENT_BINARY_DIR}/inference.pb.cc)
set(PROTO_HDR ${CMAKE_CURRENT_BINARY_DIR}/inference.pb.h)
set(GRPC_SRC  ${CMAKE_CURRENT_BINARY_DIR}/inference.grpc.pb.cc)
set(GRPC_HDR  ${CMAKE_CURRENT_BINARY_DIR}/inference.grpc.pb.h)

get_target_property(GRPC_CPP_PLUGIN_EXECUTABLE gRPC::grpc_cpp_plugin LOCATION)

add_custom_command(
    OUTPUT ${PROTO_SRC} ${PROTO_HDR} ${GRPC_SRC} ${GRPC_HDR}
    COMMAND ${Protobuf_PROTOC_EXECUTABLE}
            -I ${CMAKE_SOURCE_DIR}/../proto
            --cpp_out=${CMAKE_CURRENT_BINARY_DIR}
            --grpc_out=${CMAKE_CURRENT_BINARY_DIR}
            --plugin=protoc-gen-grpc=${GRPC_CPP_PLUGIN_EXECUTABLE}
            ${PROTO_FILE}
    DEPENDS ${PROTO_FILE}
    COMMENT "Generating gRPC / Protobuf sources"
    VERBATIM
)

add_custom_target(proto_gen DEPENDS ${PROTO_SRC} ${GRPC_SRC})

# 包含目录
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_SOURCE_DIR}/../proto)

# 源文件
set(SOURCES
    server.cpp
    ${PROTO_SRC}
    ${GRPC_SRC}
)

# 可执行文件
add_executable(server ${SOURCES})
add_dependencies(server proto_gen)

# 链接库
target_link_libraries(server
    gRPC::grpc++
    gRPC::grpc++_reflection
    protobuf::libprotobuf
)

# 安装
install(TARGETS server RUNTIME DESTINATION bin)
