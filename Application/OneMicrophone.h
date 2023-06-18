#pragma once

#include <FilterContainer.h>
#include <W5500Receiver.h>
#include <W5500Transmitter.h>
#include <Dfsdm.h>

namespace UDA {
    class OneMicrophone : public BaseConnection, public Driver::DFSDMFilter::IDataListener {
    public:
        OneMicrophone() {

        }

        void dataCallback(int16_t* data, std::size_t size, uint8_t numberFilter) {
        	if (!exitRequest) {
        		_transmitter->append(reinterpret_cast<uint8_t*>(data), size * sizeof(int16_t));
        	}
        }

        bool receive(uint8_t* data, std::size_t size) override {
            static constexpr std::size_t dataSize = 1;
            bool result = size == dataSize;

            static constexpr uint8_t maxNumberElements = 4;
            if (data[0] < maxNumberElements) {  
                _filter = &getFilterByIndex(static_cast<std::size_t>(data[0]));
            }

            return result;
        }

        void process() override {
            exitRequest = false;
            _filter->setLisnter(this);
            _filter->start();
            while(!exitRequest) {
                using namespace Eni;
                
                Threading::ThisThread::sleepForMs(10);
            }

            _filter->stop();
            _filter->setLisnter(nullptr);
        }

    private:
        UDA::Driver::DFSDMFilter* _filter = nullptr;
    };
}
