#pragma once

#include <string>

namespace Strype {

	constexpr auto gen_crc32_table()
	{
		constexpr int num_bytes = 256;
		constexpr int num_iterations = 8;
		constexpr uint32_t polynomial = 0xEDB88320;

		std::array<uint32_t, num_bytes> crc32_table{};

		for (int byte = 0; byte < num_bytes; ++byte)
		{
			uint32_t crc = (uint32_t)byte;
			for (int i = 0; i < num_iterations; ++i)
			{
				int mask = -((int)crc & 1);
				crc = (crc >> 1) ^ (polynomial & mask);
			}

			crc32_table[byte] = crc;
		}

		return crc32_table;
	}

	static constexpr auto crc32_table = gen_crc32_table();
	static_assert(
		crc32_table.size() == 256 &&
		crc32_table[1] == 0x77073096 &&
		crc32_table[255] == 0x2D02EF8D,
		"gen_crc32_table generated unexpected result."
	);

	class Hash
	{
	public:
		static constexpr uint32_t GenerateFNVHash(std::string_view str)
		{
			constexpr uint32_t FNV_PRIME = 16777619u;
			constexpr uint32_t OFFSET_BASIS = 2166136261u;

			const size_t length = str.length();
			const char* data = str.data();

			uint32_t hash = OFFSET_BASIS;
			for (size_t i = 0; i < length; ++i)
			{
				hash ^= *data++;
				hash *= FNV_PRIME;
			}
			hash ^= '\0';
			hash *= FNV_PRIME;

			return hash;
		}

		static uint32_t CRC32(const char* str)
		{
			auto crc = 0xFFFFFFFFu;

			for (auto i = 0u; auto c = str[i]; ++i) {
				crc = crc32_table[(crc ^ c) & 0xFF] ^ (crc >> 8);
			}

			return ~crc;
		}

		static uint32_t CRC32(const std::string& string)
		{
			return CRC32(string.c_str());
		}
	};
	
}
