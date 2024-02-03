#pragma once

#include <grpcpp/grpcpp.h>

#include <memory>

#include "client-config/client_config.h"
#include "computation.grpc.pb.h"

// class DecompDispatchClient {
//    public:
//     DecompDispatchClient(std::shared_ptr<grpc::Channel> channel);
//
//     bool CalculateProblem(std::string str);
//
//    private:
//     std::shared_ptr<dcmp::DecompDispatchService::Stub> stub_;
// };

class DecompDispatchClient {
   public:
    DecompDispatchClient(std::shared_ptr<grpc::Channel> channel);

    bool CalculateProblem(const std::string& str);

   private:
    std::unique_ptr<dcmp::DecompDispatchService::Stub> stub_;
};
void RunClient(const client::Config& config, const std::string& json_path);
