#!/bin/bash
# 编译 C++ gRPC 服务端

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
CPP_DIR="$SCRIPT_DIR"

echo "=== gRPC C++ 编译 ==="

# 检查依赖
if ! command -v protoc &> /dev/null; then
    echo "❌ protoc 未安装"
    echo "   Ubuntu: sudo apt install protobuf-compiler-grpc"
    exit 1
fi

if ! command -v grpc_cpp_plugin &> /dev/null; then
    echo "❌ grpc_cpp_plugin 未安装"
    echo "   Ubuntu: sudo apt install grpc++"
    exit 1
fi

# 生成 proto 代码
echo "[1] 生成 proto 代码..."
cd "$CPP_DIR"

# 生成 C++ 代码
protoc -I../proto --grpc_out=. --plugin=protoc-gen-grpc=$(which grpc_cpp_plugin) ../proto/inference.proto
protoc -I../proto --cpp_out=. ../proto/inference.proto

echo "[2] 编译..."
g++ -std=c++17 \
    -I/usr/include \
    $(pkg-config --cflags protobuf grpc) \
    server.cpp \
    inference.pb.cc inference.grpc.pb.cc \
    $(pkg-config --libs protobuf grpc) \
    -o server

echo "✅ 编译成功!"
echo "   运行: ./server"
echo ""
echo "然后在 python 目录运行:"
echo "   cd ../python"
echo "   python3 client.py"
