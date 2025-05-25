extern Strype::Application* Strype::CreateApplication(ApplicationArguments args);

int main(int argc, char** argv)
{
	Strype::Log::Init();

	auto app = Strype::CreateApplication({ argc, argv });
	app->SetArgs({ argc, argv });
	app->Run();
	delete app;

	return 0;
}