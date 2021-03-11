/* --------------------------------------------------------------------------------------------------------------------------------------- */
/* Includes ------------------------------------------------------------------------------------------------------------------------------ */
/* ....................................................................................................................................... */

#include <string.h>
#include "SerialPort.h"
#include "AppSerialPort.h"
#include "uart.h"
#include "dma.h"
#include "custom.h"

/* --------------------------------------------------------------------------------------------------------------------------------------- */
/* Defines ------------------------------------------------------------------------------------------------------------------------------- */
/* ....................................................................................................................................... */

#define GET_SERIAL_PORT(__SERIAL__)			((__SERIAL__ < USE_SERIAL_PORT_COUNT) ? &H_SERIAL_PORTs[__SERIAL__] : H_SERIAL_PORTs)
#define GET_BAUDRATE(__BD__)				((__BD__ < BAUDRATE_COUNT) ? BDs[__BD__] : BDs[0] )

const uint32_t ONE_BYTE_MKSEC[BAUDRATE_COUNT] = {
	1100,	// BD_9600
	700,	// BD_14400
	600,	// BD_19200
	400,	// BD_28800
	300,	// BD_38400
	200,	// BD_57600
	150,	// BD_115200
	100,	// BD_128000
	100,	// BD_256000
	100,	// BD_375000
};

/* --------------------------------------------------------------------------------------------------------------------------------------- */
/* Private ------------------------------------------------------------------------------------------------------------------------------- */
/* ....................................................................................................................................... */

static const uint32_t BDs[BAUDRATE_COUNT] = { 9600, 14400, 19200, 28800, 38400, 57600, 115200, 128000, 256000, 375000 };

static uint32_t updata_bytes_to_read(HSerialPort serial);
static void transmite(HSerialPort serial, uint8_t* data, uint32_t size);

/* --------------------------------------------------------------------------------------------------------------------------------------- */
/* Public -------------------------------------------------------------------------------------------------------------------------------- */
/* ....................................................................................................................................... */

static void _config(SERIAL e, BAUDRATE bd, uint32_t frame_format);
static bool _write(SERIAL e, uint8_t* data, uint32_t size, bool copy_data);
static bool _is_write_compleited(SERIAL e);
static void _wait_write(SERIAL e);
static bool _IDLE_Detect(SERIAL e);
static void _wait_IDLE(SERIAL e);
static uint32_t _bytes_to_read(SERIAL e);
static uint8_t _read_byte(SERIAL e);
static void _read(SERIAL e, uint8_t* out, uint32_t size);
static uint8_t _bd(SERIAL e);
static void set_call_back(SERIAL e, void(*call_back)(void));

static TypeDef_SerialPort _serial =
{
	.config = _config,
	.write = _write,
	.is_write_compleited = _is_write_compleited,
	.wait_write = _wait_write,
	.IDLE_detect = _IDLE_Detect,
	.wait_IDLE = _wait_IDLE,
	.bytes_to_read = _bytes_to_read,
	.read_byte = _read_byte,
	.read = _read,
	.BD = _bd,
	.set_call_back = set_call_back,
};

const SerialPort_Handle SerialPort = &_serial;

/** *****************************************
 * Настройка последовательного порта
 * @param[serial]:	порт.
 * @param[baudrate]: баудрейт.
 * @return: результат операции.
 **/
static void _config(SERIAL e, BAUDRATE bd, uint32_t frame_format)
{
	HSerialPort serial = GET_SERIAL_PORT(e);
	uint32_t baudrate = GET_BAUDRATE(bd);

	serial->baudrate = bd;

	serial->rx.index_in = 0;    serial->rx.index_out = 0;   // rx buffer reset
	serial->tx.index_in = 0;    serial->tx.index_out = 0;   // tx buffer reset
	serial->is_transmit = true;
	serial->is_IDLE = false;

	__disable_irq();

	/*  - - - - - - - -  configuration UART  - - - - - - - - - - - - -
	*/
	xHAL_UART_Config uart_config = { 0 }; {
		uart_config.UARTx = serial->hardware.uart;
		uart_config.baudrate = baudrate;
		uart_config.mode = MODE_RX_DMA__TX_DMA;
		uart_config.frame_format = frame_format;
		uart_config.flow_control = serial->hardware.IO.de ? FLOW_CONTROL_RTS : FLOW_CONTROL_NO;
		{   /* iHandle */
			uart_config.handle.TXC_Transmission_Complete = serial->interrypr.TXC;
			uart_config.handle.IDLE_IdleLineDetected = serial->interrypr.IDLE; }
	}
	xHAL_UART_Init(&uart_config);

	/*  - - - - - - - -  configuration dma rx channel  - - - - - - - -
	*/
	xHAL_DMA_Config DMA_config_ch_rx = { 0 }; {
		DMA_config_ch_rx.mode = PERIPHERAL_TO_MEMORY_CIRCULAR;
		DMA_config_ch_rx.channel_priority = CHANNEL_PRIORITY_LOW;
		DMA_config_ch_rx.count_bytes = serial->rx.size;
		{   /* Memory */
			DMA_config_ch_rx.memory.addr = (uint32_t*)serial->rx.buffer;
			DMA_config_ch_rx.memory.size = SIZE_8_Bit;
			DMA_config_ch_rx.memory.inc = true; }
		{   /* Periph */
			DMA_config_ch_rx.periph.addr = (uint32_t*)&serial->hardware.uart->RDR;
			DMA_config_ch_rx.periph.size = SIZE_8_Bit;
			DMA_config_ch_rx.periph.inc = false; }
	}
	xHAL_DMA_Start(serial->hardware.DMA.ch_rx, &DMA_config_ch_rx);

	/* - - - - - - - - configuration GPIO  - - - - - - - - - - - - - -
	*/

	xHAL_IO_Config(serial->hardware.IO.rx, IO_MODE_AFx_OD__PULL_UP__SPEED_HI | IO_AF1);
	xHAL_IO_Config(serial->hardware.IO.tx, IO_MODE_AFx_PP__PULL_NO__SPEED_HI | IO_AF1);
	xHAL_IO_Config(serial->hardware.IO.de, IO_MODE_AFx_PP__PULL_NO__SPEED_HI | IO_AF1);

	__enable_irq();
}

/** *****************************************
 * Запись в порт.
 * @param[serial]:	порт.
 * @param[data]: 	указатель на данные.
 * @param[size]: 	размер данных.
 * @return: результат операции.
 **/
static bool _write(SERIAL e, uint8_t* data, uint32_t size, bool copy_data)
{
	HSerialPort serial = GET_SERIAL_PORT(e);
	bool result = false;

	if (_is_write_compleited(e))
	{
		serial->is_transmit = false;		

		if (copy_data)
		{
			uint32_t count = (size < serial->tx.size) ? size : serial->tx.size;

			memcpy(serial->tx.buffer, data, count);
			transmite(serial, serial->tx.buffer, count);
		}
		else
		{
			transmite(serial, data, size);
		}

		result = true;
	}

	return result;
}

/** *****************************************
 * Проверить завершилась ли последняя отправка.
 * @param[serial]:	порт.
 * @return: результат операции.
 **/
static bool _is_write_compleited(SERIAL e)
{
	HSerialPort serial = GET_SERIAL_PORT(e);

	return 
		serial->is_transmit ||
		(serial->hardware.uart->ISR & USART_ISR_TXE) ||
		(serial->hardware.uart->ISR & USART_ISR_TC);
}

static void _wait_write(SERIAL e)
{
	while (!_is_write_compleited(e))
	{
		xHAL_sleep();
	}
}

/** *****************************************
 * Проверить состояние входной линии.
 * @param[serial]: порт.
 * @return: true - тишина на линии.
 **/
static bool _IDLE_Detect(SERIAL e)
{
	HSerialPort serial = GET_SERIAL_PORT(e);

	bool idle = serial->is_IDLE;

	serial->is_IDLE = false;

	return idle;
}

/** *****************************************
 * Проверить состояние входной линии.
 * @param[serial]: порт.
 * @return: true - тишина на линии.
 **/
static void _wait_IDLE(SERIAL e)
{
	HSerialPort serial = GET_SERIAL_PORT(e);

	if (serial->is_IDLE)
	{
		serial->is_IDLE = false;
	}
	else
	{
		volatile uint32_t bytes = -1;

		while (bytes != serial->hardware.DMA.ch_rx->CNDTR)
		{
			bytes = serial->hardware.DMA.ch_rx->CNDTR;

			xHAL_Delay_mkSec(ONE_BYTE_MKSEC[serial->baudrate] / 100);
		}
	}
}

/** *****************************************
 * Количество байт в буффере приема.
 * @param[serial]:	порт.
 * @return: количество байт.
 **/
static uint32_t _bytes_to_read(SERIAL e)
{
	HSerialPort serial = GET_SERIAL_PORT(e);

	return updata_bytes_to_read(serial);
}

/** *****************************************
 * Прочитать один байт из буффера приема.
 * @param[serial]:	порт.
 * @return: принятый бакт.
 **/
static uint8_t _read_byte(SERIAL e)
{
	HSerialPort serial = GET_SERIAL_PORT(e);
	SP_RingBuffer* const rb = &serial->rx;
	uint32_t index = rb->index_out;

	/* increment index out */
	rb->index_out =
		((index + 1) < rb->size) ?
		index + 1 :
		0;

	return rb->buffer[index];
}

/** *****************************************
 * Прочитать заданное количество байт из буфера приема.
 * @param[serial]:	порт.
 * @param[out]: 	указатель пустой массив.
 * @param[size]: 	размер массива.
 **/
static void _read(SERIAL e, uint8_t* out_buffer, uint32_t size)
{
	HSerialPort serial = GET_SERIAL_PORT(e);
	SP_RingBuffer* const rb = &serial->rx;

	uint32_t count = updata_bytes_to_read(serial);
	count =
		(count < size) ?
		count :
		size;

	uint32_t index = rb->index_out;
	uint8_t* _out = out_buffer;

	while (count--)
	{
		*_out++ = rb->buffer[index++];
		index = (index < rb->size) ? index : 0;
	}

	rb->index_out = index;
}

static uint8_t _bd(SERIAL e)
{
	HSerialPort serial = GET_SERIAL_PORT(e);
	return serial->baudrate;
}

static void set_call_back(SERIAL e, void(*call_back)(void))
{
	HSerialPort serial = GET_SERIAL_PORT(e);
	serial->call_back = call_back;
}

/* --------------------------------------------------------------------------------------------------------------------------------------- */
/* Private Implementation ---------------------------------------------------------------------------------------------------------------- */
/* ....................................................................................................................................... */

//###################################################################################################################
static uint32_t updata_bytes_to_read(HSerialPort serial)
{
	serial->rx.index_in = serial->rx.size - serial->hardware.DMA.ch_rx->CNDTR;

	uint32_t OUT = serial->rx.index_out;
	uint32_t IN = serial->rx.index_in;

	/* return count bytes */
	return
		IN >= OUT ?
		IN - OUT :
		(serial->rx.size + IN) - OUT;
}

//###################################################################################################################
static void transmite(HSerialPort serial, uint8_t* data, uint32_t size)
{
	/*  - - - - - - - -  configuration dma rx channel  - - - - - - - -
	*/
	xHAL_DMA_Config DMA_config_ch_tx = { 0 }; {
		DMA_config_ch_tx.mode = MEMORY_TO_PERIPHERAL;
		DMA_config_ch_tx.channel_priority = CHANNEL_PRIORITY_LOW;
		DMA_config_ch_tx.count_bytes = size;
		{   /* Memory */
			DMA_config_ch_tx.memory.addr = (uint32_t*)data;
			DMA_config_ch_tx.memory.size = SIZE_8_Bit;
			DMA_config_ch_tx.memory.inc = true; }
		{   /* Periph */
			DMA_config_ch_tx.periph.addr = (uint32_t*)&serial->hardware.uart->TDR;
			DMA_config_ch_tx.periph.size = SIZE_8_Bit;
			DMA_config_ch_tx.periph.inc = false; }
	}

	xHAL_DMA_Start(serial->hardware.DMA.ch_tx, &DMA_config_ch_tx);
}

/* --------------------------------------------------------------------------------------------------------------------------------------- */
/* Interrupt ----------------------------------------------------------------------------------------------------------------------------- */
/* ....................................................................................................................................... */

//###################################################################################################################
void IDLE(HSerialPort serial)
{
	updata_bytes_to_read(serial);
	serial->is_IDLE = true;

	if (serial->call_back)
		serial->call_back();
}

//###################################################################################################################
void TXC(HSerialPort serial)
{
	serial->is_transmit = true;
}

/* End ----------------------------------------------------------------------------------------------------------------------------------- */
/* ....................................................................................................................................... */
