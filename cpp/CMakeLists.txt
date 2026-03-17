cmake_minimum_required(VERSION 3.10)
project(grpc_demo CXX)

# C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找 gRPC
find_package(Protobuf REQUIRED)
find_package(grpc REQUIRED)

# 包含目录
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_SOURCE_DIR}/../proto)

# 生成的 proto 文件
set(PROTO_SOURCES
    ${CMAKE_CURRENT_BINARY_DIR}/inference.pb.cc
    ${CMAKE_CURRENT_BINARY_DIR}/inference.grpc.pb.cc
)

# 源文件
set(SOURCES
    server.cpp
    ${PROTO_SOURCES}
)

# 可执行文件
add_executable(server ${SOURCES})

# 链接库
target_link_libraries(server
    grpc++
    grpc++_reflection
    protobuf
)

# 安装
install(TARGETS server RUNTIME DESTINATION bin)
