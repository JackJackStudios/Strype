#include "stypch.h"
#include "ScriptEngine.h"

#include "ScriptGlue.h"

#include <Coral/TypeCache.hpp>

//Windows
#include <ShlObj.h>

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

	//Windows only
	void ScriptEngine::BuildProject(Ref<Project> proj)
	{
		std::filesystem::path filepath = proj->GetConfig().ProjectDirectory;
		std::string name = proj->GetConfig().ProjectDirectory.filename().string();

		STY_CORE_INFO("Building C# project '{}'", filepath.string());

		TCHAR programFilesFilePath[MAX_PATH];
		SHGetSpecialFolderPath(0, programFilesFilePath, CSIDL_PROGRAM_FILES, FALSE);
		std::filesystem::path msBuildPath = std::filesystem::path(programFilesFilePath) / "Microsoft Visual Studio" / "2022" / "Community" / "Msbuild" / "Current" / "Bin" / "MSBuild.exe";
		std::string command = std::format("cd \"{}\" && \"{}\" \"{}.csproj\" -property:Configuration={} -t:restore,build >nul 2>&1", (filepath / "strype").string(), msBuildPath.string(), name, STY_BUILD_CONFIG_NAME);
		system(command.c_str());
	}

	ScriptEngine::ScriptEngine(Ref<Project> proj)
	{
		m_AppAssembly.reset();

		auto filepath = proj->GetProjectDirectory() / "strype" / "Binaries" / (proj->GetProjectName() + ".dll");

		m_AppAssembly = std::make_unique<Coral::ManagedAssembly>(std::move(s_LoadContext->LoadAssembly(filepath.string())));

		if (m_AppAssembly->GetLoadStatus() != Coral::AssemblyLoadStatus::Success)
		{
			return;
		}

		BuildTypeCache(m_AppAssembly);
	}

	void ScriptEngine::BuildTypeCache(const Ref<Coral::ManagedAssembly>& assembly)
	{
		auto& types = assembly->GetTypes();
		auto& entityType = assembly->GetType("Strype.Object");

		for (auto& type : types)
		{
			std::string fullName = type->GetFullName();
			ScriptID scriptID = Hash::GenerateFNVHash(fullName);

			auto& metadata = m_ScriptMetadata[scriptID];
			metadata.FullName = fullName;
			metadata.Type = type;
		}
	}

	ScriptEngine::~ScriptEngine()
	{
		m_ScriptMetadata.clear();
		m_AppAssembly.reset();
	}

	void ScriptEngine::Initialize()
	{
		s_Host = std::make_unique<Coral::HostInstance>();

		Coral::HostSettings settings =
		{
			.CoralDirectory = (std::filesystem::current_path() / "DotNet").string(),
			.MessageCallback = OnCoralMessage,
			.ExceptionCallback = OnCSharpException
		};

		Coral::CoralInitStatus initStatus = s_Host->Initialize(settings);

		if (initStatus == Coral::CoralInitStatus::Success)
		{
			s_LoadContext = std::make_unique<Coral::AssemblyLoadContext>(std::move(s_Host->CreateAssemblyLoadContext("StrypeLoadContext")));

			auto scriptCorePath = (std::filesystem::current_path() / "DotNet" / "Strype-ScriptCore.dll").string();
			s_CoreAssembly = CreateRef<Coral::ManagedAssembly>(s_LoadContext->LoadAssembly(scriptCorePath));

			ScriptGlue::RegisterGlue(*s_CoreAssembly);
			return;
		}

		switch (initStatus)
		{
		case Coral::CoralInitStatus::CoralManagedNotFound:
		{
			auto message = std::format("Could not find Coral.Managed.dll in directory {}", settings.CoralDirectory);

#if defined(STY_PLATFORM_WINDOWS)
			int response = MessageBoxA(nullptr,
				message.c_str(),
				"Strype C# Scripting Engine Initialization Failure", MB_OK | MB_ICONERROR
			);
#else
			STY_CORE_ERROR("Strype C# Scripting Engine Initialization Failure: {}", message);
#endif
			break;
		}
		case Coral::CoralInitStatus::CoralManagedInitError:
		{
#if defined(STY_PLATFORM_WINDOWS)
			int response = MessageBoxA(nullptr,
				"Failed to initialize Coral.Managed",
				"Strype C# Scripting Engine Initialization Failure", MB_OK | MB_ICONERROR
			);
#else
			STY_CORE_ERROR("Strype C# Scripting Engine Initialization Failure: Failed to initialize Coral.Managed");
#endif
			break;
		}
		case Coral::CoralInitStatus::DotNetNotFound:
		{
#if defined(STY_PLATFORM_WINDOWS)
			int response = MessageBoxA(nullptr,
				"Strype requires .NET 8 or higher!\n\n"
				"Please make sure you have the appropriate .NET Runtime installed. Installers for all platforms can be found here: https://dotnet.microsoft.com/en-us/download/dotnet\n\n"
				"Would you like to download the latest .NET 8 Runtime installer?",
				"Strype C# Scripting Engine Initialization Failure",
				MB_YESNO | MB_ICONERROR
			);

			if (response == IDYES)
			{
				system("start https://aka.ms/dotnet/8.0/windowsdesktop-runtime-win-x64.exe");
			}
#else
			STY_CORE_ERROR(
				"Strype requires .NET 8 or higher!\n\n"
				"Please make sure you have the appropriate .NET Runtime installed. Installers for all platforms can be found here: https://dotnet.microsoft.com/en-us/download/dotnet\n\n"				"You can download the .NET installer here: https://dotnet.microsoft.com/en-us/download/dotnet."
			);

			system("open https://dotnet.microsoft.com/en-us/download/dotnet");
#endif
			break;
		}
		default:
			break;
		}

		// All of the above errors are fatal
		std::exit(-1);
	}

	void ScriptEngine::Shutdown()
	{
		s_CoreAssembly.reset();
		s_Host->UnloadAssemblyLoadContext(*s_LoadContext);

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