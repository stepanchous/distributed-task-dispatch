#include <grpcpp/grpcpp.h>

#include <fstream>
#include <iostream>
#include <memory>

#include "proto/ast_json.grpc.pb.h"

class AstJsonClient {
   public:
    AstJsonClient(std::shared_ptr<grpc::Channel> channel)
        : stub_(AstJson::AstJsonService::NewStub(channel)) {}

    bool SendAstJson(std::string ast_json) {
        AstJson::JsonString ast_json_proto;
        ast_json_proto.set_json_str(std::move(ast_json));

        grpc::ClientContext client_context;
        AstJson::Empty empty_response;

        auto status = stub_->SendAstJson(&client_context, ast_json_proto,
                                         &empty_response);

        return status.ok();
    }

   private:
    std::shared_ptr<AstJson::AstJsonService::Stub> stub_;
};

int main(int argc, char* argv[]) {
    std::cout << "Hello, From Docker!" << std::endl;

    if (argc != 2) {
        std::cerr << "Invalid number of arguments" << std::endl;
    }

    std::ifstream f(argv[1]);

    if (!f) {
        std::cerr << "Unable to open file" << std::endl;
        return 1;
    }

    std::string json_str((std::istreambuf_iterator<char>(f)),
                         std::istreambuf_iterator<char>());

    AstJsonClient client(grpc::CreateChannel(
        "localhost:5300", grpc::InsecureChannelCredentials()));

    if (client.SendAstJson(json_str)) {
        std::cout << "json was send successfully" << std::endl;
    } else {
        std::cerr << "Failed to send json" << std::endl;
    }
}
