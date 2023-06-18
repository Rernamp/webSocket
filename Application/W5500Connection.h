#pragma once

#include <W5500Transmitter.h>

namespace UDA {

    class IConnection {
    public:
        virtual ~IConnection() = default;        
        virtual void process(ITransmitter* transmiter) = 0;
        virtual bool receive(uint8_t* data, std::size_t size) {return false;};
        virtual void stop() = 0;
    };

    class BaseConnection : public IConnection {
    public:
    	void stop() override {
			exitRequest = true;
		};
    	void process(ITransmitter* transmiter) override {
    		_transmitter = transmiter;
            process();
    	}
    	virtual void process() = 0;
    protected:
    	bool exitRequest = false;
    	ITransmitter* _transmitter = nullptr;
    };
} 
