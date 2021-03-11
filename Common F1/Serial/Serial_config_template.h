// #########################################################################################################
// Define to prevent recursive inclusion _________________________________________________________________

#ifndef __SERIAL_COFIG_H
#define __SERIAL_COFIG_H

#include <stdint.h>
#include <stdbool.h>
#include "custom.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus	

	// #########################################################################################################
	// * Defines config ______________________________________________________________________________________

//#define USE_SERIAL_1
#define USE_SERIAL_2
//#define USE_SERIAL_3
//#define USE_SERIAL_4
//#define USE_SERIAL_5

// #########################################################################################################
// * Defines _____________________________________________________________________________________________

#define __SERIAL_NO_USED				0
#define __SERIAL_USED					1
#define __USED_SERIAL_1					__SERIAL_NO_USED
#define __USED_SERIAL_2					__SERIAL_NO_USED
#define __USED_SERIAL_3					__SERIAL_NO_USED
#define __USED_SERIAL_4					__SERIAL_NO_USED
#define __USED_SERIAL_5					__SERIAL_NO_USED

#ifdef USE_SERIAL_1
#undef __USED_SERIAL_1
#define __USED_SERIAL_1					1
#define __SP_1_IO_RX					0
#define __SP_1_IO_TX					0
#define __SP_1_IO_DE					0
#define __SP_1_BUFF_SIZE_RX				1
#define __SP_1_BUFF_SIZE_TX				1
#define __SP_1_DMA_RX					0
#define __SP_1_DMA_TX					0
#endif // USE_SERIAL_1

#ifdef USE_SERIAL_2
#undef __USED_SERIAL_2
#define __USED_SERIAL_2					1
#define __SP_2_IO_RX					{ .port = GPIOA, .pin = GPIO_PIN_3 }
#define __SP_2_IO_TX					{ .port = GPIOA, .pin = GPIO_PIN_2 }
#define __SP_2_IO_DE					{ .port = GPIOA, .pin = GPIO_PIN_1 }
#define __SP_2_BUFF_SIZE_RX				256
#define __SP_2_BUFF_SIZE_TX				256
#define __SP_2_DMA_RX					DMA1_Channel6
#define __SP_2_DMA_TX					DMA1_Channel7
#endif // USE_SERIAL_2

#ifdef USE_SERIAL_3
#undef __USED_SERIAL_3
#define __USED_SERIAL_3					1
#define __SP_3_IO_RX					0
#define __SP_3_IO_TX					0
#define __SP_3_IO_DE					0
#define __SP_3_BUFF_SIZE_RX				1
#define __SP_3_BUFF_SIZE_TX				1
#define __SP_3_DMA_RX					0
#define __SP_3_DMA_TX					0
#endif // USE_SERIAL_3

#ifdef USE_SERIAL_4
#undef __USED_SERIAL_4
#define __USED_SERIAL_4					1
#define __SP_4_IO_RX					0
#define __SP_4_IO_TX					0
#define __SP_4_IO_DE					0
#define __SP_4_BUFF_SIZE_RX				1
#define __SP_4_BUFF_SIZE_TX				1
#define __SP_4_DMA_RX					0
#define __SP_4_DMA_TX					0
#endif // USE_SERIAL_4

#ifdef USE_SERIAL_5
#undef __USED_SERIAL_5
#define __USED_SERIAL_5					1
#define __SP_5_IO_RX					0
#define __SP_5_IO_TX					0
#define __SP_5_IO_DE					0
#define __SP_5_BUFF_SIZE_RX				1
#define __SP_5_BUFF_SIZE_TX				1
#define __SP_5_DMA_RX					0
#define __SP_5_DMA_TX					0
#endif // USE_SERIAL_5

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __SERIAL_COFIG_H
