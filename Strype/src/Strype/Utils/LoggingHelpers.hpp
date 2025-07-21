#pragma once

#include "stypch.hpp"

#include <glm/glm.hpp>

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

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

template <typename T, size_t N>
struct fmt::formatter<std::array<T, N>>
{
    constexpr auto parse(format_parse_context& ctx)
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(const std::array<T, N>& arr, FormatContext& ctx) const
    {
        auto out = fmt::format_to(ctx.out(), "[");

        for (size_t i = 0; i < N; ++i) 
        {
            out = fmt::format_to(out, "{}", arr[i]);

            if (i + 1 != N)
                out = fmt::format_to(out, ", ");
        }

        return fmt::format_to(out, "]");
    }
};

template <typename T>
struct fmt::formatter<std::vector<T>> 
{
    constexpr auto parse(format_parse_context& ctx) 
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(const std::vector<T>& vec, FormatContext& ctx) const
    {
        auto out = fmt::format_to(ctx.out(), "[");

        for (size_t i = 0; i < vec.size(); ++i) 
        {
            out = fmt::format_to(out, "{}", vec[i]);

            if (i + 1 != vec.size())
                out = fmt::format_to(out, ", ");
        }

        return fmt::format_to(out, "]");
    }
};

template <>
struct fmt::formatter<glm::vec2>
{
    constexpr auto parse(format_parse_context& ctx)
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(const glm::vec2& v, FormatContext& ctx) const
    {
        return fmt::format_to(ctx.out(), "({}, {})", v.x, v.y);
    }
};

template <>
struct fmt::formatter<glm::vec3>
{
    constexpr auto parse(format_parse_context& ctx)
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(const glm::vec3& v, FormatContext& ctx) const
    {
        return fmt::format_to(ctx.out(), "({}, {}, {})", v.x, v.y, v.z);
    }
};

template <>
struct fmt::formatter<glm::vec4>
{
    constexpr auto parse(format_parse_context& ctx)
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(const glm::vec4& v, FormatContext& ctx) const
    {
        return fmt::format_to(ctx.out(), "({}, {}, {}, {})", v.x, v.y, v.z, v.w);
    }
};