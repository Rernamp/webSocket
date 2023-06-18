#pragma once

#include <Dfsdm.h>
#include <FilterContainer.h>

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
        void dataCallback(int16_t* data, std::size_t size, uint8_t numberFilter) {
        	if (!exitRequest) {
                _elementsDataContainer[numberFilter] = DataContainer{data, size};
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
            std::size_t numberElements = 0;
            int16_t data = 0;
            for (std::size_t element = 0; element < _filters.size(); element++) {
                if (_filters[element]) {
                    ++numberElements;
                }
            }

            while((!exitRequest)) {
                if (!_startProcess.take()) {
                    continue;
                }
                
                for (std::size_t i = 0; i < 128; i++) { //TODO fix hardcode
                
                    for (std::size_t element = 0; element < _filters.size(); element++) {
                        if (_filters[element]) {
                            tempData += *_elementsDataContainer[element].data;
                            ++_elementsDataContainer[element].data;
                        }
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
            for (std::size_t i = _filters.size(); i > 0; --i) {
                if (_filters[i - 1]) {
                	_filters[i - 1]->setLisnter(this);
                	_filters[i - 1]->start();
                }
            }
        }
        void stopFilters() {
            for (UDA::Driver::DFSDMFilter* filter : _filters) {
                if (filter) {
                    filter->stop();
                    filter->setLisnter(nullptr);
                }
            }

            _filters.fill(nullptr);
        }

        void fillFilters() {
            for (std::size_t i = 0; i < _elementsConfiguration.size(); i++) {
                if (_elementsConfiguration.test(i)) {
                    _filters[i] = &getFilterByIndex(i);
                }
            }
        }
        static constexpr std::size_t maxNumberElements = 4;
        std::array<UDA::Driver::DFSDMFilter*, maxNumberElements> _filters {nullptr};        
        std::array<DataContainer, maxNumberElements> _elementsDataContainer {};
        std::bitset<maxNumberElements> _elementsConfiguration {0};
        std::bitset<maxNumberElements> _elementsComplete {0};

        Eni::Threading::BinarySemaphore _startProcess {};
    };
}
