#pragma once

#include <cstdint>

namespace UDA {
    struct DataContainer {
        int16_t * data;
        std::size_t size;            
    };

    struct MapDataContainerToFilter {
        DataContainer container;
        uint8_t numberFilter;
    };
}