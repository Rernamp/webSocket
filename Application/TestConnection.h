#pragma once

#include <W5500Receiver.h>
#include <W5500Transmitter.h>

#include <Eni/Threading/Thread.h>

namespace UDA {
    class TestConnection : public IConnection {
    public:
        TestConnection() {            
        }

        void process(ITransmitter* transmitter) override {
            _transmitter = transmitter;

            while(!exitRequest) {
                using namespace Eni;
                ++data;
                _transmitter->append(&data, 1);
                Threading::ThisThread::sleepForMs(10);
                
            }
        }

        void stop() override {
            exitRequest = true;
        }
    private:
        bool exitRequest = false;
        uint8_t data = 0;
        ITransmitter* _transmitter = nullptr;
    };
}