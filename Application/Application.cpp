#include <Application.h>
#include <AssertCallback.h>
#include <Math/SoundProcessing.h>

#include "ioLibrary_Driver/Ethernet/socket.h"

#include <Eni/Debug/Assert.h>

#include <stdio.h>
#include <string.h>


Application& Application::getInstante() {
	static Application app;
	return app;
}

extern SPI_HandleTypeDef hspi1;
extern DFSDM_Filter_HandleTypeDef hdfsdm1_filter0;
extern DFSDM_Filter_HandleTypeDef hdfsdm1_filter1;
extern DFSDM_Filter_HandleTypeDef hdfsdm1_filter2;
extern DFSDM_Filter_HandleTypeDef hdfsdm1_filter3;

Application::Application() : _w5500Spi(hspi1, _cs), _dfsdmFilters({UDA::Driver::DFSDMFilter{&hdfsdm1_filter0, 0},
	UDA::Driver::DFSDMFilter{&hdfsdm1_filter1, 1},
	UDA::Driver::DFSDMFilter{&hdfsdm1_filter2, 2},
	UDA::Driver::DFSDMFilter{&hdfsdm1_filter3, 3}}) {
	Eni::Gpio::initOutput(_led);

	for (auto& pin : chArray) {
		Eni::Gpio::initOutput(pin);
	}
}

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
		Application::getInstante().getLauncher().getReceiver().handleInterrupt();
	}
}

void HAL_DFSDM_FilterRegConvHalfCpltCallback(DFSDM_Filter_HandleTypeDef *hdfsdm_filter)
{
	Application::getInstante().dataOfMicrophoneCallback(hdfsdm_filter, true);
	if (hdfsdm_filter == &hdfsdm1_filter0) {
		Eni::Gpio::toggle(Application::getInstante().chArray[0]);
	}
	if (hdfsdm_filter == &hdfsdm1_filter1) {
			Eni::Gpio::toggle(Application::getInstante().chArray[1]);
	}
	if (hdfsdm_filter == &hdfsdm1_filter2) {
			Eni::Gpio::toggle(Application::getInstante().chArray[2]);
	}
	if (hdfsdm_filter == &hdfsdm1_filter3) {
			Eni::Gpio::toggle(Application::getInstante().chArray[3]);
	}
}

void HAL_DFSDM_FilterRegConvCpltCallback(DFSDM_Filter_HandleTypeDef * hdfsdm_filter)
{
	Application::getInstante().dataOfMicrophoneCallback(hdfsdm_filter, false);
	if (hdfsdm_filter == &hdfsdm1_filter0) {
		Eni::Gpio::toggle(Application::getInstante().chArray[0]);
	}
	if (hdfsdm_filter == &hdfsdm1_filter1) {
			Eni::Gpio::toggle(Application::getInstante().chArray[1]);
	}
	if (hdfsdm_filter == &hdfsdm1_filter2) {
			Eni::Gpio::toggle(Application::getInstante().chArray[2]);
	}
	if (hdfsdm_filter == &hdfsdm1_filter3) {
			Eni::Gpio::toggle(Application::getInstante().chArray[3]);
	}
}


void Application::dataOfMicrophoneCallback(DFSDM_Filter_HandleTypeDef * filter, bool isHalf) {
	for (auto& filterElement : _dfsdmFilters) {
		if (filterElement.equal(filter)) {
			filterElement.interruptCallback(isHalf);
			break;
		}
	}	
}

void Application::W5500ChipInit()  {

	HAL_GPIO_WritePin(RST_GPIO_Port, RST_Pin, GPIO_PIN_RESET);
	Eni::Threading::ThisThread::sleepForMs(500);
	HAL_GPIO_WritePin(RST_GPIO_Port, RST_Pin, GPIO_PIN_SET);
	Eni::Threading::ThisThread::sleepForMs(500);

	reg_wizchip_cs_cbfunc(W5500_Select, W5500_Unselect);
	reg_wizchip_spi_cbfunc(W5500_ReadByte, W5500_WriteByte);
	reg_wizchip_spiburst_cbfunc(W5500_ReadBuff, W5500_WriteBuff);


	uint8_t rx_tx_buff_sizes[] = {2, 2, 2, 2, 2, 2, 2, 2};

	wizchip_init(rx_tx_buff_sizes, rx_tx_buff_sizes);

}

void Application::run() {
	using namespace Eni;

	static constexpr std::size_t ledThreadStackSize = 256;
	auto ledThrea = new Threading::Thread("Led", ledThreadStackSize, Threading::ThreadPriority::Normal, [this]{
		ledProcess();
	});


	W5500ChipInit();

	wizchip_setnetinfo(&gWIZNETINFO);

	ctlnetwork(CN_SET_NETINFO, (void*) &gWIZNETINFO);

	{
		wiz_NetInfo temp = {};
		ctlnetwork(CN_GET_NETINFO,(void *)&temp);
		eniAssert(memcmp(&temp,&gWIZNETINFO,sizeof(wiz_NetInfo)) == 0);
	}

	Threading::ThisThread::sleepForMs(2000);

	_launcher.getReceiver().setHandler(&_conManager);

	_launcher.init();

	// _dfsdmF3.start();
	// _dfsdmF2.start();
	// _dfsdmF1.start();
	// _dfsdmF0.start();

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

