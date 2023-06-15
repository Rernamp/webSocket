#pragma once

#include <Fifo.h>


namespace UDA::W5500 {
	class ITransmitter {
	public:
		ITransmitter() {

		}
		std::size_t append(uint8_t* data, std::size_t size) {
			if (_data.getFreeSize() < size) {
				return 0;
			}

			return _data.pushData(data, size);
		}
	private:
		Fifo<1024> _data {};
	}
	class Transmitter : public ITransmitter {
	public:
		Transmitter(uint8_t socketNumber) : _socketNumber(socketNumber) {

		}
		#error "Add in transmitter and reciver socket number"
		void process() {
			

			while(true) {
				while((_data.size() / maxBufferSize) != 0) {
					if (!sendData(_data.pop(txData.data(), maxBufferSize), maxBufferSize)) {
						break;
					}	
				}

				if ((_data.size() % maxBufferSize)) {
					std::size_t lastData = _data.size() % maxBufferSize;					
					if (!sendData(_data.pop(txData.data(), lastData), lastData)) {
						break;
					}	
				}

				Thread::ThisThread::yeild();
				#warning "May be change yeild to semaphore"
			}
		}
	private:
		bool sendData(uint8_t* data, std::size_t size) {
			auto sizeSends = send(_socketNumber, data, size);

			if (sizeSends < 0) {
				return false;
			}

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
