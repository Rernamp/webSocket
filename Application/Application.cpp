#include <Application.h>

Application& Application::getInstante() {
	static Application app;
	return app;
}

Application::Application() {
	Eni::Gpio::initOutput(_led);
}

void Application::run() {
	using namespace Eni;

	static constexpr std::size_t ledThreadStackSize = 256;
	auto ledThrea = new Threading::Thread("Led", ledThreadStackSize, Threading::ThreadPriority::Normal, [this]{
		ledProcess();
	});

	while(true) {

	}
}


void Application::ledProcess() {
	using namespace Eni;

	while(true) {
		Threading::ThisThread::sleepForMs(500);
		Gpio::toggle(_led);
	}

}
