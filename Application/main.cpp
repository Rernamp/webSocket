#include <Application.h>

extern SPI_HandleTypeDef hspi1;

extern "C" void StartDefaultTask() {
	auto app = new Application();
	app->run();
}
