#pragma once

#include <Fifo.h>
#include <Eni/Threading/Thread.h>
#include <W5500Receiver.h>

namespace UDA {
	class ITransmitter {
	public:
		ITransmitter() {
			_data.clear();
		}
		std::size_t append(uint8_t* data, std::size_t size) {
			if (_data.getFreeSize() < size) {
				return 0;
			}

			return _data.push(data, size);
		}
	protected:
		Fifo<2048> _data {};
	};

	class Transmitter : public ITransmitter {
	public:
		Transmitter(uint8_t socketNumber) : _socketNumber(socketNumber) {

		}

		void process() {
			bool exitRequest = false;
			_data.clear();

			{
				static constexpr uint8_t magicConstant = 0xE6;
				uint8_t magicValue = magicConstant;
				if (!sendData(&magicValue, 1)) {
					return;
				}
			}

			while(true) {
				while((_data.size() / maxBufferSize) != 0) {
					_data.pop(txData.data(), maxBufferSize);
					if (!sendData(txData.data(), maxBufferSize)) {
						exitRequest = true;
						break;
					}	
				}

				if (exitRequest) {
					break;
				}

				if ((_data.size() % maxBufferSize)) {
					std::size_t lastData = _data.size() % maxBufferSize;
					_data.pop(txData.data(), lastData);
					if (!sendData(txData.data(), lastData)) {
						break;
					}	
				}

				Eni::Threading::ThisThread::yield();
				#warning "May be change yeild to semaphore"
			}

			_data.clear();
		}
	private:
		bool sendData(uint8_t* data, std::size_t size) {
			
			auto sizeSends = send(_socketNumber, data, size);

			if (sizeSends < 0) {
				auto value1 = getSn_MR(_socketNumber);
				return false;
			}
			size -= sizeSends;

			while(size > 0) {
				size -= sizeSends;
				data += sizeSends;
				sizeSends = send(_socketNumber, data, size);
				if (sizeSends < 0) {
					return false;
				}
			}

			return true;
		}

		static constexpr std::size_t maxBufferSize = 256;
		std::array<uint8_t, maxBufferSize> txData {};
		uint8_t _socketNumber;
	};

}
