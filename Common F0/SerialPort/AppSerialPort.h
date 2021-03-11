/* Define to prevent recursive inclusion -------------------------------------*/

#ifndef __APP_SERIAL_PORT_H
#define __APP_SERIAL_PORT_H

#include <stdint.h>
#include <stdbool.h>
#include "stm32f0xx.h"
#include "io.h"
#include "SerialPortDef.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus		

	typedef struct {
		void(*IDLE)(void);
		void(*TXC)(void);
	}SP_Interrypt;

	typedef struct {
		uint8_t* buffer;
		uint32_t index_in;
		uint32_t index_out;
		uint32_t size;
	}SP_RingBuffer;

	typedef struct {
		USART_TypeDef* uart;
		struct
		{
			DMA_Channel_TypeDef* ch_rx;
			DMA_Channel_TypeDef* ch_tx;
		}DMA;
		struct
		{
			IO_t rx;
			IO_t tx;
			IO_t de;
		}IO;
	}SP_Hardware;

	typedef struct DATA_SerialPort
	{
		SP_Hardware hardware;
		SP_RingBuffer tx;
		SP_RingBuffer rx;
		SP_Interrypt interrypr;
		volatile bool is_transmit;
		volatile bool is_IDLE;
		uint8_t baudrate;
		void(*call_back)(void);
	}DATA_SerialPort;

	typedef DATA_SerialPort* const HSerialPort;

	extern void IDLE(HSerialPort serial);
	extern void TXC(HSerialPort serial);

	extern HSerialPort H_SERIAL_PORTs;

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __APP_SERIAL_PORT_H
