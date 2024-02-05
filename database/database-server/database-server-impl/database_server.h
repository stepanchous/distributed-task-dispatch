#pragma once

#include <SQLiteCpp/SQLiteCpp.h>
#include <grpcpp/grpcpp.h>

#include "db.grpc.pb.h"

class DatabaseServiceImpl : public db::DatabaseService::CallbackService {
   public:
    DatabaseServiceImpl(SQLite::Database db);

    grpc::ServerUnaryReactor* WriteDynamicData(
        grpc::CallbackServerContext* context, const db::DynamicRecord* request,
        db::Empty*) override;

    grpc::ServerUnaryReactor* ReadData(grpc::CallbackServerContext* context,
                                       const db::RecordId* request,
                                       db::Field* reply) override;

    grpc::ServerUnaryReactor* ClearIntermediateCalculations(
        grpc::CallbackServerContext* context, const db::ProblemId* request,
        db::Empty*) override;

   private:
    SQLite::Database db_;

    void WriteDynamicDataImpl(const db::DynamicRecord& dyn_record);

    db::Field ReadDataImpl(const db::RecordId& record_id);

    void ClearIntermediateCalculationsImpl(const db::ProblemId& problem_id);
};

void RunServer();
