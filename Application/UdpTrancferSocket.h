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
		_transferSize = _buffer.getUsedSize();
		_continueState.give();
	}

	bool addValue(uint8_t* value, std::size_t size) {
		bool result = _buffer.add(value, size);
		if (result && (_buffer.getUsedSize() >= _minTransferSize)) {
			_transferSize = _buffer.getUsedSize();
			_continueState.give();
		}

		return result;
	}
private:
	void transferProcess() {
		using namespace Eni;

		_buffer.reset();

		while(!_continueState.take()){}

		auto result = socket(_socketNumber, Sn_MR_UDP, _port, 0);
		eniAssert(result== _socketNumber);

		while(getSn_SR(_socketNumber) != SOCK_UDP)
		{
			Threading::ThisThread::sleepForMs(2);
		}

		static constexpr std::size_t sizeInitialTransfer = 2;
		_transferSize = sizeInitialTransfer;
		result = recvfrom(_socketNumber, _buffer.pointerForAdd(_transferSize), _transferSize, _hostIp.data(), &_hostPort);

		static constexpr uint8_t validateValue = 0xFE;
		std::array<uint8_t, 2> resultRecive = {};
		_buffer.get(resultRecive.data(), sizeInitialTransfer);
		if (resultRecive[0] != validateValue) {
			closeSession();
			_continueState.give();
			return;
		}

		std::size_t numberTransfer = resultRecive[1];
		std::size_t iteration = 0;

		_continueState.give();
		Eni::Threading::ThisThread::yield();

		while(true) {
			_continueState.take();
			result = sendto(_socketNumber, _buffer.pointerForGet(_transferSize), _transferSize, _hostIp.data(), _hostPort);
			if (iteration == numberTransfer) break;
			++iteration;
		}

		closeSession();
	}

	void closeSession() {
		disconnect(_socketNumber);
		close(_socketNumber);
	}

	bool _startConection = false;
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
