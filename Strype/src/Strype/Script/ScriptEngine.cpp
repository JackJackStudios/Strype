#include "stypch.hpp"
#include "ScriptEngine.hpp"

#include "ScriptGlue.hpp"

#include "Strype/Project/Project.hpp"
#include "Strype/Core/Application.hpp"

#include <Coral/TypeCache.hpp>

namespace Strype {

	static void OnCSharpException(std::string_view message)
	{
		STY_LOG_ERROR("C# Exception", message);
	}

	static void OnCoralMessage(std::string_view message, Coral::MessageLevel level)
	{
		switch (level)
		{
		case Coral::MessageLevel::Info: STY_LOG_TRACE("Script", message); break;
		case Coral::MessageLevel::Warning: STY_LOG_WARN("Script", message); break;
		case Coral::MessageLevel::Error: STY_LOG_ERROR("Script", message); break;
		}
	}

	ScriptEngine::ScriptEngine(Ref<Project> proj)
		: m_ActiveProject(proj)
	{
		ReloadAssembly();
	}

	ScriptEngine::~ScriptEngine()
	{
		UnloadAssembly();
	}

	void ScriptEngine::ReloadAssembly()
	{
		if (m_IsInitizled) UnloadAssembly();
		
		s_Host = std::make_unique<Coral::HostInstance>();

		Coral::HostSettings settings;
		settings.CoralDirectory = (std::filesystem::current_path() / "DotNet").string();
		settings.MessageCallback = OnCoralMessage;
		settings.ExceptionCallback = OnCSharpException;
		Coral::CoralInitStatus initStatus = s_Host->Initialize(settings);

		switch (initStatus)
		{
		case Coral::CoralInitStatus::CoralManagedNotFound:
			STY_LOG_ERROR("Script", "Could not find Coral.Managed.dll in directory {}", settings.CoralDirectory);
			break;

		case Coral::CoralInitStatus::CoralManagedInitError:
			STY_LOG_ERROR("Script", "Failed to initialize Coral.Managed");
			break;

		case Coral::CoralInitStatus::DotNetNotFound:
			STY_LOG_ERROR("Script", "Strype requires .NET 8 or higher!");
			break;
		}

		s_LoadContext = std::make_unique<Coral::AssemblyLoadContext>(std::move(s_Host->CreateAssemblyLoadContext("StrypeLoadContext")));
		s_CoreAssembly = CreateRef<Coral::ManagedAssembly>(s_LoadContext->LoadAssembly(settings.CoralDirectory + "/Strype-ScriptCore.dll"));

		RegisterInternalCalls();
		s_CoreAssembly->UploadInternalCalls();

		std::filesystem::path filepath = m_ActiveProject->GetProjectDirectory() / Project::HiddenFolder / "bin/net8.0" / (m_ActiveProject->GetProjectName() + ".dll");
		if (!std::filesystem::exists(filepath))
		{
			m_ActiveProject->BuildCSharp();
		}

		m_AppAssembly = std::make_unique<Coral::ManagedAssembly>(std::move(s_LoadContext->LoadAssembly(filepath.string())));
		
		if (m_AppAssembly->GetLoadStatus() != Coral::AssemblyLoadStatus::Success)
		{
			STY_LOG_ERROR("Script", "Error loading file: {}", filepath);
			return;
		}

		BuildTypeCache(m_AppAssembly);
		m_IsInitizled = true;
	}

	void ScriptEngine::UnloadAssembly()
	{
		m_ManagedObjects.Clear();

		for (auto& [scriptID, metadata] : m_ScriptMetadata)
		{
			for (auto& [fieldID, fieldMetadata] : metadata.Fields)
			{
				fieldMetadata.DefaultValue.Release();
			}
		}

		m_ScriptMetadata.clear();

		m_AppAssembly.reset();
		s_CoreAssembly.reset();

		s_Host->UnloadAssemblyLoadContext(*s_LoadContext);
		s_LoadContext.reset();

		s_Host->Shutdown();
		s_Host.reset();
	}

	void ScriptEngine::BuildTypeCache(const Ref<Coral::ManagedAssembly>& assembly)
	{
		auto& types = assembly->GetTypes();
		auto& objectType = assembly->GetType("Strype.Object");

		for (auto& type : types)
		{
			auto temp = type->CreateInstance();

			if (type->IsSubclassOf(objectType))
			{
				std::string fullName = type->GetFullName();
				ScriptID scriptID = Hash::GenerateFNVHash(fullName);

				auto& metadata = m_ScriptMetadata[scriptID];
				metadata.FullName = fullName;
				metadata.Type = type;

				for (auto& fieldInfo : type->GetFields())
				{
					Coral::ScopedString fieldName = fieldInfo.GetName();
					std::string fieldNameStr = fieldName;
					uint32_t fieldID = Hash::GenerateFNVHash(fieldNameStr);

					Coral::Type* fieldType = &fieldInfo.GetType();
					std::string typeName = fieldType->GetFullName();

					if (fieldNameStr == "ID" || fieldNameStr.find("k__BackingField") != std::string::npos)
						continue;

					if (!s_DataTypeLookup.contains(typeName))
					{
						STY_LOG_WARN("Script", "Unknown type on C# class ({}) \"{}\" ", fullName, typeName);
						continue;
					}

					if (fieldType->IsSZArray())
						fieldType = &fieldType->GetElementType();

					auto& fieldMetadata = metadata.Fields[fieldID];
					fieldMetadata.Name = fieldName;
					fieldMetadata.Type = s_DataTypeLookup.at(typeName);
					fieldMetadata.ManagedType = &fieldInfo.GetType();
					
					switch (fieldMetadata.Type)
					{
					case DataType::Bool:
						break;
					case DataType::Int:
						fieldMetadata.SetDefaultValue<int32_t>(temp);
						break;
					case DataType::UInt:
						fieldMetadata.SetDefaultValue<uint32_t>(temp);
						break;
					case DataType::Long:
						fieldMetadata.SetDefaultValue<int64_t>(temp);
						break;
					case DataType::ULong:
						fieldMetadata.SetDefaultValue<uint64_t>(temp);
						break;
					case DataType::Float:
						fieldMetadata.SetDefaultValue<float>(temp);
						break;
					case DataType::String:
						fieldMetadata.SetDefaultValue<Coral::String>(temp);
						break;
					case DataType::Vector2:
						fieldMetadata.SetDefaultValue<glm::vec2>(temp);
						break;
					case DataType::Vector3:
						fieldMetadata.SetDefaultValue<glm::vec3>(temp);
						break;
					case DataType::Vector4:
						fieldMetadata.SetDefaultValue<glm::vec4>(temp);
						break;
					}
				}
			}

			temp.Destroy();
		}
	}

	bool ScriptEngine::IsValidScript(ScriptID scriptID) const
	{
		if (!m_AppAssembly) return false;

		return scriptID != 0 && m_ScriptMetadata.contains(scriptID);
	}

}