#pragma once

#include <array>
#include <cstdint>
#include "stm32h7xx_hal.h"
#include <Eni/Debug/Assert.h>


namespace UDA::Driver {
	class DFSDMFilter {
		using MicDataType = int16_t;
		static constexpr std::size_t bufferSize = 256;
	public:

		class IDataListener {
		public:
			virtual ~IDataListener() = default;
			virtual void dataCallback(int16_t* data, std::size_t size, uint8_t numberFilter) = 0;
		};
		DFSDMFilter(DFSDM_Filter_HandleTypeDef* filterHandler, uint8_t number) : _filterHandler(filterHandler), numberFilter(number) {
		}

		void start() {
			if(HAL_DFSDM_FilterRegularMsbStart_DMA(_filterHandler, _bufferOfData.data(), _bufferOfData.size()) == HAL_ERROR) {
				eniAssert(false);
			}
		}

		std::size_t sizeOfHalfTransfere() {
			return bufferSize / 2;
		}

		void stop() {
			if(HAL_DFSDM_FilterRegularStop_DMA(_filterHandler) == HAL_ERROR) {
				eniAssert(false);
			}
		}

		void setLisnter(IDataListener* listener) {
			_listener = listener;
		}

		void interruptCallback(bool isHalf) {
			int16_t* data = _bufferOfData.data();
			std::size_t size = _bufferOfData.size() / 2;
			if (!isHalf) {
				data += size;
			}

			if (_listener) {
				_listener->dataCallback(data, size, numberFilter);
			}
		}

		inline bool equal(DFSDM_Filter_HandleTypeDef* filter) {
			return filter == _filterHandler;
		}

		const uint8_t numberFilter;

	private:
		std::array<MicDataType, bufferSize> _bufferOfData {};
		
		DFSDM_Filter_HandleTypeDef* _filterHandler;
		IDataListener* _listener = nullptr;
	};

}
