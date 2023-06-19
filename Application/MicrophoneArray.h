#pragma once

#include <Dfsdm.h>
#include <FilterContainer.h>
#include <StaticVector.h>
#include <DataContainer.h>

#include <W5500Connection.h>
#include <Math/SoundProcessing.h>

#include <Eni/Threading/Semaphore.h>

#include <array>
#include <bitset>

namespace UDA {
    class MicrophoneArray : public BaseConnection, public Driver::DFSDMFilter::IDataListener {
    public:        


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
        	if (_signalProcessing) {
				_signalProcessing->stopRequest = true;
			}
            _startProcess.give();
            BaseConnection::stop();
        }

        void process() override {
            fillFilters();

            exitRequest = false;
            if (_signalProcessing) {
                _signalProcessing->stopRequest = false;
                _signalProcessing->start();
            }
            startFilters();

            
            while((!exitRequest)) {
                if (!_startProcess.take()) {
                    continue;
                }

                if (_signalProcessing) {
                    _signalProcessing->process(_transmitter, _elementsDataContainer, _processSize);
                }
            }
            stopFilters();
        }

        bool receive(uint8_t* data, std::size_t size) override {
            static constexpr std::size_t dataSize = 2;
            bool result = size == dataSize;

            _elementsConfiguration = data[0];

            switch (data[1]) {
            case 0 :
                _signalProcessing = &_average;
                break;
            case 1 :
                _signalProcessing = &_streamAllSignal;
                break;
            default :
                _signalProcessing = nullptr;
                break;
            }
            
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
            _processSize = 0;
            for (int8_t i = _elementsConfiguration.size() - 1; i >= 0; i--) {
                if (_elementsConfiguration.test(i)) {
                    _filters.push_back(&getFilterByIndex(i));
					_elementsDataContainer.push_back(MapDataContainerToFilter{DataContainer{}, i});
                    _processSize += _filters.back()->sizeOfHalfTransfere();
                }
            }
            _processSize /= _filters.size();
        }
        static constexpr std::size_t maxNumberElements = 4;
        
        StaticVector<UDA::Driver::DFSDMFilter*, maxNumberElements> _filters {};
        StaticVector<MapDataContainerToFilter, maxNumberElements> _elementsDataContainer {};
        std::bitset<maxNumberElements> _elementsConfiguration {0};
        std::bitset<maxNumberElements> _elementsComplete {0};
        std::size_t _processSize = 0;

        IProcessing* _signalProcessing = nullptr;

        AverageProcess _average {};
        StreamAllSignal _streamAllSignal {};

        Eni::Threading::BinarySemaphore _startProcess {};
    };
}
