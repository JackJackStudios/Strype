#pragma once

#include "stypch.hpp"

namespace Strype {

	struct Buffer
	{
		void* Data = 0; // Pointer
		uint64_t Size = 0;

		Buffer() = default;

		Buffer(const void* data, uint64_t size)
			: Data((void*)data), Size(size) 
		{
		}

		template<typename T>
		Buffer(const T& value)
			: Data((void*)&value), Size(sizeof(T))
		{
		}

		Buffer(uint64_t size)
			: Size(size)
		{
			Allocate(Size);
			ZeroInitialize();
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
			Release();

			if (size == 0)
				return;

			Size = size;
			Data = malloc(size);
		}

		void Release()
		{
			free(Data);

			Data = nullptr;
			Size = 0;
		}

		void ZeroInitialize()
		{
			if (Data)
				memset(Data, 0, Size);
		}

		bool Empty() const
		{
			if (!Data || Size == 0)
				return true;

			return false;
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