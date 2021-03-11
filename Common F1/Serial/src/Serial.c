// #########################################################################################################
// * Includes ____________________________________________________________________________________________

#include "Serial.h"
#include "Serial_instance.h"
#include "custom.h"
#include <string.h>

// #########################################################################################################
// * Public: _____________________________________________________________________________________________

/// configure serial port
static bool config(SERIAL e, uint32_t baudrate, FRAME_FORMAT frame_format, callback IDLE_callback);
/// write message, copy data to internal buffer
static bool write_to_buff(SERIAL e, uint8_t* data, uint32_t size);
/// write message, no copy data to internal buffer
static bool write_to_stream(SERIAL e, uint8_t* data, uint32_t size);
/// true is write compleited
static bool is_write_compleited(SERIAL e);
/// wait write
static void wait_write(SERIAL e);
/// is detect IDLE
static bool IDLE_detect(SERIAL e);
/// wait IDLE
static void wait_IDLE(SERIAL e);
/// count bytes received
static uint32_t bytes_to_read(SERIAL e);
/// read one byte
static uint8_t read_byte(SERIAL e);
/// read bytes
static void read(SERIAL e, uint8_t* out_buffer, uint32_t size);

const SERIAL_APP Serial = {
	.config = config,
	.write_to_buff = write_to_buff,
	.write_to_stream = write_to_stream,
	.is_write_compleited = is_write_compleited,
	.wait_write = wait_write,
	.IDLE_detect = IDLE_detect,
	.wait_IDLE = wait_IDLE,
	.bytes_to_read = bytes_to_read,
	.bytes_to_read = bytes_to_read,
	.read_byte = read_byte,
	.read = read,
};

// #########################################################################################################
// * Private: ____________________________________________________________________________________________

static void transmit(SERIAL_INSTANCE* const serial, uint8_t* data, uint32_t size);
static uint32_t updata_bytes_to_read(SERIAL_INSTANCE* const serial);

// #########################################################################################################
// * Public: implementation ______________________________________________________________________________

/// configure serial port
/// **************************************************************************************
static bool config(SERIAL e, uint32_t baudrate, FRAME_FORMAT frame_format, callback IDLE_callback)
{
	//  - - - - - - - -  configurations init - - - - - - - - - - - - -
	iHAL_UART_WORD_LEN word_len =
		(frame_format & SERIAL_WORD_LEN_9) == SERIAL_WORD_LEN_9 ? WORD_LEN_9 :
		WORD_LEN_8;

	iHAL_UART_PARITY parity =
		(frame_format & SERIAL_PARITY_EVEN) == SERIAL_PARITY_EVEN ? PARITY_EVEN :
		(frame_format & SERIAL_PARITY_ODD) == SERIAL_PARITY_ODD ? PARITY_ODD :
		PARITY_NO;

	iHAL_UART_STOP_BYTE stop_byte =
		(frame_format & SERIAL_STOP_0_5) == SERIAL_STOP_0_5 ? STOP_BYTE_0_5 :
		(frame_format & SERIAL_STOP_1_5) == SERIAL_STOP_1_5 ? STOP_BYTE_1_5 :
		(frame_format & SERIAL_STOP_2) == SERIAL_STOP_2 ? STOP_BYTE_2 :
		STOP_BYTE_1;

	SERIAL_INSTANCE* const serial = get_serial_instance(e);

	assert_null(serial);
	assert_null(serial->hardware.uart);
	assert_null(serial->hardware.dma_rx);
	assert_null(serial->hardware.dma_tx);
	assert_null(serial->hardware.IO.RX.port);
	assert_null(serial->hardware.IO.TX.port);
	assert_null(serial->buff_rx.pdata);
	assert_null(serial->buff_tx.pdata);

	serial->IDLE_callback = IDLE_callback;

	iHAL_UART_DeConfig(serial->hardware.uart);
	iHAL_DMA_DeInit(serial->hardware.dma_rx);
	iHAL_DMA_DeInit(serial->hardware.dma_tx);

	//  - - - - - - - -   reset buffer - - - - - - - - - - - - - - - -
	serial->buff_rx.index_in = 0, serial->buff_rx.index_out = 0;
	serial->buff_tx.index_in = 0, serial->buff_tx.index_out = 0;
	serial->is_IDLE = false, serial->is_transmit = false;

	//  - - - - - - - -  configuration UART  - - - - - - - - - - - - -
	UART_Config uart_config = {
		.UARTx = serial->hardware.uart,
		.mode = MODE_RX_DMA__TX_DMA,
		.flow_control = FLOW_CONTROL_NO,
		.frame_format = {
			.baudrate = baudrate,
			.word_len = word_len,
			.parity = parity,
			.stop_byte = stop_byte,
		},
		.handle = {
			.TXE_Transmit_DataRegEmpty = 0U,
			.CTS_flag = 0U,
			.TXC_Transmission_Complete = serial->handle_TXC,
			.RXNE_Received_DataReadyToBeRead = 0U,
			.ORE_EverrunErrorDetected = 0U,
			.IDLE_IdleLineDetected = serial->handle_IDLE,
			.PE_ParityError = 0U,
			.LBD_BreakFlag = 0U,
			.EIE_Error_NE_ORE_FE = 0U,
			.priority = 0,
		},
	};
	iHAL_UART_Config(&uart_config);

	//  - - - - - - - -  configuration dma rx channel  - - - - - - - -
	DMA_Config dma_config = {
	.memory = {
		.addr = (uint32_t*)serial->buff_rx.pdata,
		.size = SIZE_8_Bit,
		.addr_inc = true,
	},
	.periphery = {
		.addr = (uint32_t*)&serial->hardware.uart->DR,
		.size = SIZE_8_Bit,
		.addr_inc = false,
	},
	.hardware = {
		.mode = PERIPH_TO_MEM_CIR,
		.priory = CH_PRIORY_LOW,
		.handle = { 0 },
	},
	};
	iHAL_DMA_Start(serial->hardware.dma_rx, &dma_config, serial->buff_rx.size);

	// - - - - - - - - configuration GPIO  - - - - - - - - - - - - - -
	GPIO_InitTypeDef io = { 0 };

	io.Pin = 0;
	io.Mode = GPIO_MODE_AF_INPUT;
	io.Speed = GPIO_SPEED_FREQ_HIGH;
	io.Pull = GPIO_NOPULL;
	iHAL_IO_Config(serial->hardware.IO.RX, &io);

	io.Pin = 0;
	io.Mode = GPIO_MODE_AF_PP;
	io.Speed = GPIO_SPEED_FREQ_HIGH;
	io.Pull = GPIO_NOPULL;
	iHAL_IO_Config(serial->hardware.IO.TX, &io);

	if (serial->hardware.IO.DE.port) {
		iHAL_IO_Reset(serial->hardware.IO.DE);

		io.Pin = 0;
		io.Mode = GPIO_MODE_OUTPUT_PP;
		io.Speed = GPIO_SPEED_FREQ_HIGH;
		io.Pull = GPIO_PULLDOWN;
		iHAL_IO_Config(serial->hardware.IO.DE, &io);
	}

	return true;
}

/// write message, copy data to internal buffer
/// **************************************************************************************
static bool write_to_buff(SERIAL e, uint8_t* data, uint32_t size)
{
	SERIAL_INSTANCE* const serial = get_serial_instance(e);
	bool res = false;

	if (is_write_compleited(e))
	{
		serial->is_transmit = false;

		uint32_t count = (size < serial->buff_tx.size) ? size : serial->buff_tx.size;
		memcpy(serial->buff_tx.pdata, data, count);
		transmit(serial, serial->buff_tx.pdata, count);
		res = true;
	}

	return res;
}

/// write message, no copy data to internal buffer
/// **************************************************************************************
static bool write_to_stream(SERIAL e, uint8_t* data, uint32_t size)
{
	SERIAL_INSTANCE* const serial = get_serial_instance(e);	
	bool res = false;

	if (is_write_compleited(e))
	{
		serial->is_transmit = false;

		uint32_t count = (size < serial->buff_tx.size) ? size : serial->buff_tx.size;
		transmit(serial, data, count);
		res = true;
	}

	return res;
}

/// true is write compleited
/// **************************************************************************************
static bool is_write_compleited(SERIAL e)
{
	SERIAL_INSTANCE* const serial = get_serial_instance(e);

	return
		serial->is_transmit ||
		iHAL_UART_IsTX_Free(serial->hardware.uart);
}

/// wait write
/// **************************************************************************************
static void wait_write(SERIAL e)
{
	while (!is_write_compleited(e))
	{
		__NOP();
	}
}

/// is detect IDLE
/// **************************************************************************************
static bool IDLE_detect(SERIAL e)
{
	SERIAL_INSTANCE* const serial = get_serial_instance(e);
	bool idle = serial->is_IDLE || iHAL_UART_IsRX_Free(serial->hardware.uart);

	serial->is_IDLE = false;
	return idle;
}

/// wait IDLE
/// **************************************************************************************
static void wait_IDLE(SERIAL e)
{
	if (!IDLE_detect(e))
	{
		SERIAL_INSTANCE* const serial = get_serial_instance(e);
		uint32_t bytes = -1;

		while (bytes != serial->hardware.dma_rx->CNDTR)
		{
			bytes = serial->hardware.dma_rx->CNDTR;

			HAL_Delay(1);
		}
	}
}

/// count bytes received
/// **************************************************************************************
static uint32_t bytes_to_read(SERIAL e)
{
	SERIAL_INSTANCE* const serial = get_serial_instance(e);
	return updata_bytes_to_read(serial);
}

/// read one byte
/// **************************************************************************************
static uint8_t read_byte(SERIAL e)
{
	SERIAL_INSTANCE* const serial = get_serial_instance(e);
	uint32_t index = serial->buff_rx.index_out;

	/* increment index out */
	serial->buff_rx.index_out =
		((index + 1) < serial->buff_rx.size) ?
		index + 1 :
		0;

	return serial->buff_rx.pdata[index];
}

/// read bytes
/// **************************************************************************************
static void read(SERIAL e, uint8_t* out_buffer, uint32_t size)
{
	SERIAL_INSTANCE* const serial = get_serial_instance(e);
	uint32_t count = updata_bytes_to_read(serial);
	count =
		(count < size) ?
		count :
		size;

	uint32_t index = serial->buff_rx.index_out;
	uint8_t* _out = out_buffer;

	while (count--)
	{
		*_out++ = serial->buff_rx.pdata[index++];
		index = (index < serial->buff_rx.size) ? index : 0;
	}

	serial->buff_rx.index_out = index;
}

// #########################################################################################################
// * Private: Implementation _____________________________________________________________________________

/// **************************************************************************************
static void transmit(SERIAL_INSTANCE* const serial, uint8_t* data, uint32_t size)
{
	if (serial->hardware.IO.DE.port)
		iHAL_IO_Set(serial->hardware.IO.DE);

	//  - - - - - - - -  configuration dma tx channel  - - - - - - - -
	DMA_Config dma_config = {
	.memory = {
		.addr = (uint32_t*)data,
		.size = SIZE_8_Bit,
		.addr_inc = true,
	},
	.periphery = {
		.addr = (uint32_t*)&serial->hardware.uart->DR,
		.size = SIZE_8_Bit,
		.addr_inc = false,
	},
	.hardware = {
		.mode = MEM_TO_PERIPH_ONE,
		.priory = CH_PRIORY_LOW,
		.handle = { 0U },
	},
	};
	iHAL_DMA_Start(serial->hardware.dma_tx, &dma_config, size);
}

/// **************************************************************************************
static uint32_t updata_bytes_to_read(SERIAL_INSTANCE* const serial)
{
	serial->buff_rx.index_in = serial->buff_rx.size - serial->hardware.dma_rx->CNDTR;

	uint32_t OUT = serial->buff_rx.index_out;
	uint32_t IN = serial->buff_rx.index_in;

	/* return count bytes */
	return
		IN >= OUT ?
		IN - OUT :
		(serial->buff_rx.size + IN) - OUT;
}

// #########################################################################################################
// * Interrupt ___________________________________________________________________________________________

/// **************************************************************************************
void IDLE(SERIAL_INSTANCE* const serial)
{
	updata_bytes_to_read(serial);
	serial->is_IDLE = true;

	if (serial->IDLE_callback)
		serial->IDLE_callback(serial->e);
}

/// **************************************************************************************
void TXC(SERIAL_INSTANCE* const serial)
{
	serial->is_transmit = true;

	if (serial->hardware.IO.DE.port)
		iHAL_IO_Reset(serial->hardware.IO.DE);
}

/// #########################################################################################################
/// * End file Serial.c                                                                               #####
/// #####################################################################################################
