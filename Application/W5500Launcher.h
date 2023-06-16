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
		W5500Launcher(uint8_t socketNumber) : _socketNumber(socketNumber) {
		}

		void init() {
			static constexpr std::size_t StackSize = 1024;

			_transmitProcess = Eni::Threading::Thread("Transmitt", StackSize, Eni::Threading::ThreadPriority::Normal, [this] {
				while(true) {

					if (openSocket()) {
						_transmitter.process();
					}

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
		bool closeSocket() {
			

			close(_socketNumber);
		}

		bool openSocket() {
			using namespace Eni;

			auto socketNumber = socket(_socketNumber, Sn_MR_TCP, _port, SF_TCP_NODELAY);
			bool result = (socketNumber == _socketNumber);				

			if (result) {
				while(getSn_SR(CLIENT_SOCKET) != SOCK_INIT) {
					Threading::ThisThread::sleepForMs(10);
				}

				result &= listen(_socketNumber) == SOCK_OK;				
			}

			if (result) {
				enableInterrupt();
				while(getSn_SR(CLIENT_SOCKET) == SOCK_LISTEN) {
					Threading::ThisThread::sleepForMs(10);
				}

				result &= (getSn_SR(CLIENT_SOCKET) == SOCK_ESTABLISHED);
			}


			return result;

		}

		void enableInterrupt() {
			setSn_IMR(_socketNumber, Sn_IMR_RECV);
			setSIMR(1 << _socketNumber);
		}
		
		Eni::Threading::Thread _transmitProcess {};
		Eni::Threading::Thread _receiveProcess {};

		uint8_t _socketNumber;

		W5500::Transmitter _transmitter {_socketNumber};
		W5500::Receiver _receiver {_socketNumber};


		std::array<uint8_t, 4> _hostIp {}; 
		uint16_t _hostPort {}; 

	};
}
