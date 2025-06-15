#pragma once

#include "stypch.hpp"
#include "Strype/Core/Event.hpp"
#include "Strype/Core/UUID.hpp"

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

template <>
struct fmt::formatter<Strype::Event>
{
    constexpr auto parse(format_parse_context& ctx)
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(const Strype::Event& e, FormatContext& ctx) const
    {
        return fmt::format_to(ctx.out(), "{}", e.ToString());
    }
};

template <>
struct fmt::formatter<std::filesystem::path>
{
    constexpr auto parse(format_parse_context& ctx)
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(const std::filesystem::path& p, FormatContext& ctx) const
    {
        return fmt::format_to(ctx.out(), "{}", p.string());
    }
};

template <>
struct fmt::formatter<Strype::UUID>
{
    constexpr auto parse(format_parse_context& ctx)
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(const Strype::UUID& u, FormatContext& ctx) const
    {
        return fmt::format_to(ctx.out(), "{}", (uint64_t)u);
    }
};

template <>
struct fmt::formatter<Strype::UUID32>
{
    constexpr auto parse(format_parse_context& ctx)
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(const Strype::UUID32& u, FormatContext& ctx) const
    {
        return fmt::format_to(ctx.out(), "{}", (uint32_t)u);
    }
};