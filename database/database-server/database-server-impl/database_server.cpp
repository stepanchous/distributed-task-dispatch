#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/health_check_service_interface.h>
#include <spdlog/spdlog.h>

#include <cstdlib>
#include <stdexcept>

#include "database_server.h"
#include "db.pb.h"
#include "log_format/log_format.h"

namespace env {

const char* DB_PATH = "DB_PATH";
const char* DB_ADDRESS = "DB_ADDRESS";

}  // namespace env

void RunServer() {
    SQLite::Database db(std::getenv(env::DB_PATH));

    DatabaseServiceImpl service(std::move(db));

    grpc::EnableDefaultHealthCheckService(true);
    grpc::reflection::InitProtoReflectionServerBuilderPlugin();

    std::unique_ptr<grpc::Server> server(
        grpc::ServerBuilder()
            .AddListeningPort(std::getenv(env::DB_ADDRESS),
                              grpc::InsecureServerCredentials())
            .RegisterService(&service)
            .BuildAndStart());

    spdlog::info("Database service is listening on {}",
                 std::getenv(env::DB_ADDRESS));

    server->Wait();
}

DatabaseServiceImpl::DatabaseServiceImpl(SQLite::Database db)
    : db_(std::move(db)) {}

grpc::ServerUnaryReactor* DatabaseServiceImpl::WriteDynamicData(
    grpc::CallbackServerContext* context, const db::DynamicRecord* request,
    db::Empty*) {
    spdlog::info("Write requested {}", *request);

    WriteDynamicDataImpl(*request);

    grpc::ServerUnaryReactor* reactor = context->DefaultReactor();

    reactor->Finish(grpc::Status::OK);

    return reactor;
}

grpc::ServerUnaryReactor* DatabaseServiceImpl::ReadData(
    grpc::CallbackServerContext* context, const db::RecordId* request,
    db::Field* reply) {
    spdlog::info("Read requested {}", *request);

    *reply = ReadDataImpl(*request);

    grpc::ServerUnaryReactor* reactor = context->DefaultReactor();

    reactor->Finish(grpc::Status::OK);

    return reactor;
}

grpc::ServerUnaryReactor* DatabaseServiceImpl::ClearIntermediateCalculations(
    grpc::CallbackServerContext* context, const db::ProblemId* request,
    db::Empty*) {
    spdlog::info("Clear intermediate data requested {}", *request);

    ClearIntermediateCalculationsImpl(*request);

    grpc::ServerUnaryReactor* reactor = context->DefaultReactor();

    reactor->Finish(grpc::Status::OK);

    return reactor;
}

void DatabaseServiceImpl::WriteDynamicDataImpl(
    const db::DynamicRecord& dyn_record) {
    SQLite::Statement query(db_,
                            "INSERT INTO dynamic_data "
                            "(problem_id, task_id, data)"
                            "VALUES (?, ?, ?)");

    query.bind(1, dyn_record.record_id().problem_id());
    query.bind(2, dyn_record.record_id().task_id());
    query.bind(3, dyn_record.field().SerializeAsString().data());

    query.exec();

    spdlog::info("Write performed {}", dyn_record);
}

db::Field DatabaseServiceImpl::ReadDataImpl(const db::RecordId& record_id) {
    db::Field field;

    if (record_id.has_dyn_id()) {
        SQLite::Statement query(db_,
                                "SELECT * FROM dynamic_data "
                                "WHERE problem_id = ? AND task_id = ?;");

        query.bind(1, record_id.dyn_id().problem_id());
        query.bind(2, record_id.dyn_id().task_id());

        query.executeStep();

        const void* data = query.getColumn(2);
        size_t size = query.getColumn(2).size();

        field.ParseFromArray(data, size);

    } else if (record_id.has_static_id()) {
        SQLite::Statement query(db_,
                                "SELECT * FROM static_data "
                                "WHERE identifier = ?;");

        query.bind(1, record_id.static_id().identifier());

        query.executeStep();

        const void* data = query.getColumn(1);
        size_t size = query.getColumn(1).size();

        field.ParseFromArray(data, size);
    } else {
        throw std::logic_error("Invalid message");
    }

    spdlog::info("Read field {}", field);

    return field;
}

void DatabaseServiceImpl::ClearIntermediateCalculationsImpl(
    const db::ProblemId& problem_id) {
    SQLite::Statement query(db_,
                            "DELETE FROM dynamic_data WHERE problem_id = ?");

    query.bind(1, problem_id.problem_id());

    query.exec();

    spdlog::info("Removed intermediate data for {}", problem_id);
}
