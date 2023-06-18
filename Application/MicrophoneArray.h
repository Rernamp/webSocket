#pragma once

#include <Dfsdm.h>
#include <FilterContainer.h>
#include <StaticVector.h>

#include <Eni/Threading/Semaphore.h>

#include <array>
#include <bitset>

namespace UDA {
    class MicrophoneArray : public BaseConnection, public Driver::DFSDMFilter::IDataListener {
    public:
        struct DataContainer {
            int16_t * data;
            std::size_t size;            
        };

        struct MapDataContainerToFilter {
            DataContainer container;
            uint8_t numberFilter;
        };

        void dataCallback(int16_t* data, std::size_t size, uint8_t numberFilter) {
        	if (!exitRequest) {
                {
                    for (auto& elementdataContainer : _elementsDataContainer) {
                        if (elementdataContainer.numberFilter == numberFilter) {
                            elementdataContainer.container = DataContainer{data, size};
                            break;
                        }
                    }
                }
                                
                _elementsComplete.set(numberFilter);

                if (_elementsComplete == _elementsConfiguration) {
                	_startProcess.give();
                	_elementsComplete.reset();
                }
        	}
        }

        void stop() override {
            _startProcess.give();
            BaseConnection::stop();
        }

        void process() override {
            fillFilters();

            exitRequest = false;
            startFilters();

            int32_t tempData = 0;
            int32_t numberElements = _filters.size();
            int16_t data = 0;
            while((!exitRequest)) {
                if (!_startProcess.take()) {
                    continue;
                }
                
                for (std::size_t i = 0; i < 128; i++) { //TODO fix hardcode
                
                    for (auto& element : _elementsDataContainer) {                        
                        tempData += *element.container.data;
                        ++element.container.data;
                    }
                        
                    tempData /= numberElements;
                    data = static_cast<int16_t>(tempData);
                    _transmitter->append(reinterpret_cast<uint8_t*>(&data), sizeof(int16_t));
                    tempData = 0;                    
                }
            }
            stopFilters();
        }

        bool receive(uint8_t* data, std::size_t size) override {
            static constexpr std::size_t dataSize = 2;
            bool result = size == dataSize;

            _elementsConfiguration = data[0];

            #warning "Added this choose of algoritm"
            // data[1];
            return result;
        }


    private:
        void startFilters() {
            for (UDA::Driver::DFSDMFilter* filter : _filters) {
            	filter->setLisnter(this);
            	filter->start();
            }
        }
        void stopFilters() {
            for (UDA::Driver::DFSDMFilter* filter : _filters) {
                filter->stop();
                filter->setLisnter(nullptr);
            }

            _elementsDataContainer.clear();
            _filters.clear();
        }

        void fillFilters() {
            for (int8_t i = _elementsConfiguration.size() - 1; i >= 0; i--) {
                if (_elementsConfiguration.test(i)) {
                    _filters.push_back(&getFilterByIndex(i));
					_elementsDataContainer.push_back(MapDataContainerToFilter{DataContainer{}, i});
                }
            }
        }
        static constexpr std::size_t maxNumberElements = 4;
        StaticVector<UDA::Driver::DFSDMFilter*, maxNumberElements> _filters {};
        StaticVector<MapDataContainerToFilter, maxNumberElements> _elementsDataContainer {};
        std::bitset<maxNumberElements> _elementsConfiguration {0};
        std::bitset<maxNumberElements> _elementsComplete {0};

        Eni::Threading::BinarySemaphore _startProcess {};
    };
}
