#pragma once

#include "Base.hpp"

namespace Strype {

	class UUID
	{
	public:
		UUID();
		UUID(uint64_t uuid);
		UUID(const UUID& other);

		operator uint64_t () { return m_UUID; }
		operator const uint64_t() const { return m_UUID; }

		UUID& operator=(const UUID& other) { m_UUID = other.m_UUID; return *this; }
	private:
		uint64_t m_UUID = 0;
	};

	class UUID32
	{
	public:
		UUID32();
		UUID32(uint32_t uuid);
		UUID32(const UUID32& other);

		operator uint32_t () { return m_UUID; }
		operator const uint32_t() const { return m_UUID; }

		UUID32& operator=(const UUID32& other) { m_UUID = other.m_UUID; return *this; }
	private:
		uint32_t m_UUID = 0;
	};

}

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

namespace std {

	template <>
	struct hash<Strype::UUID>
	{
		std::size_t operator()(const Strype::UUID& uuid) const
		{
			return hash<uint64_t>()((uint64_t)uuid);
		}
	};

	template <>
	struct hash<Strype::UUID32>
	{
		std::size_t operator()(const Strype::UUID32& uuid) const
		{
			return hash<uint32_t>()((uint32_t)uuid);
		}
	};
}
