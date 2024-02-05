#include <spdlog/spdlog.h>

#include <condition_variable>

#include "database_client.h"
#include "log_format/log_format.h"

namespace {

struct ExprResultVisitor {
    ExprResultVisitor(db::Field& field) : field_(field) {}

    void operator()(domain::Scalar value) {
        auto scalar = new db::Scalar;
        scalar->set_value(value);

        field_.set_allocated_scalar(scalar);
    }

    void operator()(domain::List list) {
        auto l = new db::List;

        for (const auto& value : list) {
            l->add_values(value);
        }

        field_.set_allocated_list(l);
    }

   private:
    db::Field& field_;
};

struct RecordIdVisitor {
    RecordIdVisitor(db::RecordId& record_id) : record_id_(record_id) {}

    void operator()(const std::string& identifier) {
        auto static_id = new db::StaticRecordId;
        static_id->set_identifier(identifier);

        record_id_.set_allocated_static_id(static_id);
    }

    void operator()(DatabaseClient::DynRecordId record_id) {
        auto dyn_id = new db::DynamicRecordId;
        dyn_id->set_problem_id(record_id.problem_id);
        dyn_id->set_task_id(record_id.task_id);

        record_id_.set_allocated_dyn_id(dyn_id);
    }

   private:
    db::RecordId& record_id_;
};

}  // namespace

DatabaseClient::DatabaseClient(std::shared_ptr<grpc::Channel> channel)
    : stub_(db::DatabaseService::NewStub(channel)) {}

void DatabaseClient::WriteDynamicData(size_t problem_id, size_t task_id,
                                      const domain::ExprResult& expr_result) {
    db::DynamicRecord request =
        BuildWriteRequest(problem_id, task_id, expr_result);

    db::Empty reply;

    grpc::ClientContext context;

    std::mutex mu;
    std::condition_variable cv;
    bool done = false;
    grpc::Status status;
    stub_->async()->WriteDynamicData(
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

    spdlog::info("Written to db {}", request);
}

domain::ExprResult DatabaseClient::ReadData(const RecordId& record_id) {
    db::RecordId request;
    std::visit(RecordIdVisitor(request), record_id);

    db::Field reply;

    grpc::ClientContext context;

    std::mutex mu;
    std::condition_variable cv;
    bool done = false;
    grpc::Status status;
    stub_->async()->ReadData(&context, &request, &reply,
                             [&mu, &cv, &done, &status](grpc::Status s) {
                                 status = std::move(s);
                                 std::lock_guard<std::mutex> lock(mu);
                                 done = true;
                                 cv.notify_one();
                             });

    std::unique_lock<std::mutex> lock(mu);
    while (!done) {
        cv.wait(lock);
    }

    spdlog::info("Read from db {}", reply);

    domain::ExprResult expr_result;

    if (reply.has_scalar()) {
        expr_result = reply.scalar().value();
    } else if (reply.has_list()) {
        expr_result = domain::List(reply.list().values().begin(),
                                   reply.list().values().end());
    } else {
        throw std::logic_error("Invalid message");
    }

    return expr_result;
}

void DatabaseClient::ClearIntermediateCalculations(size_t problem_id) {
    db::ProblemId request;
    request.set_problem_id(problem_id);

    db::Empty reply;

    grpc::ClientContext context;

    std::mutex mu;
    std::condition_variable cv;
    bool done = false;
    grpc::Status status;
    stub_->async()->ClearIntermediateCalculations(
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

    spdlog::info("Cleared intermediate results");
}

bool CalculateProblem(const std::string& str);

db::DynamicRecord DatabaseClient::BuildWriteRequest(
    size_t problem_id, size_t task_id, const domain::ExprResult& expr_result) {
    db::DynamicRecord request;

    auto dyn_id = new db::DynamicRecordId;
    dyn_id->set_problem_id(problem_id);
    dyn_id->set_task_id(task_id);

    auto field = new db::Field;
    std::visit(ExprResultVisitor(*field), expr_result);

    request.set_allocated_record_id(dyn_id);
    request.set_allocated_field(field);

    return request;
}
