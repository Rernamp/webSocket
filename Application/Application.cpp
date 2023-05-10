#include <Application.h>
#include <AssertCallback.h>
#include "ioLibrary_Driver/Ethernet/socket.h"

#include <stdio.h>
#include <string.h>


Application& Application::getInstante() {
	static Application app;
	return app;
}

extern SPI_HandleTypeDef hspi1;
extern DFSDM_Filter_HandleTypeDef hdfsdm1_filter0;

Application::Application() : _w5500Spi(hspi1, _cs), _dfsdm(hdfsdm1_filter0) {
	Eni::Gpio::initOutput(_led);
}


#define HTTP_SOCKET     0
#define PORT_TCPS		    5000
#define DATA_BUF_SIZE   2048
uint8_t gDATABUF[DATA_BUF_SIZE];



uint8_t stat;
uint8_t reqnr;

//extern "C" void freertosAssert(int x) {
//	if (x == 0) {
//		for (;;) {
//
//		}
//	}
//}

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

extern "C" void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	if (GPIO_Pin == INT_Pin) {
		Application::getInstante().getTransfer().interruptCallback();
	}
}

void HAL_DFSDM_FilterRegConvHalfCpltCallback(DFSDM_Filter_HandleTypeDef *hdfsdm_filter)
{
	Application::getInstante().dataOfMicrophoneCallback(true);
}

void HAL_DFSDM_FilterRegConvCpltCallback(DFSDM_Filter_HandleTypeDef *hdfsdm_filter)
{
	Application::getInstante().dataOfMicrophoneCallback(false);
}


void Application::dataOfMicrophoneCallback(bool isHalf) {
	_dfsdm.interruptCallback(isHalf);
}

void Application::run() {
	using namespace Eni;

	static constexpr std::size_t ledThreadStackSize = 256;
	auto ledThrea = new Threading::Thread("Led", ledThreadStackSize, Threading::ThreadPriority::Normal, [this]{
		ledProcess();
	});

	_dfsdm.setLisnter(this);

	HAL_GPIO_WritePin(RST_GPIO_Port, RST_Pin, GPIO_PIN_RESET);
	Threading::ThisThread::sleepForMs(10);
	HAL_GPIO_WritePin(RST_GPIO_Port, RST_Pin, GPIO_PIN_SET);
	Threading::ThisThread::sleepForMs(1000);

	reg_wizchip_cs_cbfunc(W5500_Select, W5500_Unselect);
	reg_wizchip_spi_cbfunc(W5500_ReadByte, W5500_WriteByte);
	reg_wizchip_spiburst_cbfunc(W5500_ReadBuff, W5500_WriteBuff);

	uint8_t rx_tx_buff_sizes[] = {2, 2, 2, 2, 2, 2, 2, 2};

	wizchip_init(rx_tx_buff_sizes, rx_tx_buff_sizes);

	wizchip_setnetinfo(&gWIZNETINFO);

	ctlnetwork(CN_SET_NETINFO, (void*) &gWIZNETINFO);
	Threading::ThisThread::sleepForMs(2000);

	_transmittion.setSender(_transfer);
	_transfer.start();

	while(true) {

		Threading::ThisThread::yield();
	}
}


void Application::ledProcess() {
	using namespace Eni;

	while(true) {
		Threading::ThisThread::sleepForMs(500);
		Gpio::toggle(_led);
	}

}

