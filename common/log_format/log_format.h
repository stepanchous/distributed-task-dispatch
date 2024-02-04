#pragma once

#include <fmt/format.h>
#include <spdlog/spdlog.h>

#include <magic_enum/magic_enum.hpp>

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
