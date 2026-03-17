# gRPC 教程 - Python ↔ C++ 通信 Demo

## 概述

本项目演示 **Python 客户端** 与 **C++ 服务端** 之间的 gRPC 通信，是学习 gRPC 的入门教程。

## 架构

```
┌─────────────┐                           ┌─────────────┐
│   Python    │      gRPC (网络)          │     C++     │
│  (Client)   │ ─────────────────────────▶│  (Server)   │
│             │   InferRequest             │             │
│             │◀───────────────────────────│  TensorRT   │
│             │   InferResponse            │  (模拟推理) │
└─────────────┘                           └─────────────┘
```

## 学习路径

1. **第一步**: 阅读 [howto.md](./howto.md) 理解原理
2. **第二步**: 查看生成的 proto 代码
3. **第三步**: 运行 Python 客户端，观察输出
4. **第四步**: 阅读 C++ 服务端代码，理解服务注册

---

## 项目文件

```
RPC_demo/
├── README.md           # 本文件
├── howto.md          # 教程文档
├── proto/
│   └── inference.proto    # gRPC 接口定义
├── python/
│   ├── inference_pb2.py       # 生成的 Protobuf 代码
│   ├── inference_pb2_grpc.py  # 生成的 gRPC 代码
│   └── client.py          # Python 客户端
├── cpp/
│   ├── inference.pb.h     # 生成的 C++ 头文件
│   ├── inference.grpc.pb.h # 生成的 C++ gRPC 头文件
│   ├── server.cpp         # C++ 服务端
│   └── CMakeLists.txt     # 构建配置
└── build/                # 编译输出目录
```

---

## 快速开始

### 1. 安装依赖

```bash
# Python 依赖
pip install grpcio grpcio-tools

# C++ 依赖 (Ubuntu)
sudo apt install -y build-essential cmake grpc++ libprotobuf-dev protobuf-compiler-grpc
```

### 2. 生成 Proto 代码

```bash
# Python
cd python
python3 -m grpc_tools.protoc -I../proto --python_out=. --grpc_python_out=. ../proto/inference.proto

# C++ (在 cpp 目录执行)
protoc -I../proto --grpc_out=. --plugin=protoc-gen-grpc=$(which grpc_cpp_plugin) ../proto/inference.proto
protoc -I../proto --cpp_out=. ../proto/inference.proto
```

### 3. 编译 C++ 服务端

```bash
cd cpp
mkdir build && cd build
cmake ..
make -j4
```

### 4. 运行

```bash
# 终端 1: 启动 C++ 服务端
./build/server

# 终端 2: 运行 Python 客户端
cd python
python3 client.py
```

---

## 输出示例

### C++ 服务端

```
C++ TensorRT 服务端已启动，监听端口: 127.0.0.1:50051
C++ 收到来自 Python 的图片，宽: 1920
```

### Python 客户端

```
Python: 正在构造图像数据...
Python: 发送请求给 C++，等待推理...
Python: 收到结果啦！消息: TRT Inference Done!
Python: 解析到的 Bounding Boxes: [100.5, 200.0]
```

---

## 关键概念

### 1. `.proto` 文件 - 签合同

```protobuf
service TensorRTEngine {
    rpc RunInference (InferRequest) returns (InferResponse) {}
}
```

定义了服务名称、方法和请求/响应格式。

### 2. 生成的代码

| 语言 | 生成文件 | 作用 |
|------|----------|------|
| Python | `inference_pb2.py` | 数据结构 (Request/Response) |
| Python | `inference_pb2_grpc.py` | 通信存根 (Stub) |
| C++ | `inference.pb.h` | 数据结构 |
| C++ | `inference.grpc.pb.h` | 服务基类 |

### 3. 客户端调用

```python
# 像调用本地函数一样
response = stub.RunInference(request)
```

---

## 进阶学习

### 本项目 vs 实际项目

| 本项目 | 实际项目 (YOLO 部署) |
|--------|---------------------|
| 直接传图像 bytes | Shared Memory 传图像 |
| 模拟推理结果 | TensorRT 真实推理 |
| 本地通信 | 网络/跨设备通信 |
| 单帧 | 视频流 |

### 下一步学习

1. **流式 gRPC**: `rpc StreamInfer(stream InferRequest) returns (stream InferResponse)`
2. **双向流**: 实时视频传输
3. **认证**: TLS 加密
4. **负载均衡**: 多个推理服务实例

---

## 常见问题

### Q: 编译报错找不到 grpc?

```bash
# 安装完整 gRPC
git clone --recurse-submodules https://github.com/grpc/grpc
cd grpc
mkdir -p cmake/build
cmake -DCMAKE_BUILD_TYPE=Release -DgRPC_BUILD_TESTS=OFF ../..
make -j4
sudo make install
```

### Q: Python 找不到生成的模块?

```bash
# 确保在正确目录执行
cd python
python3 -m grpc_tools.protoc ...
```

### Q: C++ 服务启动失败?

```bash
# 检查端口是否被占用
lsof -i :50051

# 换端口
./build/server --port 50052
```

---

## 参考资料

- [gRPC 官方文档](https://grpc.io/docs/)
- [Protobuf 文档](https://developers.google.com/protocol-buffers)
- [howto.md](./howto.md) - 本教程的详细说明
