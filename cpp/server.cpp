/**
 * C++ gRPC 服务端 - TensorRT 推理服务
 * 
 * 编译:
 *   cd cpp
 *   mkdir build && cd build
 *   cmake ..
 *   make -j4
 * 
 * 运行:
 *   ./build/server
 * 
 * 测试:
 *   cd python
 *   python3 client.py
 */

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <grpcpp/grpcpp.h>

// gRPC 生成的.pb.h 文件
#include "inference.grpc.pb.h"

// 使用命名空间
using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using namespace ai_service;

// -------------------------------------------------
// TensorRT 模拟推理函数
// -------------------------------------------------
std::vector<float> mock_tensorrt_inference(const std::string& image_data, 
                                           int width, int height) {
    // 这里是模拟的 TensorRT 推理过程
    // 实际项目中，你需要:
    // 1. 将 image_data 转换为 float 数组
    // 2. 将数据从 CPU 拷贝到 GPU
    // 3. 调用 TensorRT 执行推理
    // 4. 将结果从 GPU 拷贝回 CPU
    
    std::cout << "    [TRT] 收到图像数据: " << width << "x" << height 
              << ", 大小: " << image_data.size() << " bytes" << std::endl;
    std::cout << "    [TRT] 模拟推理中..." << std::endl;
    
    // 模拟返回一些检测框 [x1, y1, x2, y2, ...]
    std::vector<float> results = {100.5f, 200.0f, 300.5f, 400.0f};
    
    return results;
}

// -------------------------------------------------
// 实现 gRPC 服务
// -------------------------------------------------
class TensorRTEngineImpl final : public TensorRTEngine::Service {
    
    /**
     * 重写 RunInference 方法
     * 
     * 这个方法会被 gRPC 自动调用:
     * 1. Python 客户端发送请求
     * 2. gRPC 框架接收并反序列化
     * 3. 调用我们实现的 RunInference
     * 4. 我们返回 Status 和 Response
     * 5. gRPC 框架序列化成二进制发给 Python
     */
    Status RunInference(ServerContext* context, 
                       const InferRequest* request,
                       InferResponse* reply) override {
        
        // -------------------------------------------------
        // 第 1 步: 获取 Python 发送的数据
        // -------------------------------------------------
        std::string image_data = request->image_data();
        int width = request->width();
        int height = request->height();
        
        std::cout << "\n[C++] 收到来自 Python 的图片:" << std::endl;
        std::cout << "    图像宽度: " << width << std::endl;
        std::cout << "    图像高度: " << height << std::endl;
        std::cout << "    数据大小: " << image_data.size() << " bytes" << std::endl;
        
        // -------------------------------------------------
        // 第 2 步: TensorRT 推理 (这里用模拟函数)
        // -------------------------------------------------
        std::vector<float> bboxes = mock_tensorrt_inference(image_data, width, height);
        
        // -------------------------------------------------
        // 第 3 步: 构造响应
        // -------------------------------------------------
        reply->set_success(true);
        
        // 添加检测框到响应 (repeated 字段用 add_xxx 添加)
        for (float bbox : bboxes) {
            reply->add_bounding_boxes(bbox);
        }
        
        reply->set_message("TRT Inference Done! Python↔C++ gRPC 通信成功!");
        
        std::cout << "    [C++] 推理完成，返回 " << bboxes.size() << " 个检测框" << std::endl;
        
        // -------------------------------------------------
        // 第 4 步: 返回状态
        // -------------------------------------------------
        // Status::OK 表示成功
        // 其他状态码表示错误
        return Status::OK;
    }
};

// -------------------------------------------------
// 主函数: 启动 gRPC 服务器
// -------------------------------------------------
void RunServer() {
    // 服务器监听地址
    std::string server_address = "127.0.0.1:50051";
    
    // 创建服务实现实例
    TensorRTEngineImpl service;
    
    // gRPC 服务器构建器
    ServerBuilder builder;
    
    // 注册服务
    builder.RegisterService(&service);
    
    // 监听端口
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    
    // 创建并启动服务器
    std::unique_ptr<Server> server(builder.BuildAndStart());
    
    if (!server) {
        std::cerr << "❌ 服务器启动失败!" << std::endl;
        return;
    }
    
    std::cout << "\n" << "========================================" << std::endl;
    std::cout << "  C++ gRPC 服务端已启动" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "  监听地址: " << server_address << std::endl;
    std::cout << "  服务名称: ai_service.TensorRTEngine.RunInference" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "\n等待 Python 客户端连接...\n" << std::endl;
    
    // 服务器保持运行
    server->Wait();
}

int main(int argc, char** argv) {
    RunServer();
    return 0;
}
