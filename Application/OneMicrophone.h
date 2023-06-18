#pragma once

#include <FilterContainer.h>
#include <W5500Receiver.h>
#include <W5500Transmitter.h>
#include <Dfsdm.h>

namespace UDA {
    class OneMicrophone : public IConnection, public Driver::DFSDMFilter::IDataListener {
    public:
        OneMicrophone() {

        }

        void dataCallback(int16_t* data, std::size_t size, uint8_t numberFilter) {
        	if (!exitRequest) {
        		_transmitter->append(reinterpret_cast<uint8_t*>(data), size * 2);
        	}
        }

        bool receive(uint8_t* data, std::size_t size) override {
            static constexpr std::size_t dataSize = 1;
            bool result = size == dataSize;

            static constexpr uint8_t maxNumberElements = 4;
            if (data[0] < maxNumberElements) {  
                switch(data[0]) {
                case 0: 
                    _filter = &getFilter1();
                    break;
                case 1: 
                    _filter = &getFilter2();
                    break;
                case 2: 
                    _filter = &getFilter3();
                    break;
                case 3: 
                    _filter = &getFilter4();
                    break;
                default:
                    _filter = &getFilter1();
                    break;
                }
                
            }

            return result;
        }

        void process(ITransmitter* transmitter) override {
            _transmitter = transmitter;
            
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
        ITransmitter* _transmitter = nullptr;
    };
}
