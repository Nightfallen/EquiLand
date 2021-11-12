// EquiLand.cpp : Defines the entry point for the application.
//

#include <source/Application.h>
#include <source/gui.hpp>

int main(int, char**)
{
	Application* app = new Application();
	app->SetUIHandler(UI_HANDLER);
	app->Run();
	return 0;
}