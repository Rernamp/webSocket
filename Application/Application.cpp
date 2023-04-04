#include <Application.h>
#include "ioLibrary_Driver/Ethernet/socket.h"

#include <stdio.h>
#include <string.h>


Application& Application::getInstante() {
	static Application app;
	return app;
}

extern SPI_HandleTypeDef hspi1;
Application::Application() : _w5500Spi(hspi1, _cs) {
	Eni::Gpio::initOutput(_led);
}


#define HTTP_SOCKET     0
#define PORT_TCPS		    5000
#define DATA_BUF_SIZE   2048
uint8_t gDATABUF[DATA_BUF_SIZE];



uint8_t stat;
uint8_t reqnr;

void W5500_Select(void)
{
    Application::getInstante().getSpi().select();
}

void W5500_Unselect(void)
{
	Application::getInstante().getSpi().unselect();
}

void W5500_ReadBuff(uint8_t* buff, uint16_t len)
{
	Application::getInstante().getSpi().readBuff(buff, len);
}

void W5500_WriteBuff(uint8_t* buff, uint16_t len)
{
	Application::getInstante().getSpi().writeBuff(buff, len);
}

uint8_t W5500_ReadByte(void)
{
    uint8_t byte;
    Application::getInstante().getSpi().readBuff(&byte, sizeof(byte));
    return byte;
}

void W5500_WriteByte(uint8_t byte)
{
	Application::getInstante().getSpi().writeBuff(&byte, sizeof(byte));
}



void Application::run() {
	using namespace Eni;

	static constexpr std::size_t ledThreadStackSize = 256;
	auto ledThrea = new Threading::Thread("Led", ledThreadStackSize, Threading::ThreadPriority::Normal, [this]{
		ledProcess();
	});

	HAL_GPIO_WritePin(RST_GPIO_Port, RST_Pin, GPIO_PIN_RESET);
	Threading::ThisThread::sleepForMs(1);
	HAL_GPIO_WritePin(RST_GPIO_Port, RST_Pin, GPIO_PIN_SET);
	Threading::ThisThread::sleepForMs(1000);

	reg_wizchip_cs_cbfunc(W5500_Select, W5500_Unselect);
	reg_wizchip_spi_cbfunc(W5500_ReadByte, W5500_WriteByte);
	reg_wizchip_spiburst_cbfunc(W5500_ReadBuff, W5500_WriteBuff);

	uint8_t rx_tx_buff_sizes[] = {2, 2, 2, 2, 2, 2, 2, 2};

	wizchip_init(rx_tx_buff_sizes, rx_tx_buff_sizes);

	wizchip_setnetinfo(&gWIZNETINFO);

	ctlnetwork(CN_SET_NETINFO, (void*) &gWIZNETINFO);
	Threading::ThisThread::sleepForMs(1000);

	std::array<uint8_t, 4> ip;
	uint16_t port;


	uint32_t size = buffer.size();

	buffer.fill(10);

	_transfer.start();

	while(true) {
		_transfer.addValue(buffer.data(),  size);
		Threading::ThisThread::sleepForMs(200);
		reqnr++;
	}
}


void Application::ledProcess() {
	using namespace Eni;

	while(true) {
		Threading::ThisThread::sleepForMs(500);
		Gpio::toggle(_led);
	}

}

