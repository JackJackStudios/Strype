#pragma once

#include <Coral/Assembly.hpp>
#include <Coral/String.hpp>
#include <Coral/Array.hpp>

#include "TypeInfo.h"

namespace Strype {

	class ScriptGlue
	{
	public:
		static void RegisterGlue(Coral::ManagedAssembly& coreAssembly);

	private:
		static void RegisterComponentTypes(Coral::ManagedAssembly& coreAssembly);
		static void RegisterInternalCalls(Coral::ManagedAssembly& coreAssembly);
	};

	namespace InternalCalls {

		template<std::default_initializable T>
		struct Param
		{
			std::byte Data[sizeof(T)];

			operator T() const
			{
				T result;
				std::memcpy(&result, Data, sizeof(T));
				return result;
			}
		};

		template<typename T>
		struct OutParam
		{
			std::byte* Ptr = nullptr;

			T* operator->() noexcept { return reinterpret_cast<T*>(Ptr); }
			const T* operator->() const noexcept { return reinterpret_cast<const T*>(Ptr); }

			T& operator*() { return *reinterpret_cast<T*>(Ptr); }
			const T& operator*() const { return *reinterpret_cast<const T*>(Ptr); }
		};

		enum class LogLevel : int32_t
		{
			Trace = BIT(0),
			Info = BIT(2),
			Warn = BIT(3),
			Error = BIT(4),
			Critical = BIT(5)
		};
		
		void Log_LogMessage(LogLevel level, Coral::String fmt);

	}

}