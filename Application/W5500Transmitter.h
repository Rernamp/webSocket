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
		Fifo<1024> _data {};
	};

	class Transmitter : public ITransmitter {
	public:
		Transmitter(uint8_t socketNumber) : _socketNumber(socketNumber) {

		}

		void process() {
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
						break;
					}	
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
		}
	private:
		bool sendData(uint8_t* data, std::size_t size) {
			auto value = getSn_MR(_socketNumber);
			auto sizeSends = send(_socketNumber, data, size);

			if (sizeSends < 0) {
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

		static constexpr std::size_t maxBufferSize = 128;
		std::array<uint8_t, maxBufferSize> txData {};
		uint8_t _socketNumber;
	};

}
