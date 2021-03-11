/* --------------------------------------------------------------------------------------------------------------------------------------- */
/* Includes ------------------------------------------------------------------------------------------------------------------------------ */
/* ....................................................................................................................................... */

#include "APPSerialPort.h"
#include "Common.h"
#include "custom.h"

/* --------------------------------------------------------------------------------------------------------------------------------------- */
/* Defines ------------------------------------------------------------------------------------------------------------------------------- */
/* ....................................................................................................................................... */

#if (USE_SERIAL_PORT_COUNT == 0)
#undef USE_SERIAL_1
#define USE_SERIAL_1					1

#define SERIAL_1						0
#define SERIAL_1_BUFF_SIZE_RX			1
#define SERIAL_1_BUFF_SIZE_TX			1
#define SERIAL_1_IO_RX					IO_NO_PIN_PORT
#define SERIAL_1_IO_TX					IO_NO_PIN_PORT
#define SERIAL_1_IO_DE					IO_NO_PIN_PORT
#define SERIAL_1_DMA_TX					NULL
#define SERIAL_1_DMA_RX					NULL
#endif // default USE_SERIAL_1 

/* --------------------------------------------------------------------------------------------------------------------------------------- */
/* Private ------------------------------------------------------------------------------------------------------------------------------- */
/* ....................................................................................................................................... */

#if (USE_SERIAL_1)
static uint8_t uart_ring_buffer_rx_1[SERIAL_1_BUFF_SIZE_RX];
static uint8_t uart_ring_buffer_tx_1[SERIAL_1_BUFF_SIZE_TX];
static void IDLE1(void);
static void TXC1(void);
#endif // USE_SERIAL_1

#if (USE_SERIAL_2)
static uint8_t uart_ring_buffer_rx_2[SERIAL_2_BUFF_SIZE_RX];
static uint8_t uart_ring_buffer_tx_2[SERIAL_2_BUFF_SIZE_TX];
static void IDLE2(void);
static void TXC2(void);
#endif // USE_SERIAL_2

static DATA_SerialPort _data_serial_port[USE_SERIAL_PORT_COUNT] = {
#if (USE_SERIAL_1)
{
	.hardware = {.uart = USART1,
					.DMA = {.ch_rx = SERIAL_1_DMA_RX, .ch_tx = SERIAL_1_DMA_TX,},
					.IO = {.rx = SERIAL_1_IO_RX, .tx = SERIAL_1_IO_TX, .de = SERIAL_1_IO_DE, },
				},
	.rx = {.buffer = uart_ring_buffer_rx_1, .size = sizeof(uart_ring_buffer_rx_1), .index_in = 0, .index_out = 0, },
	.tx = {.buffer = uart_ring_buffer_tx_1, .size = sizeof(uart_ring_buffer_tx_1), .index_in = 0, .index_out = 0, },
	.interrypr = {.IDLE = IDLE1, .TXC = TXC1, },
	.is_transmit = false,
	.is_IDLE = false,
},
#endif // USE_SERIAL_1
#if (USE_SERIAL_2)
{
	.hardware = {.uart = USART2,
					.DMA = {.ch_rx = SERIAL_2_DMA_RX, .ch_tx = SERIAL_2_DMA_TX,},
					.IO = {.rx = SERIAL_2_IO_RX, .tx = SERIAL_2_IO_TX, .de = SERIAL_2_IO_DE, },
				},
	.rx = {.buffer = uart_ring_buffer_rx_2, .size = sizeof(uart_ring_buffer_rx_2), .index_in = 0, .index_out = 0, },
	.tx = {.buffer = uart_ring_buffer_tx_2, .size = sizeof(uart_ring_buffer_tx_2), .index_in = 0, .index_out = 0, },
	.interrypr = {.IDLE = IDLE2, .TXC = TXC2, },
	.is_transmit = false,
	.is_IDLE = false,
},
#endif // USE_SERIAL_2
};

#if (USE_SERIAL_1)
__weak void SERIAL_1_IDLE(void) { UNUSED(0); }
static void IDLE1(void) { IDLE(&_data_serial_port[SERIAL_1]); SERIAL_1_IDLE(); }
static void TXC1(void) { TXC(&_data_serial_port[SERIAL_1]); }
#endif // USE_SERIAL_1

#if (USE_SERIAL_2)
__weak void SERIAL_2_IDLE(void) { UNUSED(0); }
static void IDLE2(void) { IDLE(&_data_serial_port[SERIAL_2]); SERIAL_2_IDLE(); }
static void TXC2(void) { TXC(&_data_serial_port[SERIAL_2]); }
#endif // USE_SERIAL_2

/* --------------------------------------------------------------------------------------------------------------------------------------- */
/* Public -------------------------------------------------------------------------------------------------------------------------------- */
/* ....................................................................................................................................... */

HSerialPort H_SERIAL_PORTs = _data_serial_port;

/* End ----------------------------------------------------------------------------------------------------------------------------------- */
/* ....................................................................................................................................... */
