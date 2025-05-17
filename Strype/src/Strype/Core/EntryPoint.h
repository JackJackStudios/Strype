extern Strype::Application* Strype::CreateApplication();

int main(int argc, char** argv)
{
	Strype::Log::Init();

	auto app = Strype::CreateApplication();
	app->SetArgs({ argc, argv });
	app->Run();
	delete app;

	return 0;
}