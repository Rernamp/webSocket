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
	}

	void flush() {
		_transferSize = _buffer.getSize();
		_continueState.give();
	}

	bool addValue(uint8_t value, std::size_t size) {
		bool result = _buffer.add(&value, size);
		if (result && (_buffer.getSize() > _minTransferSize)) {
			_transferSize = _minTransferSize;
			_continueState.give();
		}

		return result;
	}
private:
	void transferProcess() {
		using namespace Eni;

		while(!_continueState.take()){}


		auto result = socket(_socketNumber, Sn_MR_UDP, _port, 0);
		eniAssert(result== _socketNumber);

		while(getSn_SR(_socketNumber) != SOCK_UDP)
		{
			Threading::ThisThread::sleepForMs(2);
		}

		static constexpr std::size_t sizeInitialTransfer = 1;
		result = recvfrom(_socketNumber, _buffer.data(), sizeInitialTransfer, _hostIp.data(), &_hostPort);

		static constexpr uint8_t validateValue = 0xFE;
		if (_buffer.get() != validateValue) {
			disconnect(_socketNumber);
			close(_socketNumber);
			return;
		}

		while(true) {
			_continueState.take();
			result = sendto(_socketNumber, _buffer.data(), _transferSize, _hostIp.data(), _hostPort);
		}
	}

	Eni::Threading::BinarySemaphore _continueState {};
	Eni::Threading::Thread _transferProcess {};
	const std::array<uint8_t, 4> _ip;
	const uint16_t _port;
	std::array<uint8_t, 4> _hostIp {};
	uint16_t _hostPort {0};
	const uint8_t _socketNumber;

	std::size_t _transferSize = 0;
	static constexpr std::size_t _minTransferSize = 128;
	Container<uint8_t, 1024> _buffer {};
};
