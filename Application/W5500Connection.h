#pragma once

namespace UDA::W5500 {

    class IConnection {
    public:
        virtual ~IConnection() = default;
        virtual void stopEvent() = 0;
    };

    class Connection {
    public:
    private:
    };
} 