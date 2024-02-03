#pragma once

#include <fmt/format.h>
#include <spdlog/spdlog.h>

#include <magic_enum/magic_enum.hpp>

#include "domain/domain.h"
#include "task.pb.h"

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
            operand.problem_id(), operand.task_id(), operand.is_scalar());
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
struct fmt::formatter<task::Task> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const task::Task& task, FormatContext& ctx) {
        auto operation_type_sv = magic_enum::enum_name(
            static_cast<domain::ExprType>(task.operation_type()));
        const std::string operation_type(operation_type_sv.begin(),
                                         operation_type_sv.end());

        auto it = fmt::format_to(
            ctx.out(),
            "[Task] problem_id: {}, task_id: {}, operation_type: {}, "
            "operands: ",
            task.problem_id(), task.task_id(), operation_type);

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
