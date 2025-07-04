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
		uint64_t m_UUID;
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
		uint32_t m_UUID;
	};

}

namespace std {

	template <>
	struct hash<Strype::UUID>
	{
		std::size_t operator()(const Strype::UUID& uuid) const
		{
			// uuid is already a randomly generated number, and is suitable as a hash key as-is.
			// this may change in future, in which case return hash<uint64_t>{}(uuid); might be more appropriate
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
