#pragma once

#include <fmt/format.h>
#include <spdlog/spdlog.h>

#include <magic_enum/magic_enum.hpp>
#include <stdexcept>
#include <variant>

#include "db.pb.h"
#include "domain/domain.h"
#include "task.pb.h"
#include "worker.pb.h"

template <>
struct fmt::formatter<task::StaticOperand> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const task::StaticOperand& operand, FormatContext& ctx) {
        return fmt::format_to(ctx.out(),
                              "[StaticOperand] identifier: {}, is_scalar: {}",
                              operand.identifier(), operand.is_scalar());
    }
};

template <>
struct fmt::formatter<task::DynOperand> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const task::DynOperand& operand, FormatContext& ctx) {
        return fmt::format_to(
            ctx.out(),
            "[DynOperand] problem_id: {}, task_id: {}, is_scalar: {}",
            operand.id().problem_id(), operand.id().task_id(),
            operand.is_scalar());
    }
};

template <>
struct fmt::formatter<task::Operand> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const task::Operand& operand, FormatContext& ctx) {
        if (operand.has_static_operand()) {
            return fmt::format_to(ctx.out(), "[Operand] StaticOperand: {}",
                                  operand.static_operand());
        } else if (operand.has_dyn_operand()) {
            return fmt::format_to(ctx.out(), "[Operand] DynOperand: {}",
                                  operand.dyn_operand());
        } else {
            return fmt::format_to(ctx.out(), "[Operand] Operand: Invalid");
        }
    }
};

template <>
struct fmt::formatter<task::TaskId> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const task::TaskId& task_id, FormatContext& ctx) {
        return fmt::format_to(ctx.out(), "[TaskId] problem_id: {}, task_id: {}",
                              task_id.problem_id(), task_id.task_id());
    }
};

template <>
struct fmt::formatter<task::Task> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const task::Task& task, FormatContext& ctx) {
        auto operation_type_sv = magic_enum::enum_name(
            static_cast<domain::ExprType>(task.operation_type()));
        const std::string operation_type(operation_type_sv.begin(),
                                         operation_type_sv.end());

        auto it = fmt::format_to(ctx.out(),
                                 "[Task] id: {}, operation_type: {}, "
                                 "operands: ",
                                 task.id(), operation_type);

        bool first = true;

        for (const auto& operand : task.operands()) {
            if (first) {
                it = fmt::format_to(it, "{}", operand);
                first = false;
                continue;
            }

            it = fmt::format_to(it, ", {}", operand);
        }

        return it;
    }
};

template <>
struct fmt::formatter<task::WorkerTaskId> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const task::WorkerTaskId& task_id, FormatContext& ctx) {
        return fmt::format_to(ctx.out(), "[WorkerTaskId] identity: {}, id: {}",
                              task_id.identity(), task_id.id());
    }
};

template <>
struct fmt::formatter<task::WorkerMessage> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const task::WorkerMessage& message, FormatContext& ctx) {
        if (message.has_task_id()) {
            return fmt::format_to(ctx.out(), "[WorkerMessage] task_id: {}",
                                  message.task_id());
        } else if (message.has_core_count()) {
            return fmt::format_to(ctx.out(), "[WorkerAck] core_count: {}",
                                  message.core_count());
        } else {
            return fmt::format_to(ctx.out(), "[WorkerMessage] Invalid");
        }
    }
};

template <>
struct fmt::formatter<db::ProblemId> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const db::ProblemId& problem_id, FormatContext& ctx) {
        return fmt::format_to(ctx.out(), "[db::ProblemID] problem_id: {}",
                              problem_id.problem_id());
    }
};

template <>
struct fmt::formatter<db::StaticRecordId> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const db::StaticRecordId& record_id, FormatContext& ctx) {
        return fmt::format_to(ctx.out(), "[StaticRecordId] identifier: {}",
                              record_id.identifier());
    }
};

template <>
struct fmt::formatter<db::DynamicRecordId> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const db::DynamicRecordId& record_id, FormatContext& ctx) {
        return fmt::format_to(ctx.out(),
                              "[DynamicRecordId] problem_id: {}, task_id: {}",
                              record_id.problem_id(), record_id.task_id());
    }
};

template <>
struct fmt::formatter<db::RecordId> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const db::RecordId& record_id, FormatContext& ctx) {
        if (record_id.has_dyn_id()) {
            return fmt::format_to(ctx.out(), "[RecordId] dyn_id: {}",
                                  record_id.dyn_id());
        } else if (record_id.has_static_id()) {
            return fmt::format_to(ctx.out(), "[RecordId] static_id: {}",
                                  record_id.static_id());
        } else {
            throw std::logic_error("Invlaid message");
        }
    }
};

template <>
struct fmt::formatter<db::Scalar> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const db::Scalar& scalar, FormatContext& ctx) {
        return fmt::format_to(ctx.out(), "[Scalar] scalar: {}", scalar.value());
    }
};

template <>
struct fmt::formatter<db::List> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const db::List& list, FormatContext& ctx) {
        auto it = fmt::format_to(ctx.out(), "[List] list: ");

        bool first = true;
        for (const auto& value : list.values()) {
            if (first) {
                it = fmt::format_to(it, "{}", value);
                first = false;
                continue;
            }

            it = fmt::format_to(it, ", {}", value);
        }

        return it;
    }
};

template <>
struct fmt::formatter<db::DynamicRecord> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const db::DynamicRecord& dyn_record, FormatContext& ctx) {
        return fmt::format_to(ctx.out(),
                              "[DynamicRecord] record_id: {}, field: {}",
                              dyn_record.record_id(), dyn_record.field());
    }
};

template <>
struct fmt::formatter<db::Field> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const db::Field& field, FormatContext& ctx) {
        if (field.has_scalar()) {
            return fmt::format_to(ctx.out(), "[Field] data: {}",
                                  field.scalar());
        } else if (field.has_list()) {
            return fmt::format_to(ctx.out(), "[Field] data: {}", field.list());
        } else {
            throw std::logic_error("Invalid message");
        }
    }
};

template <>
struct fmt::formatter<task::WorkerTask> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const task::WorkerTask& worker_task, FormatContext& ctx) {
        return fmt::format_to(ctx.out(), "[WorkerTask] identity: {}, task: {}",
                              worker_task.identity(), worker_task.task());
    }
};

template <>
struct fmt::formatter<domain::List> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const domain::List& list, FormatContext& ctx) {
        auto it = fmt::format_to(ctx.out(), "[List] list: ");

        bool first = true;
        for (const auto& value : list) {
            if (first) {
                it = fmt::format_to(ctx.out(), "({}", value);
                first = false;
                continue;
            }

            it = fmt::format_to(ctx.out(), ", {}", value);
        }

        it = fmt::format_to(ctx.out(), ")");

        return it;
    }
};

template <>
struct fmt::formatter<domain::ExprResult> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const domain::ExprResult& expr_result, FormatContext& ctx) {
        if (std::holds_alternative<domain::List>(expr_result)) {
            return fmt::format_to(ctx.out(), "[ExprResult] expr_result: {}",
                                  std::get<domain::List>(expr_result));
        } else if (std::holds_alternative<domain::Scalar>(expr_result)) {
            return fmt::format_to(ctx.out(), "[ExprResult] expr_result: {}",
                                  std::get<domain::Scalar>(expr_result));
        } else {
            throw std::logic_error("Invalid variant");
        }
    }
};
