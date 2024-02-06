#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/health_check_service_interface.h>
#include <grpcpp/security/server_credentials.h>
#include <spdlog/spdlog.h>

#include <memory>
#include <sstream>

#include "computation.pb.h"
#include "decomposition/ast.h"
#include "dispatch/dispatch.h"
#include "dispatch/task_dealer.h"
#include "grpc_server.h"

namespace env {

const char* GRPC_ADDRESS = "GRPC_ADDRESS";

}

namespace {

struct ExprResultVisitor {
    ExprResultVisitor(dcmp::CalculationReply& reply) : reply(reply) {}

    void operator()(const domain::List& list) {
        auto l = new dcmp::List;

        for (const auto& value : list) {
            l->add_values(value);
        }

        reply.set_allocated_list(l);
    }

    void operator()(domain::Scalar scalar) { reply.set_scalar(scalar); }

   private:
    dcmp::CalculationReply& reply;
};

}  // namespace

DecompDispatchServiceImpl::DecompDispatchServiceImpl(
    BrokerConnection broker_connection)
    : dispatcher_(std::move(broker_connection)),
      dispatcher_thread_(
          std::thread(&Dispatcher::RunDispatcher, &dispatcher_)) {}

grpc::ServerUnaryReactor* DecompDispatchServiceImpl::CalculateProblem(
    grpc::CallbackServerContext* context, const dcmp::AstRequest* request,
    dcmp::CalculationReply* reply) {
    std::istringstream input(request->str());
    auto ast = dcmp::AST::FromJson(input);

    domain::ExprResult expr_result =
        dispatcher_.CalculateProblem(std::move(ast));

    std::visit(ExprResultVisitor(*reply), expr_result);

    grpc::ServerUnaryReactor* reactor = context->DefaultReactor();

    reactor->Finish(grpc::Status::OK);

    return reactor;
}

void RunServer() {
    DecompDispatchServiceImpl service(BrokerConnection::New());

    grpc::EnableDefaultHealthCheckService(true);
    grpc::reflection::InitProtoReflectionServerBuilderPlugin();

    std::unique_ptr<grpc::Server> server(
        grpc::ServerBuilder()
            .AddListeningPort(std::getenv(env::GRPC_ADDRESS),
                              grpc::InsecureServerCredentials())
            .RegisterService(&service)
            .BuildAndStart());

    spdlog::info("gRPC Server is listening on {}",
                 std::getenv(env::GRPC_ADDRESS));

    server->Wait();
}
