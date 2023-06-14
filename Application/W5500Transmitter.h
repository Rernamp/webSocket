#pragma once

#include <Fifo.h>

#include <W5500Connection.h>

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

		void setConnection(IConnection* connection) {
			_connection = connection;
		}

	private:
		IConnection* _connection = nullptr;
		Fifo<1024> _data {};
	}
	class Transmitter : public ITransmitter {
	public:
		#error "Add in transmitter and reciver socket number"
		void process() {
			

			while(true) {
				// if (_data.size() > 0) {
					while((_data.size() / maxBufferSize) != 0) {
						if (!sendData(_data.pop(txData.data(), maxBufferSize), maxBufferSize)) {
							emitStopEvent();
							break;
						}	
					}

					if ((_data.size() % maxBufferSize)) {
						std::size_t lastData = _data.size() % maxBufferSize;					
						if (!sendData(_data.pop(txData.data(), lastData), lastData)) {
							emitStopEvent();
							break;
						}	
					}
				// }
			}
		}
	private:
		bool sendData(uint8_t* data, std::size_t size) {
			#warning "Implement send data to W5500"
			#warning "add return false for generate stop enent"
			return 0;
		}

		void emitStopEvent() {
			if (_connection) {
				_connection->stopEvent();
			}
		}

		static constexpr std::size_t maxBufferSize = 128;
		std::array<uint8_t, maxBufferSize> txData {};
	};

}
