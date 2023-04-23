#include <Application.h>

extern SPI_HandleTypeDef hspi1;

extern "C" void StartDefaultTask() {
	Application::getInstante().run();
}
