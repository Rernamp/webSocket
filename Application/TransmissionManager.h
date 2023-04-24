#pragma once

#include <Interface.h>

#include <Eni/Threading/Thread.h>
#include <Eni/Threading/Semaphore.h>

namespace UDA {


	class TransmissionManager final : public  ITransmissionManager {
		static constexpr std::size_t StackSize = 1024;
	public:
		TransmissionManager() {
			_transmissionThread = Eni::Threading::Thread("TransmissionProcess", StackSize, Eni::Threading::ThreadPriority::Normal, [this]() {
				while(true) {
					transmissionProcess();
				}
			});
		}

		void setSender(ISender& sender) {
			_sender = &sender;
		}

		void startTransmission() override {
			_processState.give();
		}
		bool received(uint8_t* data, std::size_t size) override {
#warning "Implement this"
			_processState.give();
			return size > 0;
		}
	private:
		void transmissionProcess() {
			_processState.take();

			while(true) {
				if (_processState.take(0)) {
					break;
				}
				_sender->addValue(_data.data(), _data.size());\
				Eni::Threading::ThisThread::sleepForMs(10);
			}
		}
	private:
		std::array<uint8_t, 128> _data {};
		ISender* _sender;
		Eni::Threading::Thread _transmissionThread;
		Eni::Threading::BinarySemaphore _processState {};
	};
}
