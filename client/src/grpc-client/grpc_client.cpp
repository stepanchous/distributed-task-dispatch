#include <condition_variable>
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

bool DecompDispatchClient::CalculateProblem(const std::string& str) {
    dcmp::AstRequest request;
    request.set_str(str);

    dcmp::CalculationReply reply;

    grpc::ClientContext context;

    std::mutex mu;
    std::condition_variable cv;
    bool done = false;
    grpc::Status status;
    stub_->async()->CalculateProblem(
        &context, &request, &reply, [&mu, &cv, &done, &status](grpc::Status s) {
            status = std::move(s);
            std::lock_guard<std::mutex> lock(mu);
            done = true;
            cv.notify_one();
        });

    std::unique_lock<std::mutex> lock(mu);
    while (!done) {
        cv.wait(lock);
    }

    std::cout << reply.value() << std::endl;

    // Act upon its status.
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
