#pragma once

#include <PinMap.h>

#include <Eni/Threading/Thread.h>
#include <Eni/Gpio/Gpio.h>


class Application {
public:
	Application();
	static Application& getInstante();
	void run();
private:
	void ledProcess();
	Eni::GpioPin _led {LED_GPIO_Port, LED_Pin};
};
