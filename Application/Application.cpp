#include <Application.hpp>

#include <stdio.h>
#include <string.h>


Application& Application::getInstante() {
	static Application app;
	return app;
}

extern SPI_HandleTypeDef hspi1;
Application::Application() : _w5500Spi(hspi1, _cs) {
	Eni::Gpio::initOutput(_led);
}

void Application::run() {
	using namespace Eni;

	static constexpr std::size_t ledThreadStackSize = 256;
	auto ledThrea = new Threading::Thread("Led", ledThreadStackSize, Threading::ThreadPriority::Normal, [this]{
		ledProcess();
	});

	HAL_GPIO_WritePin(RST_GPIO_Port, RST_Pin, GPIO_PIN_RESET);
	Threading::ThisThread::sleepForMs(1);
	HAL_GPIO_WritePin(RST_GPIO_Port, RST_Pin, GPIO_PIN_SET);
	Threading::ThisThread::sleepForMs(1000);



//	_w5500.reset();

	_w5500.set_ip(const_cast<uint8_t*>(_ip.data()));
	_w5500.set_mac(const_cast<uint8_t*>(_mac.data()));
	_w5500.set_gateway(const_cast<uint8_t*>(_gateWayMask.data()));
	_w5500.set_subnet_mask(const_cast<uint8_t*>(_subnetMask.data()));

	_w5500.init();

	_socket.set_source_port(_port);


    // Read the interrupt flags for the socket
    auto flags = _socket.get_interrupt_flags();

    // Socket has just connected - handle our connection callback
    if (flags & W5500::Registers::Socket::InterruptFlags::CONNECT) {
        _socket.clear_interrupt_flag(W5500::Registers::Socket::InterruptFlags::CONNECT);

        // Handle application level logic on connection
    }

    // Disconnect - close and try again
    if (flags & W5500::Registers::Socket::InterruptFlags::DISCONNECT) {
        _socket.close();
        _socket.clear_interrupt_flag(W5500::Registers::Socket::InterruptFlags::DISCONNECT);
    }

    // Timeout - reset socket and try again
    if (flags & W5500::Registers::Socket::InterruptFlags::TIMEOUT) {
        _socket.close();
        _socket.clear_interrupt_flag(W5500::Registers::Socket::InterruptFlags::TIMEOUT);
    }


	if (!_socket.ready()) {
		if (!_socket.init()) {
			_socket.close();

		}
	}



	while(true) {
		Threading::ThisThread::sleepForMs(200);
	}
}


void Application::ledProcess() {
	using namespace Eni;

	while(true) {
		Threading::ThisThread::sleepForMs(500);
		Gpio::toggle(_led);
	}

}

