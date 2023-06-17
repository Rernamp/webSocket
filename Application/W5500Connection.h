#pragma once

#include <W5500Transmitter.h>

namespace UDA {

    class IConnection {
    public:
        virtual ~IConnection() = default;        
        virtual void process(ITransmitter* transmiter) = 0;        
        virtual void stop() = 0;        
    };

    class Connection {
    public:
    private:
    };
} 
