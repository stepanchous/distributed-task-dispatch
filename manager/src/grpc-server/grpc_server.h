#pragma once

#include <grpcpp/grpcpp.h>

#include <thread>

#include "computation.grpc.pb.h"
#include "dispatch/dispatch.h"

class DecompDispatchServiceImpl
    : public dcmp::DecompDispatchService::CallbackService {
   public:
    DecompDispatchServiceImpl(Dispatcher& dispatcher);

    grpc::ServerUnaryReactor* CalculateProblem(
        grpc::CallbackServerContext* context, const dcmp::AstRequest* request,
        dcmp::CalculationReply* reply) override;

   private:
    Dispatcher& dispatcher_;
    std::thread dispatcher_thread_;
};

void RunServer();
