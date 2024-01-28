#pragma once

#include <grpcpp/grpcpp.h>

#include "manager-config/manager_config.h"
#include "proto/ast_json.grpc.pb.h"

class AstJsonServiceImpl : public AstJson::AstJsonService::Service {
   public:
    grpc::Status SendAstJson(grpc::ServerContext* context,
                             const AstJson::JsonString* json_str,
                             AstJson::Empty* reply) override;
};

void RunServer(const manager::Config& config);
