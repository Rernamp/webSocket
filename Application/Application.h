#pragma once

#include <PinMap.h>

#include <Eni/Threading/Thread.h>
#include <Eni/Gpio/Gpio.h>
#include <W5500Spi.h>
#include <UdpTrancferSocket.h>

class Application {
public:
	Application();
	static Application& getInstante();
	void run();
	W5500Spi& getSpi() {
		return _w5500Spi;
	}
private:
	void ledProcess();
	Eni::GpioPin _led {LED_GPIO_Port, LED_Pin};
	Eni::GpioPin _cs {SPI1_CS_GPIO_Port, SPI1_CS_Pin};
	W5500Spi _w5500Spi;
	std::array<uint8_t, 128> buffer;
};
