#pragma once

#include <grpcpp/grpcpp.h>

#include <memory>

#include "db.grpc.pb.h"
#include "domain/domain.h"

class DatabaseClient {
   public:
    struct DynRecordId {
        size_t problem_id;
        size_t task_id;
    };

    using RecordId = std::variant<std::string, DynRecordId>;

    DatabaseClient(std::shared_ptr<grpc::Channel> channel);

    void WriteDynamicData(size_t problem_id, size_t task_id,
                          const domain::ExprResult& expr_result);

    domain::ExprResult ReadData(const RecordId& record_id);

    void ClearIntermediateCalculations(size_t problem_id);

    bool CalculateProblem(const std::string& str);

   private:
    std::unique_ptr<db::DatabaseService::Stub> stub_;

    static db::DynamicRecord BuildWriteRequest(
        size_t problem_id, size_t task_id,
        const domain::ExprResult& expr_result);
};
