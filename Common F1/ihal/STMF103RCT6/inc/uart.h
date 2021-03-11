// #########################################################################################################
// Define to prevent recursive inclusion _________________________________________________________________

#ifndef __UART_H
#define __UART_H

#include <stdint.h>
#include <stdbool.h>
#include "stm32f1xx.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

	// UART MODE _____________________________________________________________________________________________
	typedef enum UART_MODE {
		MODE_RX_NO__TX_NO = 0U,
		MODE_RX_NO__TX_YES = USART_CR1_TE,
		MODE_RX_NO__TX_DMA = USART_CR1_TE | USART_CR3_DMAT,

		MODE_RX_YES__TX_NO = USART_CR1_RE,
		MODE_RX_YES__TX_YES = USART_CR1_RE | USART_CR1_TE,
		MODE_RX_YES__TX_DMA = USART_CR1_RE | USART_CR1_TE | USART_CR3_DMAT,

		MODE_RX_DMA__TX_NO = USART_CR1_RE | USART_CR3_DMAR,
		MODE_RX_DMA__TX_YES = USART_CR1_RE | USART_CR3_DMAR | USART_CR1_TE,
		MODE_RX_DMA__TX_DMA = USART_CR1_RE | USART_CR3_DMAR | USART_CR1_TE | USART_CR3_DMAT,
	}iHAL_UART_MODE;

	// UART FLOW CONTROL _____________________________________________________________________________________
	typedef enum UART_FLOW_CONTROL {
		FLOW_CONTROL_NO = 0UL,
		FLOW_CONTROL_RTS = USART_CR3_RTSE,
		FLOW_CONTROL_CTS = USART_CR3_CTSE,
		FLOW_CONTROL_RTS_CTS = USART_CR3_RTSE | USART_CR3_CTSE,
	}iHAL_UART_FLOW_CONTROL;

	// UART WORD LENGTH ______________________________________________________________________________________
	typedef enum UART_WORD_LEN {
		WORD_LEN_8 = 0x00,
		WORD_LEN_9 = USART_CR1_M,
	}iHAL_UART_WORD_LEN;

	// UART PARITY ___________________________________________________________________________________________
	typedef enum UART_PARITY {
		PARITY_NO = 0x00,
		PARITY_EVEN = USART_CR1_PCE,
		PARITY_ODD = USART_CR1_PCE | USART_CR1_PS,
	}iHAL_UART_PARITY;

	// UART STOP BYTE ________________________________________________________________________________________
	typedef enum UART_STOP_BYTE {
		STOP_BYTE_1 = 0x00,
		STOP_BYTE_0_5 = USART_CR2_STOP_0,
		STOP_BYTE_1_5 = USART_CR2_STOP_0 | USART_CR2_STOP_1,
		STOP_BYTE_2 = USART_CR2_STOP_1,
	}iHAL_UART_STOP_BYTE;

	// UART FRAME FORMAT _____________________________________________________________________________________
	typedef struct UART_FRAME_FORMAT {
		uint32_t baudrate;
		iHAL_UART_WORD_LEN word_len;
		iHAL_UART_PARITY parity;
		iHAL_UART_STOP_BYTE stop_byte;
	}iHAL_UART_FRAME_FORMAT;

	// UART INTERRYPTS _______________________________________________________________________________________
	typedef void(*iHAL_UART_HANDLE)(void);
	typedef struct UART_INTERRYPTS {
		iHAL_UART_HANDLE TXE_Transmit_DataRegEmpty;
		iHAL_UART_HANDLE CTS_flag;
		iHAL_UART_HANDLE TXC_Transmission_Complete;
		iHAL_UART_HANDLE RXNE_Received_DataReadyToBeRead;
		iHAL_UART_HANDLE ORE_EverrunErrorDetected;
		iHAL_UART_HANDLE IDLE_IdleLineDetected;
		iHAL_UART_HANDLE PE_ParityError;
		iHAL_UART_HANDLE LBD_BreakFlag;
		iHAL_UART_HANDLE EIE_Error_NE_ORE_FE;
		int32_t priority;
	}iHAL_UART_INTERRYPTS;

	// UART CONFIG ___________________________________________________________________________________________
	typedef struct Config {
		USART_TypeDef* UARTx;
		iHAL_UART_MODE mode;
		iHAL_UART_FLOW_CONTROL flow_control;
		iHAL_UART_FRAME_FORMAT frame_format;
		iHAL_UART_INTERRYPTS handle;
	}UART_Config;

	// UART __________________________________________________________________________________________________

	void iHAL_UART_Config(UART_Config* config);
	void iHAL_UART_DeConfig(USART_TypeDef* UARTx);

	void iHAL_UART_Start(USART_TypeDef* UARTx);
	void iHAL_UART_Stop(USART_TypeDef* UARTx);

	bool iHAL_UART_IsTX_Free(USART_TypeDef* UARTx);
	bool iHAL_UART_IsRX_Free(USART_TypeDef* UARTx);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __UART_H
