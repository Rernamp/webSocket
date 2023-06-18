#pragma once

#include <Dfsdm.h>

#include <array>
#include <bitset>

namespace UDA {
    class MicrophoneArray : public IConnection, public Driver::DFSDMFilter::IDataListener {
    public:
        void dataCallback(int16_t* data, std::size_t size, uint8_t numberFilter) {
        	if (!exitRequest) {
        	}
        }

        void process(ITransmitter* transmitter) override {
            _transmitter = transmitter;

            _filter = &getFilter1();

            exitRequest = false;
            start();

            while(!exitRequest) {
                using namespace Eni;
                
                Threading::ThisThread::sleepForMs(10);
            }

            stop();
        }

        bool receive(uint8_t* data, std::size_t size) override {
            
        }


    private:
        void start() {
            for (UDA::Driver::DFSDMFilter* filter : _filters) {
                if (filter) {
                    filter->setLisnter(this);
                    filter->start();
                }
            }
        }
        void stop() {
            for (UDA::Driver::DFSDMFilter* filter : _filters) {
                if (filter) {
                    filter->stop();
                    filter->setLisnter(nullptr);
                }
            }
        }
        static constexpr std::size_t maxNumberElements = 4;
        std::array<UDA::Driver::DFSDMFilter*, maxNumberElements> _filters {nullptr};
        std::bitset<maxNumberElements> _elementsConfiguration {0};
        std::bitset<maxNumberElements> _elementsComplete {0};
        
    };
}