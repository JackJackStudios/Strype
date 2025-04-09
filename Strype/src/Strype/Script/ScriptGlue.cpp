#include "stypch.h"
#include "ScriptGlue.h"

#include "Strype/Room/Room.h"
#include "Strype/Room/Object.h"
#include "Strype/Room/Components.h"

#include "Strype/Project/Project.h"

#define STY_ADD_INTERNAL_CALL(icall) coreAssembly.AddInternalCall("Strype.InternalCalls", #icall, (void*)InternalCalls::icall)

namespace Strype {

	std::unordered_map<Coral::TypeId, std::function<void(Object&)>> s_CreateComponentFuncs;
	std::unordered_map<Coral::TypeId, std::function<bool(Object&)>> s_HasComponentFuncs;
	std::unordered_map<Coral::TypeId, std::function<void(Object&)>> s_RemoveComponentFuncs;

	template<typename TComponent>
	static void RegisterManagedComponent(Coral::ManagedAssembly& coreAssembly)
	{
		// NOTE(Peter): Get the demangled type name of TComponent
		const TypeNameString& componentTypeName = TypeInfo<TComponent, true>().Name();
		std::string componentName = std::format("Strype.{}", componentTypeName);

		auto& type = coreAssembly.GetType(componentName);

		if (type)
		{
			s_CreateComponentFuncs[type.GetTypeId()] = [](Object& entity) { entity.AddComponent<TComponent>(); };
			s_HasComponentFuncs[type.GetTypeId()] = [](Object& entity) { return entity.HasComponent<TComponent>(); };
			s_RemoveComponentFuncs[type.GetTypeId()] = [](Object& entity) { entity.RemoveComponent<TComponent>(); };
		}
		else
		{
			HZ_CORE_VERIFY(false, "No C# component class found for {}!", componentName);
		}
	}

	void ScriptGlue::RegisterGlue(Coral::ManagedAssembly& coreAssembly)
	{
		if (!s_CreateComponentFuncs.empty())
		{
			s_CreateComponentFuncs.clear();
			s_HasComponentFuncs.clear();
			s_RemoveComponentFuncs.clear();
		}

		RegisterComponentTypes(coreAssembly);
		RegisterInternalCalls(coreAssembly);

		coreAssembly.UploadInternalCalls();
	}

	void ScriptGlue::RegisterComponentTypes(Coral::ManagedAssembly& coreAssembly)
	{
		RegisterManagedComponent<Transform>(coreAssembly);
	}

	void ScriptGlue::RegisterInternalCalls(Coral::ManagedAssembly& coreAssembly)
	{
		STY_ADD_INTERNAL_CALL(Log_LogMessage);
	}

	namespace InternalCalls {

		void Log_LogMessage(LogLevel level, Coral::String fmt)
		{
			std::string message = fmt;
			switch (level)
			{
			case LogLevel::Trace:
				STY_TRACE(message);
				break;
			case LogLevel::Info:
				STY_INFO(message);
				break;
			case LogLevel::Warn:
				STY_WARN(message);
				break;
			case LogLevel::Error:
				STY_ERROR(message);
				break;
			case LogLevel::Critical:
				STY_CRITICAL(message);
				break;
			}
			Coral::String::Free(fmt);
		}

	}

}