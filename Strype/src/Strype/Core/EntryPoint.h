extern Strype::Application* Strype::CreateApplication();

int main(int argc, char** argv)
{
	Strype::Log::Init();

	auto app = Strype::CreateApplication();
	app->Run();
	delete app;

	return 0;
}