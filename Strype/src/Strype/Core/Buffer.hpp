#pragma once

#include "stypch.hpp"

namespace Strype {

	struct Buffer
	{
		void* Data = nullptr;
		uint64_t Size = 0;

		Buffer() = default;

		Buffer(const void* data, uint64_t size = 0)
			: Data((void*)data), Size(size) 
		{
		}

		static Buffer Copy(const Buffer& other)
		{
			Buffer buffer;
			buffer.Allocate(other.Size);
			memcpy(buffer.Data, other.Data, other.Size);
			return buffer;
		}

		static Buffer Copy(const void* data, uint64_t size)
		{
			Buffer buffer;
			buffer.Allocate(size);
			memcpy(buffer.Data, data, size);
			return buffer;
		}

		void Allocate(uint64_t size)
		{
			delete[](uint8_t*)Data;
			Data = nullptr;
			Size = size;

			if (size == 0)
				return;

			Data = new uint8_t[size];
		}

		void Release()
		{
			delete[](uint8_t*)Data;
			Data = nullptr;
			Size = 0;
		}

		void ZeroInitialize()
		{
			if (Data)
				memset(Data, 0, Size);
		}

		template<typename T>
		T& Read(uint64_t offset = 0)
		{
			return *(T*)((uint8_t*)Data + offset);
		}

		template<typename T>
		const T& Read(uint64_t offset = 0) const
		{
			return *(T*)((uint8_t*)Data + offset);
		}

		uint8_t* ReadBytes(uint64_t size, uint64_t offset) const
		{
			STY_CORE_VERIFY(offset + size <= Size, "Buffer overflow!");
			uint8_t* buffer = new uint8_t[size];
			memcpy(buffer, (uint8_t*)Data + offset, size);
			return buffer;
		}

		void Write(const void* data, uint64_t size, uint64_t offset = 0)
		{
			STY_CORE_VERIFY(offset + size <= Size, "Buffer overflow!");
			memcpy((uint8_t*)Data + offset, data, size);
		}

		operator bool() const
		{
			return (bool)Data;
		}

		uint8_t& operator[](int index)
		{
			return ((uint8_t*)Data)[index];
		}

		uint8_t operator[](int index) const
		{
			return ((uint8_t*)Data)[index];
		}

		template<typename T>
		T* As() const
		{
			return (T*)Data;
		}

		inline uint64_t GetSize() const { return Size; }
	};

}