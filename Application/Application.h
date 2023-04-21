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

	UsbTrancferSocket& getTransfer() {
		return _transfer;
	}
private:
	void ledProcess();
public:
	Eni::GpioPin _led {LED_GPIO_Port, LED_Pin};
private:
	Eni::GpioPin _cs {SPI1_CS_GPIO_Port, SPI1_CS_Pin};
	W5500Spi _w5500Spi;
	std::array<uint8_t, 128> buffer;

	wiz_NetInfo gWIZNETINFO = { .mac = {0x00, 0x08, 0xdc, 0xab, 0xcd, 0xef},
	                            .ip = {192, 168, 3, 21},
	                            .sn = {255, 255, 255, 0},
	                            .gw = {192, 168, 88, 1},
	                            .dns = {0, 0, 0, 0},
	                            .dhcp = NETINFO_STATIC };
	UsbTrancferSocket _transfer {{192, 168, 3, 21}, 80, 1};
};
