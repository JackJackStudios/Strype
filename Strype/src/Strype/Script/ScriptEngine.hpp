#pragma once

#include "Strype/Project/Project.hpp"

#include "Strype/Core/UUID.hpp"
#include "Strype/Core/Hash.hpp"

#include "CSharpObject.hpp"

#include <Coral/HostInstance.hpp>
#include <Coral/Assembly.hpp>
#include <Coral/Type.hpp>

namespace Strype {

	enum class DataType
	{
		Bool,
		Float,
		Int, UInt,
		Long, ULong,
		Vector2, Vector3, Vector4,
	};

	static std::unordered_map<std::string, DataType> s_DataTypeLookup = {
		{ "System.Boolean", DataType::Bool },
		{ "System.Single",  DataType::Float },
		{ "System.Int32",   DataType::Int },
		{ "System.UInt32",  DataType::UInt },
		{ "System.Int64",   DataType::Long },
		{ "System.UInt64",  DataType::ULong },
		{ "Strype.Vector2",  DataType::Vector2 },
		{ "Strype.Vector3",  DataType::Vector3 },
		{ "Strype.Vector4",  DataType::Vector4 }
	};

	struct ScriptField
	{
		std::string Name;
		DataType Type;
	};

	struct ScriptMetadata
	{
		Coral::Type* Type;
		std::string FullName;

		std::unordered_map<uint32_t, ScriptField> Fields;
	};

	using ScriptID = UUID;
	using ScriptMap = std::unordered_map<ScriptID, ScriptMetadata>;

	class ScriptEngine
	{
	public:
		ScriptEngine(Ref<Project> proj);
		~ScriptEngine();

		static void Initialize();
		static void Shutdown();

		static void BuildProject(const std::filesystem::path& path);

		const ScriptMetadata& GetScriptMetadata(ScriptID scriptID) const { return m_ScriptMetadata.at(scriptID); }
		const ScriptMap& GetAllScripts() const { return m_ScriptMetadata; }

		bool IsValidScript(ScriptID scriptID) const;
		bool IsValidScript(const std::string& name) const { return IsValidScript(GetIDByName(name)); }
		
		const ScriptField& GetField(ScriptID scriptID, const std::string& name) const { return GetScriptMetadata(scriptID).Fields.at(Hash::GenerateFNVHash(name)); }
		const std::string& GetScriptName(ScriptID scriptID) const { return GetScriptMetadata(scriptID).FullName; }
		const Coral::Type* GetTypeByName(const std::string& name) const { return m_ScriptMetadata.at(Hash::GenerateFNVHash(name)).Type; }
		const ScriptID GetIDByName(const std::string& name) const { return Hash::GenerateFNVHash(name); }

		template<typename... TArgs>
		CSharpObject CreateInstance(ScriptID scriptID, TArgs&&... args)
		{
			auto* type = m_ScriptMetadata[scriptID].Type;
			auto instance = type->CreateInstance(std::forward<TArgs>(args)...);
			auto [index, handle] = m_ManagedObjects.Insert(std::move(instance));

			CSharpObject result;
			result.m_Handle = &handle;
			return result;
		}

		void DestroyInstance(CSharpObject& instance)
		{
			instance.m_Handle->Destroy();
			instance.m_Handle = nullptr;
		}
	private:
		void BuildTypeCache(const Ref<Coral::ManagedAssembly>& assembly);
	private:
		inline static std::unique_ptr<Coral::HostInstance> s_Host;
		inline static std::unique_ptr<Coral::AssemblyLoadContext> s_LoadContext;
		inline static Ref<Coral::ManagedAssembly> s_CoreAssembly;

		Coral::StableVector<Coral::ManagedObject> m_ManagedObjects;
		Ref<Coral::ManagedAssembly> m_AppAssembly;
		ScriptMap m_ScriptMetadata;
	};

}