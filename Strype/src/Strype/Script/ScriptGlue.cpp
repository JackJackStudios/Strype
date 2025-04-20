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
			STY_CORE_ERROR("No C# component class found for {}!", componentName);
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
		STY_ADD_INTERNAL_CALL(Object_CreateComponent);
		STY_ADD_INTERNAL_CALL(Object_HasComponent);
		STY_ADD_INTERNAL_CALL(Object_RemoveComponent);
		STY_ADD_INTERNAL_CALL(Room_ObjectExists);
		STY_ADD_INTERNAL_CALL(Room_DestroyObject);
		STY_ADD_INTERNAL_CALL(Transform_GetPosition);
		STY_ADD_INTERNAL_CALL(Transform_SetPosition);
		STY_ADD_INTERNAL_CALL(Transform_GetRotation);
		STY_ADD_INTERNAL_CALL(Transform_SetRotation);
		STY_ADD_INTERNAL_CALL(Transform_GetScale);
		STY_ADD_INTERNAL_CALL(Transform_SetScale);
		STY_ADD_INTERNAL_CALL(Log_LogMessage);
	}

	namespace InternalCalls {

		void Object_CreateComponent(uint32_t id, Coral::ReflectionType componentType)
		{
			Object obj = Object{ (entt::entity)id, Project::GetActiveRoom().get() };

			Coral::Type& type = componentType;

			if (auto it = s_HasComponentFuncs.find(type.GetTypeId()); it != s_HasComponentFuncs.end() && it->second(obj))
				return;

			if (auto it = s_CreateComponentFuncs.find(type.GetTypeId()); it != s_CreateComponentFuncs.end())
				it->second(obj);
		}

		bool Object_HasComponent(uint32_t id, Coral::ReflectionType componentType)
		{
			Object obj = Object{ (entt::entity)id, Project::GetActiveRoom().get() };

			Coral::Type& type = componentType;

			STY_CORE_INFO("Hello");

			if (auto it = s_HasComponentFuncs.find(type.GetTypeId()); it != s_HasComponentFuncs.end())
			{
				auto func = it->second;
				return func(obj);
			}

			return false;
		}

		bool Object_RemoveComponent(uint32_t id, Coral::ReflectionType componentType)
		{
			Object obj = Object{ (entt::entity)id, Project::GetActiveRoom().get() };

			Coral::Type& type = componentType;

			if (auto it = s_HasComponentFuncs.find(type.GetTypeId()); it == s_HasComponentFuncs.end() || !it->second(obj))
				return false;

			if (auto it = s_RemoveComponentFuncs.find(type.GetTypeId()); it != s_RemoveComponentFuncs.end())
			{
				it->second(obj);
				return true;
			}

			return false;
		}

		bool Room_ObjectExists(uint32_t id)
		{
			return Project::GetActiveRoom()->ObjectExists(Object{ (entt::entity)id, Project::GetActiveRoom().get() });
		}

		void Room_DestroyObject(uint32_t id)
		{
			return Project::GetActiveRoom()->DestroyObject(Object{ (entt::entity)id, Project::GetActiveRoom().get() });
		}

		void Transform_GetPosition(uint32_t id, glm::vec2* outPosition)
		{
			Object obj = Object{ (entt::entity)id, Project::GetActiveRoom().get() };
			*outPosition = obj.GetComponent<Transform>().Position;
		}											 
													 
		void Transform_SetPosition(uint32_t id, glm::vec2* inPosition)
		{
			Object obj = Object{ (entt::entity)id, Project::GetActiveRoom().get() };
			obj.GetComponent<Transform>().Position = *inPosition;
		}

		void Transform_GetRotation(uint32_t id, float* outRotation)
		{
			Object obj = Object{ (entt::entity)id, Project::GetActiveRoom().get() };
			*outRotation = obj.GetComponent<Transform>().Rotation;
		}

		void Transform_SetRotation(uint32_t id, float* inRotation)
		{
			Object obj = Object{ (entt::entity)id, Project::GetActiveRoom().get() };
			obj.GetComponent<Transform>().Rotation = *inRotation;
		}

		void Transform_GetScale(uint32_t id, glm::vec2* outScale)
		{
			Object obj = Object{ (entt::entity)id, Project::GetActiveRoom().get() };
			*outScale = obj.GetComponent<Transform>().Scale;
		}

		void Transform_SetScale(uint32_t id, glm::vec2* inScale)
		{
			Object obj = Object{ (entt::entity)id, Project::GetActiveRoom().get() };
			obj.GetComponent<Transform>().Scale = *inScale;
		}

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
			}
			Coral::String::Free(fmt);
		}

	}

}