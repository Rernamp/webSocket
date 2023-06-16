#pragma once

#include <Eni/Threading/Semaphore.h>

#include <array>

#include <W5500Connection.h>

namespace UDA::W5500 {

	class Receiver {
	public:
		class IReceiverHandler : public IConnection {
		public:
			virtual ~IReceiverHandler() = default;
			virtual bool receive(uint8_t* data, std::size_t size) = 0;
		};
	public:

		Receiver(uint8_t socketNumber) : _socketNumber(socketNumber) {

		}
		
		void setHandler(IReceiverHandler* handler) {
			_receiverHandler = handler;
		}

		void handleInterrupt() {
			_dataReceived.give();
		}

		void process() {
			while (true) {
				while(!_dataReceived.take()) {}

				handleReceive();
				handleStateOfSocket();

			}
			
		}		

	private:		
		void handleReceive() {
			bool stopEvent = false;

			if (stopEvent) {
				emitStopEvent();	
			}
		}

		void handleSocketStatus() {
			bool stopEvent = false;
			
			if (stopEvent) {
				emitStopEvent();	
			}
		}

		void emitStopEvent() {
			if (_receiverHandler) {
				_receiverHandler->stopEvent();
			}
		}


		IReceiverHandler* _receiverHandler = nullptr;
		Eni::Threading::BinarySemaphore _dataReceived {};

		static constexpr std::size_t maxBufferSize = 128;
		std::array<uint8_t, 128> _data {};
		uint8_t _socketNumber;
	};

}
