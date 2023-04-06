#pragma once

#include <PinMap.h>

#include <Eni/Threading/Thread.h>
#include <Eni/Gpio/Gpio.h>


#include <W5500Spi.h>

#include <W5500DriverSpi.hpp>
#include <W5500/W5500.hpp>
#include <W5500/Socket.hpp>

class Application {
public:
	Application();
	static Application& getInstante();
	void run();
private:
	void ledProcess();
	Eni::GpioPin _led {LED_GPIO_Port, LED_Pin};
	Eni::GpioPin _cs {SPI1_CS_GPIO_Port, SPI1_CS_Pin};

	W5500Spi _w5500Spi;
	Bus _bus {_w5500Spi};
	W5500::W5500 _w5500 {_bus};

	static constexpr uint8_t _udpSocketNumber = 1;
	W5500::UdpSocket _socket {_w5500, _udpSocketNumber};

	const std::array<uint8_t, 4> _ip {192, 168, 3, 207};
	const std::array<uint8_t, 4> _subnetMask {255, 255, 255, 0};
	const std::array<uint8_t, 4> _gateWayMask {192, 168, 88, 1};
	const std::array<uint8_t, 6> _mac {0x00, 0x08, 0xdc, 0xab, 0xcd, 0xef};
	static constexpr uint16_t _port = 80;

};
