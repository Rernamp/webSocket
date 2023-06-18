#pragma once

#include <W5500Transmitter.h>

namespace UDA {

    class IConnection {
    public:
        virtual ~IConnection() = default;        
        virtual void process(ITransmitter* transmiter) = 0;        
        virtual void stop() {
            exitRequest = true;
        };
    protected:
        bool exitRequest = false;
    };

    class Connection {
    public:
    private:
    };
} 
