#include <spdlog/spdlog.h>

#include <condition_variable>
#include <fstream>
#include <stdexcept>

#include "grpc_client.h"
#include "log_format/log_format.h"

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

namespace env {

const char* GRPC_ADDRESS = "GRPC_ADDRESS";

}

DecompDispatchClient::DecompDispatchClient(
    std::shared_ptr<grpc::Channel> channel)
    : stub_(dcmp::DecompDispatchService::NewStub(channel)) {}

bool DecompDispatchClient::CalculateProblem(const std::string& str) {
    dcmp::AstRequest request;
    spdlog::info("Called calculate");

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

    spdlog::info("Got result {}", reply);

    return status.ok();
}

void RunClient(const std::string& json_path) {
    auto json_str = ReadFile(json_path);

    DecompDispatchClient client(grpc::CreateChannel(
        std::getenv(env::GRPC_ADDRESS), grpc::InsecureChannelCredentials()));

    if (client.CalculateProblem(json_str)) {
        spdlog::info("Finished successfully");
    } else {
        spdlog::error("Client failed");
    }
}
