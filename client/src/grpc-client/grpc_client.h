#pragma once

#include <grpcpp/grpcpp.h>

#include <memory>

#include "computation.grpc.pb.h"

class DecompDispatchClient {
   public:
    DecompDispatchClient(std::shared_ptr<grpc::Channel> channel);

    bool CalculateProblem(const std::string& str);

   private:
    std::unique_ptr<dcmp::DecompDispatchService::Stub> stub_;
};

void RunClient(const std::string& json_path);
