#include <fstream>
#include <stdexcept>

#include "grpc_client.h"

namespace {

std::string ReadFile(const std::string& path) {
    std::ifstream f(path);

    if (!f) {
        throw std::invalid_argument("Unable to open file: " + path);
    }

    return std::string((std::istreambuf_iterator<char>(f)),
                       std::istreambuf_iterator<char>());
}

}  // namespace

DecompDispatchClient::DecompDispatchClient(
    std::shared_ptr<grpc::Channel> channel)
    : stub_(dcmp::DecompDispatchService::NewStub(channel)) {}

bool DecompDispatchClient::CalculateProblem(std::string str) {
    dcmp::AstRequest ast_json;
    ast_json.set_str(std::move(str));

    grpc::ClientContext context;
    dcmp::CalculationReply reply;

    grpc::CompletionQueue cq;

    grpc::Status status;

    std::unique_ptr<grpc::ClientAsyncResponseReader<dcmp::CalculationReply>>
        rpc(stub_->AsyncCalculateProblem(&context, ast_json, &cq));

    rpc->Finish(&reply, &status, (void*)this);

    void* got_tag;
    bool ok = false;

    GPR_ASSERT(cq.Next(&got_tag, &ok));
    GPR_ASSERT(got_tag == (void*)this);
    GPR_ASSERT(ok);

    return status.ok();
}

void RunClient(const client::Config& config, const std::string& json_path) {
    auto json_str = ReadFile(json_path);

    DecompDispatchClient client(
        grpc::CreateChannel(config.target, grpc::InsecureChannelCredentials()));

    if (client.CalculateProblem(json_str)) {
        std::cout << "Operation succeeded" << std::endl;
    } else {
        std::cerr << "Operation failed" << std::endl;
    }
}
