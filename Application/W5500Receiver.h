#pragma once

#include <Eni/Threading/Semaphore.h>

#include <array>

#include <W5500Connection.h>

namespace UDA {

	class Receiver {
	public:
		class IReceiverHandler {
		public:
			virtual ~IReceiverHandler() = default;
			virtual bool receive(uint8_t* data, std::size_t size) = 0;
			virtual void stopEvent() = 0;			
			static constexpr uint8_t magicConstant = 0xE6;
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
			}
		}		
		
		void emitStopEvent() {
			if (_receiverHandler) {
				_receiverHandler->stopEvent();
			}
		}

	private:		
		void handleReceive() {
			bool result = (1 << _socketNumber) & (getSIR());

			if (result) {
				auto interruptRegister = getSn_IR(_socketNumber);
				if (interruptRegister & Sn_IR_RECV) {
					receive();
					setSn_IR(_socketNumber, Sn_IR_RECV);
				}

			}
		}

		void receive() {
			int32_t succesLenght = 0;
			while(succesLenght = recv(_socketNumber, _data.data(), _data.size()), ((succesLenght > 0) && _receiverHandler)) {
				_receiverHandler->receive(_data.data(), succesLenght);
			}
		}

		IReceiverHandler* _receiverHandler = nullptr;
		Eni::Threading::BinarySemaphore _dataReceived {};

		static constexpr std::size_t maxBufferSize = 128;
		std::array<uint8_t, 128> _data {};
		uint8_t _socketNumber;
	};

}
