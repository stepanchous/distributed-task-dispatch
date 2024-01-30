#include <iostream>
#include <memory>
#include <sstream>

#include "decomposition/ast.h"
#include "decomposition/decomposition.h"
#include "grpc_server.h"

void DecompDispatchServerImpl::Run(const manager::Config& config) {
    grpc::ServerBuilder server_builder;

    server_builder
        .AddListeningPort(config.addr_uri, grpc::InsecureServerCredentials())
        .RegisterService(&service_);

    cq_ = server_builder.AddCompletionQueue();

    server_ = server_builder.BuildAndStart();

    std::cout << "Server listening on " << config.addr_uri << std::endl;

    HandleRpcs();
}

DecompDispatchServerImpl::~DecompDispatchServerImpl() {
    server_->Shutdown();
    cq_->Shutdown();
}

void DecompDispatchServerImpl::HandleRpcs() {
    new CallData(&service_, cq_.get());
    void* tag;
    bool ok;

    while (true) {
        GPR_ASSERT(cq_->Next(&tag, &ok));
        GPR_ASSERT(ok);
        static_cast<CallData*>(tag)->Proceed();
    }
}

DecompDispatchServerImpl::CallData::CallData(
    dcmp::DecompDispatchService::AsyncService* service,
    grpc::ServerCompletionQueue* cq)
    : service_(service),
      cq_(cq),
      responder_(&context_),
      status_(CallStatus::CREATE) {
    Proceed();
}

void DecompDispatchServerImpl::CallData::Proceed() {
    if (status_ == CallStatus::CREATE) {
        status_ = CallStatus::PROCESS;
        service_->RequestCalculateProblem(&context_, &request_, &responder_,
                                          cq_, cq_, this);
    } else if (status_ == CallStatus::PROCESS) {
        new CallData(service_, cq_);

        std::istringstream input(request_.str());
        auto ast = dcmp::AST::FromJson(input);

        dcmp::TaskDecompositor decompositor =
            dcmp::TaskDecompositor::New(std::move(ast));

        decompositor.PrintTasks(std::cout);

        status_ = CallStatus::FINISH;

        responder_.Finish(reply_, grpc::Status::OK, this);
    } else {
        GPR_ASSERT(status_ == CallStatus::FINISH);
        delete this;
    }
}

// grpc::Status AstJsonServiceImpl::SendAstJson(
//     grpc::ServerContext*, const AstJson::JsonString* json_str,
//     AstJson::Empty*) {
// std::istringstream input(json_str->json_str());
// auto ast = dcmp::AST::FromJson(input);
//
// dcmp::TaskDecompositor decompositor =
//     dcmp::TaskDecompositor::New(std::move(ast));
//
// decompositor.PrintTasks(std::cout);
//
//     return grpc::Status::OK;
// }
//
// void RunServer(const manager::Config& config) {
//     std::string server_address(config.addr_uri);
//
//     AstJsonServiceImpl service;
//
//     grpc::ServerBuilder server_builder;
//
//     server_builder
//         .AddListeningPort(server_address,
//         grpc::InsecureServerCredentials()) .RegisterService(&service);
//
//     std::unique_ptr<grpc::Server> server(server_builder.BuildAndStart());
//
//     std::cout << "Server listening on " << server_address << std::endl;
//
//     auto shutdown_handler = [](int) { abort(); };
//
//     std::signal(SIGTERM, shutdown_handler);
//
//     server->Wait();
// }
