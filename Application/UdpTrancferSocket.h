#pragma once

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
			transferProcess();
		});
	}


	void start() {
		_continueState.give();
	}
private:
	void transferProcess() {
		while(!_continueState.take()){}


		stat = socket(_socketNumber, Sn_MR_UDP, _port, 0);
		eniAssert(stat == _socketNumber);

		while(getSn_SR(_socketNumber) != SOCK_UDP)
		{
			Threading::ThisThread::sleepForMs(2);
		}



	}
	Eni::Threading::BinarySemaphore _continueState {};
	Eni::Threading::Thread _transferProcess {};
	const std::array<uint8_t, 4> _ip;
	const uint16_t _port;
	const std::array<uint8_t, 4> _hostIp {};
	const uint16_t _hostPort {0};
	const uint8_t _socketNumber;
};
