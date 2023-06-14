#pragma once

#include "ioLibrary_Driver/Ethernet/socket.h"

#include <Eni/Threading/Semaphore.h>
#include <Eni/Threading/Thread.h>
#include <Eni/Debug/Assert.h>

#include "W5500Receiver.h"
#include "W5500Transmitter.h"


namespace UDA {
	class W5500Launcher {
	public:
		W5500Launcher(std::array<uint8_t, 4> ip, uint16_t port, uint8_t socketNumber) : _ip(ip), _port(port), _socketNumber(socketNumber) {
		}

		void init() {
			static constexpr std::size_t StackSize = 1024;

			_transmitProcess = Eni::Threading::Thread("Transmitt", StackSize, Eni::Threading::ThreadPriority::Normal, [this] {
				while(true) {
					openSocket();

					_transmitter.process();

					closeSocket();
				}
			});

			_receiveProcess = Eni::Threading::Thread("Receive", StackSize, Eni::Threading::ThreadPriority::Normal, [this] {
				while(true) {
					_receiver.process();
				}
			});
		}



		W5500::ITransmitter& getTransmitter() {
			return _transmitter;
		}

		W5500::Receiver& getReceiver() {
			return _receiver;
		}
	private:
		void closeSocket() {
			#warning "implement this"
		}

		void openSocket() {
			#warning "implement this"
		}
		W5500::Transmitter _transmitter {};
		W5500::Receiver _receiver {};

		Eni::Threading::Thread _transmitProcess {};
		Eni::Threading::Thread _receiveProcess {};

		std::array<uint8_t, 4> _ip; 
		uint16_t _port; 
		uint8_t _socketNumber;

		std::array<uint8_t, 4> _hostIp {}; 
		uint16_t _hostPort {}; 

	};
}
