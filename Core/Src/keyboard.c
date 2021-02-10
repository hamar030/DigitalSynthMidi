/*
 * controller.c
 *
 *  Created on: Jan 12, 2021
 *      Author: bimo
 */

#include "keyboard.h"
#include "spi.h"
#include "usbd_cdc_if.h"
#include "usbd_midi_if.h"

#include "math.h"

extern uint32_t osKernelGetTickCount(void);
//extern void SendCDCData(char *data);

typedef struct __SPIDebug {
	uint8_t spi_state;
	uint8_t spi_error;
	uint8_t dma_state;
	uint8_t dma_error;
} SPIDebug;

uint8_t spitx_buffer[100];
uint8_t spirx_buffer[100];
uint8_t cdcbuffer[100];
static uint32_t Tick2;

uint8_t dataa[] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };

void StartKeyboardTask(void *argument) {
	static int row;
	static SPIDebug spirx, spitx;
//loop
	for (;;) {

		if (osKernelGetTickCount() - Tick2 >= 5) {
			Tick2 = osKernelGetTickCount();
			//SendCDCData("Spi Send  ");
			if (row < 7) {
				row++;
			} else {
				row = 0;
			}
			//for (row = 0; row < 7; row++) {

			HAL_GPIO_WritePin(SPISS_Port, SS595_Pin, GPIO_PIN_RESET);

			*spitx_buffer = dataa[row];
			spitx.spi_state = HAL_SPI_Transmit_DMA(&hspi2, spitx_buffer, 1);
			spitx.spi_error = HAL_SPI_GetError(&hspi2);
			spitx.dma_state = HAL_DMA_GetState(&hdma_spi2_tx);
			spitx.dma_error = HAL_DMA_GetError(&hdma_spi2_tx);

			HAL_GPIO_WritePin(SPISS_Port, SS595_Pin, GPIO_PIN_SET);

			for (int col = 0; col < 7; col++) {
				HAL_GPIO_WritePin(SPISS_Port, SS165_Pin, GPIO_PIN_RESET);

				spirx.spi_state = HAL_SPI_Receive_DMA(&hspi2, spirx_buffer, 1);
				spirx.spi_error = HAL_SPI_GetError(&hspi2);
				spirx.dma_state = HAL_DMA_GetState(&hdma_spi2_rx);
				spirx.dma_error = HAL_DMA_GetError(&hdma_spi2_rx);

				HAL_GPIO_WritePin(SPISS_Port, SS165_Pin, GPIO_PIN_SET);
			}

			CDC_Transmit_FS((uint8_t*) cdcbuffer,
					sprintf((char*) &cdcbuffer,
							"\033c "
									"SPI Tx: buf[0x%02x], state[0x%02x], error[0x%02x] | DMA Tx: state[0x%02x], error[0x%02x] \r\n "
									"SPI Rx: buf[0x%02x], state[0x%02x], error[0x%02x] | DMA Rx: state[0x%02x], error[0x%02x]"
							"\r\n",
							*spitx_buffer, spitx.spi_state,spitx.spi_error, spirx.dma_state, spitx.dma_error,
							*spirx_buffer, spirx.spi_state, spirx.spi_error, spirx.dma_state, spirx.dma_error));

			//}
		}
	}
}

