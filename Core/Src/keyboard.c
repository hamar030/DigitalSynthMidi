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
extern void osDelay(uint32_t ticks);
//extern void SendCDCData(char *data);

typedef struct __SPIDebug {
	uint8_t spi_state;
	uint8_t spi_error;
	uint8_t dma_state;
	uint8_t dma_error;
} SPIDebug;

typedef enum __PianoMode{
	SingleMode = 0x00,
	SplitMode = 0x01,
	QuadMode = 0x02
} PianoModes;

uint8_t spitx_buffer[64];
uint8_t spirx_buffer[64];
uint8_t cdcbuffer[512];

uint8_t Data165;
uint8_t Data595[] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };
uint8_t Notes[11][12] = {
		{0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B},
		{0x0C,0x0D,0x0E,0x0F,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17},
		{0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,0x20,0x21,0x22,0x23},
		{0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F},
		{0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B},
		{0x3C,0x3D,0x3E,0x3F,0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47},
		{0x48,0x49,0x4A,0x4B,0x4C,0x4D,0x4E,0x4F,0x50,0x51,0x52,0x53},
		{0x54,0x55,0x56,0x57,0x58,0x59,0x5A,0x5B,0x5C,0x5D,0x5E,0x5F},
		{0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6A,0x6B},
		{0x6C,0x6D,0x6E,0x6F,0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77},
		{0x78,0x79,0x7A,0x7B,0x7C,0x7D,0x7E,0x7F}
};
static int KeyState[8][8]={0};
static uint8_t Channel, Velocity;
int DefaultOctave = 4;
PianoModes DefaultMode = SingleMode;
void StartKeyboardTask(void *argument) {
	static SPIDebug spirx, spitx;
	static uint32_t Tick3;
	static int row, octave, PianoMode, notex=0, notey=0, splitter;
	int col, loc, coll[8]={0};
	uint8_t Notes_Buff[8][8]={0};
	char* pianomode;

	octave=DefaultOctave;
	PianoMode=DefaultMode;
	Channel=0;
	Velocity=75;

	HAL_GPIO_WritePin(SPISS_Port, SS595_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(SPISS_Port, SS165_Pin, GPIO_PIN_SET);

	/*
	 * loop
	 */

	for (;;) {
		//if (osKernelGetTickCount() - Tick2 >= 5) {
		//	Tick2 = osKernelGetTickCount();
			//if (row < 8) {
				//row++;
		//	} else {
			//	row = 1;
		//	}
		for (int x=1;x<8;x++){
			for(int y =0;y<8;y++){
				if(PianoMode == SingleMode){
					pianomode="Single Mode";
					if(splitter==56){
						notex=0;
						notey=0;
						splitter=0;
					}
				}
				else if(PianoMode == SplitMode){
					pianomode="Split Mode";
					if(splitter==28){
						notex=0;
						notey=0;
						splitter=0;
					}
				}
				else if (PianoMode == QuadMode){
					pianomode="Quad Mode";
					if(splitter==14){
						notex=0;
						notey=0;
						splitter=0;
					}
				}

				Notes_Buff[x][y] = Notes[(octave+notex)][notey];
				  if(notey<=12){
					  notey++;
				  }else{
					  notey=0;
					  notex++;
				  }
				splitter++;
			}
		}

		for (row = 1; row < 8; row++) {

			HAL_GPIO_WritePin(SPISS_Port, SS595_Pin, GPIO_PIN_RESET);

			*spitx_buffer = Data595[row];
			spitx.spi_state = HAL_SPI_Transmit_DMA(&hspi2, spitx_buffer, sizeof(*spitx_buffer));
			spitx.spi_error = HAL_SPI_GetError(&hspi2);
			spitx.dma_state = HAL_DMA_GetState(&hdma_spi2_tx);
			spitx.dma_error = HAL_DMA_GetError(&hdma_spi2_tx);

			HAL_GPIO_WritePin(SPISS_Port, SS595_Pin, GPIO_PIN_SET);


			HAL_GPIO_WritePin(SPISS_Port, SS165_Pin, GPIO_PIN_RESET);
			osDelay(1);
			HAL_GPIO_WritePin(SPISS_Port, SS165_Pin, GPIO_PIN_SET);
			osDelay(1);

			spirx.spi_state = HAL_SPI_Receive_DMA(&hspi2, spirx_buffer, sizeof(*spirx_buffer));
			spirx.spi_error = HAL_SPI_GetError(&hspi2);
			spirx.dma_state = HAL_DMA_GetState(&hdma_spi2_rx);
			spirx.dma_error = HAL_DMA_GetError(&hdma_spi2_rx);

			loc=8;
			for (col = 0; col < 8; col++) {
				Data165 = *spirx_buffer;
				coll[col]= (( Data165 << loc) >> 8)%2;

				if((!KeyState[row][col]) && coll[col]) {
					sendNoteOn(Channel, Notes_Buff[row][col], Velocity);
					processMidiMessage();
					/*
					  CDC_Transmit_FS((uint8_t*) cdcbuffer,
									sprintf((char*) &cdcbuffer,"Note On[%d][%d]: 0x%02X \r\n",row,col,Notes_Buff[row][col]));
					*/
					KeyState[row][col]=1;
				};


				if(KeyState[row][col] && (!coll[col])) {
					sendNoteOff(Channel, Notes_Buff[row][col], Velocity);
					processMidiMessage();
					/*
					CDC_Transmit_FS((uint8_t*) cdcbuffer,
									sprintf((char*) &cdcbuffer,"Note Off[%d][%d]: 0x%02X \r\n",row,col,Notes_Buff[row][col]));
					*/
					KeyState[row][col]=0;

				};
				loc--;
			}
		}

			//}



		//for (int i=0; i<7 ;i++){
		//	CDC_Transmit_FS((uint8_t*) cdcbuffer,
		//			sprintf((char*) &cdcbuffer, "spirx[%d]: 0x%02x | ",i,spirx_buffer[i]));
		//}
		/*
		if (osKernelGetTickCount() - Tick3 >= 50) {
			Tick3 = osKernelGetTickCount();
			CDC_Transmit_FS((uint8_t*) cdcbuffer,
				sprintf((char*) &cdcbuffer,
						"\033c \r\n"
						"SPI Tx: buf[0x%02x], state[0x%02x], error[0x%02x] | DMA Tx: state[0x%02x], error[0x%02x] \r\n"
						"SPI Rx: buf[0x%02x], state[0x%02x], error[0x%02x] | DMA Rx: state[0x%02x], error[0x%02x] \r\n",
						*spitx_buffer, spitx.spi_state, spitx.spi_error, spirx.dma_state, spitx.dma_error,
						*spirx_buffer, spirx.spi_state, spirx.spi_error, spirx.dma_state, spirx.dma_error
			));
			osDelay(1);
			CDC_Transmit_FS((uint8_t*) cdcbuffer,
				sprintf((char*) &cdcbuffer,
						"Piano Mode: %s \r\n"
						"%d|%d|%d|%d|%d|%d|%d|%d \r\n",
						pianomode,
						coll[0],coll[1],coll[2],coll[3],coll[4],coll[5],coll[6],coll[7]
			));

		}*/
	}
}

