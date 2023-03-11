#include <Application.h>

extern "C" void StartDefaultTask() {
	auto app = new Application();
	app->run();
}
