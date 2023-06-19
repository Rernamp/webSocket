#pragma once

#include "ioLibrary_Driver/Ethernet/socket.h"

#include <Eni/Threading/Semaphore.h>
#include <Eni/Threading/Thread.h>

#include "W5500Receiver.h"
#include "W5500Transmitter.h"


namespace UDA {
	class W5500Launcher {
	public:
		W5500Launcher(uint8_t port, uint8_t socketNumber) : _socketNumber(socketNumber), _port(port) {
		}

		void init() {
			static constexpr std::size_t StackSize = 1024;

			_transmitProcess = Eni::Threading::Thread("Transmitt", StackSize, Eni::Threading::ThreadPriority::Normal, [this] {
				while(true) {
					if (openSocket()) {
						_transmitter.process();
					}

					closeSocket();
					Eni::Threading::ThisThread::yield();
				}
			});

			_receiveProcess = Eni::Threading::Thread("Receive", StackSize, Eni::Threading::ThreadPriority::Normal, [this] {
				while(true) {
					_receiver.process();
					Eni::Threading::ThisThread::yield();
				}
			});
		}

		ITransmitter& getTransmitter() {
			return _transmitter;
		}

		Receiver& getReceiver() {
			return _receiver;
		}
	private:
		void closeSocket() {
			getReceiver().emitStopEvent();
//			close(_socketNumber);
		}

		bool openSocket() {
			using namespace Eni;

			bool result = (socket(_socketNumber, Sn_MR_TCP, _port, SF_TCP_NODELAY) == _socketNumber);				

			if (result) {
				while(getSn_SR(_socketNumber) != SOCK_INIT) {
					Threading::ThisThread::sleepForMs(10);
				}

				result &= listen(_socketNumber) == SOCK_OK;				
			}

			if (result) {
				while(getSn_SR(_socketNumber) == SOCK_LISTEN) {
					Threading::ThisThread::sleepForMs(10);
				}

				result &= (getSn_SR(_socketNumber) == SOCK_ESTABLISHED);
			}

			enableInterrupt();

			

			return result;
		}

		void enableInterrupt() {
			setSn_IMR(_socketNumber, Sn_IR_RECV);
			setSIMR(1 << _socketNumber);
		}
		
		Eni::Threading::Thread _transmitProcess {};
		Eni::Threading::Thread _receiveProcess {};

		uint8_t _socketNumber;
		uint8_t _port;

		Transmitter _transmitter {_socketNumber};
		Receiver _receiver {_socketNumber};


		std::array<uint8_t, 4> _hostIp {}; 
		uint16_t _hostPort {}; 

	};
}
