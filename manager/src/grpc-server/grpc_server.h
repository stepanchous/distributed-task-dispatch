#pragma once

#include <grpcpp/grpcpp.h>

#include "manager-config/manager_config.h"
#include "proto/computation.grpc.pb.h"

class DecompDispatchServerImpl final {
   public:
    void Run(const manager::Config& config);

    ~DecompDispatchServerImpl();

   private:
    std::unique_ptr<grpc::ServerCompletionQueue> cq_;
    dcmp::DecompDispatchService::AsyncService service_;
    std::unique_ptr<grpc::Server> server_;

   private:
    void HandleRpcs();

    class CallData {
       public:
        CallData(dcmp::DecompDispatchService::AsyncService* service,
                 grpc::ServerCompletionQueue* cq);

        void Proceed();

       private:
        dcmp::DecompDispatchService::AsyncService* service_;
        grpc::ServerCompletionQueue* cq_;
        grpc::ServerContext context_;
        dcmp::AstRequest request_;
        dcmp::CalculationReply reply_;
        grpc::ServerAsyncResponseWriter<dcmp::CalculationReply> responder_;

        enum CallStatus {
            CREATE,
            PROCESS,
            FINISH,
        };

        CallStatus status_;
    };
};

void RunServer(const manager::Config& config);
