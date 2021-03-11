/* Define to prevent recursive inclusion -------------------------------------*/

#ifndef __SERIAL_PORT_DEF_H
#define __SERIAL_PORT_DEF_H

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

#include <stdint.h>
#include <stdbool.h>
#include "io.h"
#include "pcb_config.h"

#define USE_SERIAL_1					1
#define USE_SERIAL_2					1

#if (USE_SERIAL_1)
#define SERIAL_1						(0)
#define SERIAL_1_IO_RX					IO_RX
#define SERIAL_1_IO_TX					IO_TX
#define SERIAL_1_IO_DE					IO_NO_PIN_PORT
#define SERIAL_1_BUFF_SIZE_RX			128
#define SERIAL_1_BUFF_SIZE_TX			128
#define SERIAL_1_DMA_TX					DMA1_Channel2
#define SERIAL_1_DMA_RX					DMA1_Channel3
#endif // USE_SERIAL_1

#if (USE_SERIAL_2)
#define SERIAL_2						(USE_SERIAL_1)
#define SERIAL_2_IO_RX					IO_RS485_RX
#define SERIAL_2_IO_TX					IO_RS485_TX
#define SERIAL_2_IO_DE					IO_RS485_DE
#define SERIAL_2_BUFF_SIZE_RX			128
#define SERIAL_2_BUFF_SIZE_TX			128
#define SERIAL_2_DMA_TX					DMA1_Channel4
#define SERIAL_2_DMA_RX					DMA1_Channel5
#endif // USE_SERIAL_2

#define USE_SERIAL_PORT_COUNT			(USE_SERIAL_1 + USE_SERIAL_2)

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __SERIAL_PORT_DEF_H
