// #########################################################################################################
// * Includes ____________________________________________________________________________________________

#include "Serial_instance.h"
#include "Serial.h"
#include "custom.h"
#include "common.h"

/// **************************************************************************************
static SERIAL_INSTANCE serial_instances[SERIAL_COUNT];

// #########################################################################################################
// * Public: implementation ______________________________________________________________________________

/// **************************************************************************************
inline SERIAL_INSTANCE* get_serial_instance(SERIAL e)
{
	if ((e != SERIAL_NO) && (e <= SERIAL_COUNT))
		return &serial_instances[e - 1];

	Error_Handler_Mes("GET_NO_USED_SERIAL");
	return 0;
}

// #########################################################################################################
// * Private: Implementation _____________________________________________________________________________

#define GET_INSTANCE(E)		get_serial_instance(E)

#ifdef USE_SERIAL_1
/// **************************************************************************************
static uint8_t buff_instance_rx1[__SP_1_BUFF_SIZE_RX];
static uint8_t buff_instance_tx1[__SP_1_BUFF_SIZE_TX];
static void IDLE1(void) { IDLE(GET_INSTANCE(SERIAL_1)); }
static void TXC1(void) { TXC(GET_INSTANCE(SERIAL_1)); }
#endif // USE_SERIAL_1

#ifdef USE_SERIAL_2
/// **************************************************************************************
static uint8_t buff_instance_rx2[__SP_2_BUFF_SIZE_RX];
static uint8_t buff_instance_tx2[__SP_2_BUFF_SIZE_TX];
static void IDLE2(void) { IDLE(GET_INSTANCE(SERIAL_2)); }
static void TXC2(void) { TXC(GET_INSTANCE(SERIAL_2)); }
#endif // USE_SERIAL_2

#ifdef USE_SERIAL_3
/// **************************************************************************************
static uint8_t buff_instance_rx3[__SP_3_BUFF_SIZE_RX];
static uint8_t buff_instance_tx3[__SP_3_BUFF_SIZE_TX];
static void IDLE3(void) { IDLE(GET_INSTANCE(SERIAL_3)); }
static void TXC3(void) { TXC(GET_INSTANCE(SERIAL_3)); }
#endif // USE_SERIAL_3

#ifdef USE_SERIAL_4
/// **************************************************************************************
static uint8_t buff_instance_rx4[__SP_4_BUFF_SIZE_RX];
static uint8_t buff_instance_tx4[__SP_4_BUFF_SIZE_TX];
static void IDLE4(void) { IDLE(GET_INSTANCE(SERIAL_4)); }
static void TXC4(void) { TXC(GET_INSTANCE(SERIAL_4)); }
#endif // USE_SERIAL_4

#ifdef USE_SERIAL_5
/// **************************************************************************************
static uint8_t buff_instance_rx5[__SP_5_BUFF_SIZE_RX];
static uint8_t buff_instance_tx5[__SP_5_BUFF_SIZE_TX];
static void IDLE5(void) { IDLE(GET_INSTANCE(SERIAL_5)); }
static void TXC5(void) { TXC(GET_INSTANCE(SERIAL_5)); }
#endif // USE_SERIAL_5

// #########################################################################################################
// * Private: instance ___________________________________________________________________________________

static SERIAL_INSTANCE serial_instances[SERIAL_COUNT] = {
#ifdef USE_SERIAL_1
	/// **************************************************************************************
	{
		.hardware = {.uart = USART1, .dma_rx = __SP_1_DMA_RX, .dma_tx = __SP_1_DMA_TX, .IO = {.RX = __SP_1_IO_RX, .TX = __SP_1_IO_TX, .DE = __SP_1_IO_DE, }, },
		.buff_rx = {.pdata = buff_instance_rx1, .size = ARRAY_SIZE(buff_instance_rx1), .index_in = 0, .index_out = 0, },
		.buff_tx = {.pdata = buff_instance_tx1, .size = ARRAY_SIZE(buff_instance_tx1), .index_in = 0, .index_out = 0, },
		.handle_IDLE = IDLE1,
		.handle_TXC = TXC1,
		.IDLE_callback = 0UL,
		.is_transmit = false,
		.is_IDLE = false,
		.baudrate = 0,
		.e = SERIAL_1,
	},
	#endif // USE_SERIAL_1

	#ifdef USE_SERIAL_2
	/// **************************************************************************************
	{
		.hardware = {.uart = USART2, .dma_rx = __SP_2_DMA_RX, .dma_tx = __SP_2_DMA_TX, .IO = {.RX = __SP_2_IO_RX, .TX = __SP_2_IO_TX, .DE = __SP_2_IO_DE, }, },
		.buff_rx = {.pdata = buff_instance_rx2, .size = ARRAY_SIZE(buff_instance_rx2), .index_in = 0, .index_out = 0, },
		.buff_tx = {.pdata = buff_instance_tx2, .size = ARRAY_SIZE(buff_instance_tx2), .index_in = 0, .index_out = 0, },
		.handle_IDLE = IDLE2,
		.handle_TXC = TXC2,
		.IDLE_callback = 0UL,
		.is_transmit = false,
		.is_IDLE = false,
		.baudrate = 0,
		.e = SERIAL_2,
	},
	#endif // USE_SERIAL_2

	#ifdef USE_SERIAL_3
	/// **************************************************************************************
	{
		.hardware = {.uart = USART3, .dma_rx = __SP_3_DMA_RX, .dma_tx = __SP_3_DMA_TX, .IO = {.RX = __SP_3_IO_RX, .TX = __SP_3_IO_TX, .DE = __SP_3_IO_DE, }, },
		.buff_rx = {.pdata = buff_instance_rx3, .size = ARRAY_SIZE(buff_instance_rx3), .index_in = 0, .index_out = 0, },
		.buff_tx = {.pdata = buff_instance_tx3, .size = ARRAY_SIZE(buff_instance_tx3), .index_in = 0, .index_out = 0, },
		.handle_IDLE = IDLE3,
		.handle_TXC = TXC3,
		.IDLE_callback = 0UL,
		.is_transmit = false,
		.is_IDLE = false,
		.baudrate = 0,
		.e = SERIAL_3,
	},
	#endif // USE_SERIAL_3

	#ifdef USE_SERIAL_4
	/// **************************************************************************************
	{
		.hardware = {.uart = UART4, .dma_rx = __SP_4_DMA_RX, .dma_tx = __SP_4_DMA_TX, .IO = {.RX = __SP_4_IO_RX, .TX = __SP_4_IO_TX, .DE = __SP_4_IO_DE, }, },
		.buff_rx = {.pdata = buff_instance_rx4, .size = ARRAY_SIZE(buff_instance_rx4), .index_in = 0, .index_out = 0, },
		.buff_tx = {.pdata = buff_instance_tx4, .size = ARRAY_SIZE(buff_instance_tx4), .index_in = 0, .index_out = 0, },
		.handle_IDLE = IDLE4,
		.handle_TXC = TXC4,
		.IDLE_callback = 0UL,
		.is_transmit = false,
		.is_IDLE = false,
		.baudrate = 0,
		.e = SERIAL_4,
	},
	#endif // USE_SERIAL_4

	#ifdef USE_SERIAL_5
	/// **************************************************************************************
	{
		.hardware = {.uart = UART5, .dma_rx = __SP_5_DMA_RX, .dma_tx = __SP_5_DMA_TX, .IO = {.RX = __SP_5_IO_RX, .TX = __SP_5_IO_TX, .DE = __SP_5_IO_DE, }, },
		.buff_rx = {.pdata = buff_instance_rx5, .size = ARRAY_SIZE(buff_instance_rx5), .index_in = 0, .index_out = 0, },
		.buff_tx = {.pdata = buff_instance_tx5, .size = ARRAY_SIZE(buff_instance_tx5), .index_in = 0, .index_out = 0, },
		.handle_IDLE = IDLE5,
		.handle_TXC = TXC5,
		.IDLE_callback = 0UL,
		.is_transmit = false,
		.is_IDLE = false,
		.baudrate = 0,
		.e = SERIAL_5,
	},
	#endif // USE_SERIAL_5

};

/// #########################################################################################################
/// * End file Serial_instance.c                                                                      #####
/// #####################################################################################################
