#pragma once

#include <Container.h>
#include "ioLibrary_Driver/Ethernet/socket.h"

#include <Eni/Threading/Semaphore.h>
#include <Eni/Threading/Thread.h>
#include <Eni/Debug/Assert.h>

#include <array>

class UsbTrancferSocket {
	static constexpr std::size_t StackSize = 1024;
public:
	UsbTrancferSocket(std::array<uint8_t, 4> ip, uint16_t port, uint8_t socketNumber) : _ip(ip), _port(port), _socketNumber(socketNumber) {
		_transferProcess = Eni::Threading::Thread("UdpTransfer", StackSize, Eni::Threading::ThreadPriority::Normal, [this] {
			while(true) {
				transferProcess();
			}
		});
	}


	void start() {
		_continueState.give();
		Eni::Threading::ThisThread::yield();
		_continueState.take();
	}

	void flush() {
		_transferSize = _tranciveBuffer.getUsedSize();
		_continueState.give();
	}

	bool addValue(uint8_t* value, std::size_t size) {
		bool result = _tranciveBuffer.add(value, size);
		if (result && (_tranciveBuffer.getUsedSize() >= _minTransferSize)) {
			_transferSize = _tranciveBuffer.getUsedSize();
			_continueState.give();
		}

		return result;
	}

	void interruptCallback() {
		_receviState.give();
	}
private:
	void transferProcess() {
		using namespace Eni;

		_tranciveBuffer.reset();
		_receiveBuffer.reset();

		while(!_continueState.take()){}

		auto result = socket(_socketNumber, Sn_MR_UDP, _port, 0);
		eniAssert(result== _socketNumber);

		enableInterrupt();

		while(getSn_SR(_socketNumber) != SOCK_UDP)
		{
			Threading::ThisThread::sleepForMs(2);
		}

		static constexpr std::size_t sizeInitialTransfer = 2;
		_transferSize = sizeInitialTransfer;
		receive(_receiveBuffer.pointerForAdd(_transferSize), _transferSize);
		_receiveBuffer.applyAdd(_transferSize);

		static constexpr uint8_t validateValue = 0xFE;
		std::array<uint8_t, 2> resultRecive = {};
		_receiveBuffer.get(resultRecive.data(), sizeInitialTransfer);

		if (resultRecive[0] != validateValue) {
			closeSession();
			_continueState.give();
			return;
		}

		_continueState.give();
		Eni::Threading::ThisThread::yield();

		while(true) {
			_continueState.take();

			if (receive(_receiveBuffer.pointerForAdd(_transferSize), _transferSize, 0)) {
				_receiveBuffer.applyAdd(_transferSize);
				break;
			}
			result = sendto(_socketNumber, _tranciveBuffer.pointerForGet(_transferSize), _transferSize, _hostIp.data(), _hostPort);
			_tranciveBuffer.applyGet(_transferSize);
		}

		closeSession();
	}


	void closeSession() {
		disconnect(_socketNumber);
		close(_socketNumber);
	}

	void enableInterrupt() {
		setSn_IR(_socketNumber, Sn_IR_RECV);
		setSIMR(1 << _socketNumber);
	}

	bool receive(uint8_t* data, std::size_t size, std::size_t timeOut = portMAX_DELAY) {
		bool result = _receviState.take(timeOut);
		if (result) {
			result &= handleInterrupt();
			if (result) {
				result &= (recvfrom(_socketNumber, data, size, _hostIp.data(), &_hostPort) > 0);
			}

			clearInterruptFlags();
		}

		return result;
	}

	bool handleInterrupt() {
		bool result = (1 << _socketNumber) & (getSIR());

		result &= static_cast<bool>(getSn_IR(_socketNumber) & Sn_IR_RECV);

		return result;
	}

	void clearInterruptFlags() {
		setSn_IR(_socketNumber, Sn_IR_RECV);
	}

	bool _startConection = false;
	Eni::Threading::BinarySemaphore _continueState {};
	Eni::Threading::BinarySemaphore _receviState {};
	Eni::Threading::Thread _transferProcess {};
	const std::array<uint8_t, 4> _ip;
	const uint16_t _port;
	std::array<uint8_t, 4> _hostIp {};
	uint16_t _hostPort {0};
	const uint8_t _socketNumber;

	std::size_t _transferSize = 0;
	static constexpr std::size_t _minTransferSize = 128;
	Container<uint8_t, 1024> _tranciveBuffer {};
	Container<uint8_t, 1024> _receiveBuffer {};
};
