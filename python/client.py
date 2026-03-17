#!/usr/bin/env python3
"""
Python 客户端 - 调用 C++ gRPC 服务

这个客户端演示了如何像调用本地函数一样，调用远端 C++ 服务
"""

import grpc
import inference_pb2       # gRPC 生成的 Protobuf 数据结构
import inference_pb2_grpc  # gRPC 生成的通信存根


def run():
    """
    主函数: 演示 gRPC 调用流程
    """
    print("=" * 50)
    print("Python gRPC 客户端 Demo")
    print("=" * 50)
    
    # -------------------------------------------------
    # 第 1 步: 建立与 C++ 服务端的连接
    # -------------------------------------------------
    # 如果是跨机器通信，把 127.0.0.1 换成服务端 IP
    server_address = "127.0.0.1:50051"
    
    print(f"\n[1] 连接到 C++ 服务端: {server_address}")
    with grpc.insecure_channel(server_address) as channel:
        
        # -------------------------------------------------
        # 第 2 步: 获取通信存根 (Stub)
        # -------------------------------------------------
        # Stub 相当于一个代理，帮你处理网络通信细节
        stub = inference_pb2_grpc.TensorRTEngineStub(channel)
        
        print("[2] 获取通信存根成功")
        
        # -------------------------------------------------
        # 第 3 步: 构造请求数据
        # -------------------------------------------------
        # 严格按照 .proto 合同构造数据
        print("[3] 构造请求数据...")
        
        # 模拟图像数据 (实际项目中用 cv2 读取)
        # 这里用一小段伪造的 JPEG 头作为示例
        fake_image_bytes = b'\xff\xd8\xff\xe0\x00\x10\x4a\x46\x49\x46'
        
        request = inference_pb2.InferRequest(
            image_data=fake_image_bytes,
            width=1920,
            height=1080
        )
        
        print(f"    - 图像宽度: {request.width}")
        print(f"    - 图像高度: {request.height}")
        print(f"    - 数据大小: {len(request.image_data)} bytes")
        
        # -------------------------------------------------
        # 第 4 步: 调用远程服务 (RPC)
        # -------------------------------------------------
        # 这行代码会:
        # 1. 序列化 request 为二进制
        # 2. 发送请求到 C++ 服务端
        # 3. 等待服务端处理
        # 4. 接收响应
        # 5. 反序列化响应
        print("\n[4] 发送请求给 C++，等待响应...")
        
        try:
            response = stub.RunInference(request)
            
            # -------------------------------------------------
            # 第 5 步: 处理响应
            # -------------------------------------------------
            print("[5] 收到 C++ 服务端的响应!")
            print(f"    - 推理是否成功: {response.success}")
            print(f"    - 附加消息: {response.message}")
            print(f"    - 检测框数量: {len(response.bounding_boxes)}")
            
            if response.bounding_boxes:
                print(f"    - Bounding Boxes: {list(response.bounding_boxes)}")
            
            print("\n" + "=" * 50)
            print("✅ gRPC 调用成功!")
            print("=" * 50)
            
        except grpc.RpcError as e:
            print(f"\n❌ gRPC 调用失败!")
            print(f"   错误码: {e.code()}")
            print(f"   错误信息: {e.details()}")
            print("\n请确保 C++ 服务端正在运行!")
            print("在 cpp 目录执行: mkdir build && cd build && cmake .. && make && ./server")


if __name__ == "__main__":
    run()
