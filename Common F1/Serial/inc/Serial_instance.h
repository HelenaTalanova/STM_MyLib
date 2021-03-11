// #########################################################################################################
// Define to prevent recursive inclusion _________________________________________________________________

#ifndef __SERIAL_INSTANCE_H
#define __SERIAL_INSTANCE_H

#include <stdint.h>
#include <stdbool.h>
#include "stm32f1xx.h"
#include "Serial.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus	

	typedef struct {
		uint8_t* pdata;
		uint32_t index_in;
		uint32_t index_out;
		uint32_t size;
	}SERIAL_BUFF;

	typedef struct {
		USART_TypeDef* uart;
		DMA_Channel_TypeDef* dma_rx;
		DMA_Channel_TypeDef* dma_tx;
		struct IO {
			IO_t RX;
			IO_t TX;
			IO_t DE;
		}IO;
	}SERIAL_HARDWARE;

	typedef struct TypeDef_SerialPort {
		SERIAL_HARDWARE hardware;
		SERIAL_BUFF buff_tx;
		SERIAL_BUFF buff_rx;
		void(*handle_IDLE)(void);
		void(*handle_TXC)(void);
		callback IDLE_callback;
		volatile bool is_transmit;
		volatile bool is_IDLE;
		uint32_t baudrate;
		SERIAL e;
	}SERIAL_INSTANCE;

	SERIAL_INSTANCE* get_serial_instance(SERIAL e);

	extern void IDLE(SERIAL_INSTANCE* const serial);
	extern void TXC(SERIAL_INSTANCE* const serial);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __SERIAL_INSTANCE_H
