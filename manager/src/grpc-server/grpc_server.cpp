#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/health_check_service_interface.h>
#include <grpcpp/security/server_credentials.h>
#include <spdlog/spdlog.h>

#include <memory>
#include <sstream>

#include "decomposition/ast.h"
#include "dispatch/dispatch.h"
#include "dispatch/task_dealer.h"
#include "grpc_server.h"

DecompDispatchServiceImpl::DecompDispatchServiceImpl(Dispatcher& dispatcher)
    : dispatcher_(dispatcher),
      dispatcher_thread_(
          std::thread(&Dispatcher::RunDispatcher, &dispatcher_)) {}

grpc::ServerUnaryReactor* DecompDispatchServiceImpl::CalculateProblem(
    grpc::CallbackServerContext* context, const dcmp::AstRequest* request,
    dcmp::CalculationReply* reply) {
    std::istringstream input(request->str());
    auto ast = dcmp::AST::FromJson(input);

    // TODO: Get Result
    auto problem_id = dispatcher_.CalculateProblem(std::move(ast));
    (void)problem_id;

    reply->set_value("hui");

    grpc::ServerUnaryReactor* reactor = context->DefaultReactor();

    reactor->Finish(grpc::Status::OK);

    return reactor;
}

void RunServer(const manager::Config& config) {
    BrokerConnection requester = BrokerConnection::New(config);

    Dispatcher dispatcher(requester);

    DecompDispatchServiceImpl service(dispatcher);

    grpc::EnableDefaultHealthCheckService(true);
    grpc::reflection::InitProtoReflectionServerBuilderPlugin();

    grpc::ServerBuilder server_builder;

    server_builder
        .AddListeningPort(config.grpc_address,
                          grpc::InsecureServerCredentials())
        .RegisterService(&service);

    std::unique_ptr<grpc::Server> server(
        grpc::ServerBuilder()
            .AddListeningPort(config.grpc_address,
                              grpc::InsecureServerCredentials())
            .RegisterService(&service)
            .BuildAndStart());

    spdlog::info("gRPC Server is listening on {}", config.grpc_address);

    server->Wait();
}
