#pragma once

#include <Eni/Threading/Thread.h>

class Application {
public:
	Application();
	static Application& getInstante();
	void run();
private:
};
