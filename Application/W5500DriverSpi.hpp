#pragma once

#include <W5500/Bus.hpp>

#include <W5500Spi.h>


class Bus : public W5500::Bus {
public:
	Bus(W5500Spi& spi) : _spi(spi) {

	}

	void init() override {
		W5500::Bus::init();
	}

	void spi_xfer(uint8_t send, uint8_t *recv) override {
		_spi.writeByte(send);

		*recv = _spi.readByte();
	}

	uint64_t millis() override {
		return 0;
	}


	void chip_select() override {
		_spi.select();
	}
    void chip_deselect() override {
    	_spi.unselect();
    }

private:
    W5500Spi& _spi;
};
