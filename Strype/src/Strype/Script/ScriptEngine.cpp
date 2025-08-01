#include "stypch.hpp"
#include "ScriptEngine.hpp"

#include "ScriptGlue.hpp"

#include "Strype/Core/Application.hpp"

#include <Coral/TypeCache.hpp>

namespace Strype {

	static void OnCSharpException(std::string_view message)
	{
		STY_CORE_ERROR("C# Exception: {}", message);
	}

	static void OnCoralMessage(std::string_view message, Coral::MessageLevel level)
	{
		switch (level)
		{
		case Coral::MessageLevel::Info:
			STY_CORE_INFO("{}", std::string(message));
			break;
		case Coral::MessageLevel::Warning:
			STY_CORE_WARN("{}", std::string(message));
			break;
		case Coral::MessageLevel::Error:
			STY_CORE_ERROR("{}", std::string(message));
			break;
		}
	}

	ScriptEngine::ScriptEngine(Ref<Project> proj)
	{
		m_AppAssembly.reset();

		std::filesystem::path filepath = proj->GetProjectDirectory() / HIDDEN_FOLDER / "bin/net8.0" / (proj->GetProjectName() + ".dll");
		m_AppAssembly = std::make_unique<Coral::ManagedAssembly>(std::move(s_LoadContext->LoadAssembly(filepath.string())));

		if (m_AppAssembly->GetLoadStatus() == Coral::AssemblyLoadStatus::Success)
		{
			BuildTypeCache(m_AppAssembly);
			return;
		}

		STY_CORE_ERROR("Error loading file: {}", filepath);
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
					Coral::ScopedString typeName = fieldType->GetFullName();

					if (fieldNameStr == "ID" || !s_DataTypeLookup.contains(typeName) || fieldNameStr.find("k__BackingField") != std::string::npos)
						continue;

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

	ScriptEngine::~ScriptEngine()
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
	}

	void ScriptEngine::Initialize()
	{
		s_Host = std::make_unique<Coral::HostInstance>();

		Coral::HostSettings settings;
		settings.CoralDirectory = (Application::Get().GetConfig().MasterDir / "DotNet").string();
		settings.MessageCallback = OnCoralMessage;
		settings.ExceptionCallback = OnCSharpException;
		Coral::CoralInitStatus initStatus = s_Host->Initialize(settings);

		if (initStatus == Coral::CoralInitStatus::Success)
		{
			s_LoadContext = std::make_unique<Coral::AssemblyLoadContext>(std::move(s_Host->CreateAssemblyLoadContext("StrypeLoadContext")));

			auto scriptCorePath = (Application::Get().GetConfig().MasterDir / "DotNet" / "Strype-ScriptCore.dll").string();
			s_CoreAssembly = CreateRef<Coral::ManagedAssembly>(s_LoadContext->LoadAssembly(scriptCorePath));

			ScriptGlue::RegisterGlue(*s_CoreAssembly);
			return;
		}

		switch (initStatus)
		{
		case Coral::CoralInitStatus::CoralManagedNotFound:
			STY_CORE_ERROR("Could not find Coral.Managed.dll in directory {}", settings.CoralDirectory);
			break;

		case Coral::CoralInitStatus::CoralManagedInitError:
			STY_CORE_ERROR("Failed to initialize Coral.Managed");
			break;

		case Coral::CoralInitStatus::DotNetNotFound:
			STY_CORE_ERROR("Strype requires .NET 8 or higher!");
			break;
		}

		// All of the above errors are fatal
		std::exit(-1);
	}

	void ScriptEngine::Shutdown()
	{
		s_CoreAssembly.reset();

		s_Host->UnloadAssemblyLoadContext(*s_LoadContext);
		s_LoadContext.reset();

		s_Host->Shutdown();
		s_Host.reset();
	}

	bool ScriptEngine::IsValidScript(ScriptID scriptID) const
	{
		if (!m_AppAssembly)
			return false;

		return m_ScriptMetadata.contains(scriptID);
	}

}