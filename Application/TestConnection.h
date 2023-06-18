#pragma once

#include <W5500Receiver.h>
#include <W5500Transmitter.h>

#include <Eni/Threading/Thread.h>

#include <array>

namespace UDA {
    class TestConnection : public IConnection {
    public:
        TestConnection() {            
        	for (std::size_t i = 0; i < _data.size(); i++) {
        		_data[i] = i;
        	}
        }

        void process(ITransmitter* transmitter) override {
            _transmitter = transmitter;
            exitRequest = false;
            while(!exitRequest) {
                using namespace Eni;
                _transmitter->append(_data.data(), _data.size());
                Threading::ThisThread::sleepForMs(10);
                
            }
        }

        void stop() override {
            exitRequest = true;
        }
    private:
        bool exitRequest = false;
        static constexpr std::size_t sizeTestData = 256;
        std::array<uint8_t, sizeTestData> _data = {};
        ITransmitter* _transmitter = nullptr;
    };
}
