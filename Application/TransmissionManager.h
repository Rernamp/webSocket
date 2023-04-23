#pragma once

namespace UDA {
	class ITransmissionManager {
	public:
		virtual void startTransmission() = 0;
		virtual void receive(uint8_t* data, std::size_t size) = 0;
	};

	class TransmissionManager final : public  ITransmissionManager {

	};
}
