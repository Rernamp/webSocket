#pragma once

#include <array>

namespace UDA::Driver {
	class DFSDMFilter {
		using MicDataType = int16_t;
		static constexpr std::size_t bufferSize = 256;
	public:

		class IDataListener {
		public:
			virtual ~IDataListener() = default;
			virtual void dataCallback(int16_t* data, std::size_t size) = 0;
		};
		DFSDMFilter(DFSDM_Filter_HandleTypeDef filterHandler) : _filterHandler(filterHandler) {
//			for (std::size_t i = 0; i < _tempData.size(); i++) {
//				_tempData[i] = i;
//			}
			_tempData.fill(128);
		}

		void start() {
			if(HAL_DFSDM_FilterRegularMsbStart_DMA(&_filterHandler, _bufferOfData.data(), _bufferOfData.size()) == HAL_ERROR) {
				Error_Handler();
			}
		}

		void setLisnter(IDataListener* listener) {
			_listener = listener;
		}

		void interruptCallback(bool isHalf) {
			int16_t* data = _tempData.data();
			std::size_t size = _tempData.size() / 2;
			if (!isHalf) {
				data += size;
			}

			if (_listener) {
				_listener->dataCallback(data, size);
			}
		}

	private:
		std::array<MicDataType, bufferSize> _bufferOfData {};
		std::array<MicDataType, bufferSize> _tempData {};
		DFSDM_Filter_HandleTypeDef _filterHandler;
		IDataListener* _listener = nullptr;
	};

}
