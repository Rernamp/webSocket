#pragma once

#include <PinMap.h>

#include <Eni/Threading/Thread.h>
#include <Eni/Gpio/Gpio.h>

#include <W5500Spi.h>
#include <W5500Launcher.h>
#include <DFSDM.h>
#include <ConnectionsManager.h>

class Application {
public:
	Application();
	static Application& getInstante();
	void run();
	void dataOfMicrophoneCallback(bool isHalf);
	W5500Spi& getSpi() {
		return _w5500Spi;
	}


	UDA::W5500Launcher& getLauncher() {
		return _launcher;
	}

	UDA::Driver::DFSDMFilter& getFilter() {
		return _dfsdmF0;
	}

private:
	void ledProcess();

	void W5500ChipInit();
public:
	Eni::GpioPin _led {LED_GPIO_Port, LED_Pin};

	std::array<Eni::GpioPin, 4> chArray {Eni::GpioPin{CH0_GPIO_Port, CH0_Pin}, Eni::GpioPin{CH1_GPIO_Port, CH1_Pin},
		Eni::GpioPin{CH2_GPIO_Port, CH2_Pin}, Eni::GpioPin{CH3_GPIO_Port, CH3_Pin}};
private:
	Eni::GpioPin _cs {SPI_CS_GPIO_Port, SPI_CS_Pin};
	W5500Spi _w5500Spi;
	UDA::Driver::DFSDMFilter _dfsdmF0;
	UDA::Driver::DFSDMFilter _dfsdmF1;
	UDA::Driver::DFSDMFilter _dfsdmF2;
	UDA::Driver::DFSDMFilter _dfsdmF3;

	wiz_NetInfo gWIZNETINFO = { .mac = {0x00, 0x08, 0xdc, 0xab, 0xcd, 0xef},
	                            .ip = {192, 168, 3, 21},
	                            .sn = {255, 255, 255, 0},
	                            .gw = {192, 168, 88, 1},
	                            .dns = {0, 0, 0, 0},
	                            .dhcp = NETINFO_STATIC };

	static constexpr uint8_t _port = 80;
	static constexpr uint8_t _socketNumber = 2;

	UDA::W5500Launcher _launcher {_port, _socketNumber};

	UDA::ConnectionsManager _conManager {&_launcher.getTransmitter()};
};
