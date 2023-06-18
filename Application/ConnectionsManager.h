#pragma once

#include <W5500Receiver.h>
#include <W5500Transmitter.h>

#include <Eni/Threading/Thread.h>
#include <Eni/Threading/Semaphore.h>
#include <Eni/Debug/Assert.h>


#include <TestConnection.h>
#include <OneMicrophone.h>

namespace UDA {
    class ConnectionsManager : public Receiver::IReceiverHandler {
    public:
        ConnectionsManager(ITransmitter* transmitter) : _transmitter(transmitter) {

            eniAssert(transmitter != nullptr);

            static constexpr std::size_t stackSize = 1024;
            _connectionProcess = Eni::Threading::Thread("Connection process", stackSize, Eni::Threading::ThreadPriority::Normal, [this]{
                process();
            });
        }
        
        bool receive(uint8_t* data, std::size_t size) override {
            bool result = (size >= 2) && (data[0] == magicConstant);
            if (result) {
                switch (data[1]) {
                case 0:
                    _actualConnection = &_test;
                    _startConnectionProcess.give();
                    break;
                case 1:
                    _actualConnection = &_oneMicrophone;
                    _startConnectionProcess.give();
                    break;
                default:
                    break;
                }
                size -= 2;
            }
            if (size > 0) {
                _actualConnection->receive(&data[2], size);
            }
            return result;    
        }

        void stopEvent() override {
        	if (_actualConnection) {
        		_actualConnection->stop();
        	}
        }

    private:
        void process() {

            while(true) {
                while(!_startConnectionProcess.take()) {}

                if (_actualConnection) {
                    _actualConnection->process(_transmitter);
                }

            }
        }
        Eni::Threading::Thread _connectionProcess {};
        Eni::Threading::BinarySemaphore _startConnectionProcess {};
        
        ITransmitter* _transmitter;
        IConnection* _actualConnection = nullptr;

        TestConnection _test {};
        OneMicrophone _oneMicrophone {};
    };
}
