#pragma once

#include <FilterContainer.h>
#include <W5500Receiver.h>
#include <W5500Transmitter.h>
#include <Dfsdm.h>

namespace UDA {
    class MicrophoneArray : public IConnection, public Driver::DFSDMFilter::IDataListener {
    public:
        MicrophoneArray() {

        }

        void dataCallback(int16_t* data, std::size_t size) {
        	if (!exitRequest) {
        		_transmitter->append(reinterpret_cast<uint8_t*>(data), size * 2);
        	}
        }

        void process(ITransmitter* transmitter) override {
            _transmitter = transmitter;
            _filter = &getFilter1();
            exitRequest = false;
            _filter->setLisnter(this);
            _filter->start();
            while(!exitRequest) {
                using namespace Eni;
                
                Threading::ThisThread::sleepForMs(10);
            }

            _filter->stop();
        }

    private:
        UDA::Driver::DFSDMFilter* _filter = nullptr;
        ITransmitter* _transmitter = nullptr;
    };
}
