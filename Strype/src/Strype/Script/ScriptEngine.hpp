#pragma once

#include "Strype/Core/UUID.hpp"
#include "Strype/Core/Hash.hpp"

#include <Coral/HostInstance.hpp>
#include <Coral/Assembly.hpp>
#include <Coral/Type.hpp>
#include <Coral/Array.hpp>

namespace Strype {

	enum class DataType
	{
		Bool,
		Float,
		Int, UInt,
		Long, ULong,
		String,
		Vector2, Vector3, Vector4,
	};

	static std::unordered_map<std::string, DataType> s_DataTypeLookup = {
		{ "System.Boolean", DataType::Bool },
		{ "System.Single",  DataType::Float },
		{ "System.Int32",   DataType::Int },
		{ "System.UInt32",  DataType::UInt },
		{ "System.Int64",   DataType::Long },
		{ "System.UInt64",  DataType::ULong },
		{ "System.String",  DataType::String },
		{ "Strype.Vector2", DataType::Vector2 },
		{ "Strype.Vector3", DataType::Vector3 },
		{ "Strype.Vector4", DataType::Vector4 }
	};

	struct ScriptField
	{
		std::string Name;
		DataType Type;
		Coral::Type* ManagedType;

		Buffer DefaultValue;

	private:
		template<typename T>
		void SetDefaultValue(Coral::ManagedObject& temp)
		{
			if (ManagedType->IsSZArray())
			{
				auto value = temp.GetFieldValue<Coral::Array<T>>(Name);
				DefaultValue = Buffer::Copy(value.Data(), value.ByteLength());
				Coral::Array<T>::Free(value);
			}
			else
			{
				DefaultValue.Allocate(sizeof(T));
				auto value = temp.GetFieldValue<T>(Name);
				DefaultValue.Write(&value, sizeof(T));
			}
		}

		friend class ScriptEngine;
	};

	struct ScriptMetadata
	{
		Coral::Type* Type;
		std::string FullName;

		std::unordered_map<uint32_t, ScriptField> Fields;
	};

	using ScriptID = UUID;
	using ScriptMap = std::unordered_map<ScriptID, ScriptMetadata>;
	
	class Project;
	class ScriptEngine
	{
	public:
		ScriptEngine(Ref<Project> proj);

		void UnloadAssembly();
		void ReloadAssembly();

		const ScriptMetadata& GetScriptMetadata(ScriptID scriptID) const { return m_ScriptMetadata.at(scriptID); }
		const ScriptMap& GetAllScripts() const { return m_ScriptMetadata; }

		bool IsValidScript(ScriptID scriptID) const;
		bool IsValidScript(const std::string& name) const { return IsValidScript(GetIDByName(name)); }
		
		const ScriptField& GetField(ScriptID scriptID, const std::string& name) const { return GetScriptMetadata(scriptID).Fields.at(Hash::GenerateFNVHash(name)); }
		const std::string& GetScriptName(ScriptID scriptID) const { return GetScriptMetadata(scriptID).FullName; }
		const Coral::Type* GetTypeByName(const std::string& name) const { return m_ScriptMetadata.at(Hash::GenerateFNVHash(name)).Type; }
		const ScriptID GetIDByName(const std::string& name) const { return Hash::GenerateFNVHash(name); }


		template<typename... TArgs>
		Coral::ManagedObject* CreateInstance(ScriptID scriptID, TArgs&&... args)
		{
			auto* type = m_ScriptMetadata[scriptID].Type;
			auto instance = type->CreateInstance(std::forward<TArgs>(args)...);
			auto [index, handle] = m_ManagedObjects.Insert(std::move(instance));

			return &handle;
		}

		void DestroyInstance(Coral::ManagedObject*& instance)
		{
			instance->Destroy();
			instance = nullptr;
		}
	private:
		void BuildTypeCache(const Ref<Coral::ManagedAssembly>& assembly);
	private:
		bool m_IsInitizled = false;

		std::unique_ptr<Coral::HostInstance> s_Host;
		std::unique_ptr<Coral::AssemblyLoadContext> s_LoadContext;
		Ref<Coral::ManagedAssembly> s_CoreAssembly;

		Ref<Project> m_ActiveProject;
		Coral::StableVector<Coral::ManagedObject> m_ManagedObjects;
		Ref<Coral::ManagedAssembly> m_AppAssembly;
		ScriptMap m_ScriptMetadata;
	};

}