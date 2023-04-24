#pragma once

namespace UDA {
	class ITransmissionManager {
	public:
		virtual void startTransmission() = 0;
		virtual bool received(uint8_t* data, std::size_t size) = 0;
	};

	class ISender {
	public:
		virtual bool addValue(uint8_t* value, std::size_t size) = 0;
		virtual void flush() = 0;
		virtual void start() = 0;
	};

}
