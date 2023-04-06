#include <Application.hpp>

extern SPI_HandleTypeDef hspi1;

extern "C" void StartDefaultTask() {
	Application* app = new Application();
	app->run();
}
