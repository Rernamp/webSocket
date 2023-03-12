#pragma once

#include <Eni/Gpio/Gpio.h>

class W5500Spi {
public:
	W5500Spi(SPI_HandleTypeDef hspi, Eni::GpioPin cs) : _cs(cs), _hspi(hspi) {
		Eni::Gpio::initOutput(_cs);
	}

	void select(void)
	{
		Eni::Gpio::reset(_cs);
	}

	void unselect(void)
	{
		Eni::Gpio::set(_cs);
	}

	void readBuff(uint8_t* buff, uint16_t len)
	{
		HAL_SPI_Receive(&_hspi, buff, len, HAL_MAX_DELAY);
	}

	void writeBuff(uint8_t* buff, uint16_t len)
	{
		HAL_SPI_Transmit(&_hspi, buff, len, HAL_MAX_DELAY);
	}

	uint8_t readByte(void)
	{
	    uint8_t byte;
	    readBuff(&byte, sizeof(byte));
	    return byte;
	}

	void writeByte(uint8_t byte)
	{
		writeBuff(&byte, sizeof(byte));
	}

private:
	Eni::GpioPin _cs;
	SPI_HandleTypeDef _hspi;

};
