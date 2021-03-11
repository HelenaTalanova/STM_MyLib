/* Define to prevent recursive inclusion -------------------------------------*/

#ifndef __SERIAL_H
#define __SERIAL_H

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

#include <stdint.h>
#include <stdbool.h>	
#include "custom.h"

	typedef enum {
		BD_9600,
		BD_14400,
		BD_19200,
		BD_28800,
		BD_38400,
		BD_57600,
		BD_115200,
		BD_128000,
		BD_256000,
		BD_375000,
		BAUDRATE_COUNT,	
	}BAUDRATE;

	typedef enum {
		SERIAL_PORT_1,
		SERIAL_PORT_2,
	}SERIAL;

	typedef struct TypeDef_SerialPort
	{
		/// configure serial port
		void (*config)(SERIAL e, BAUDRATE bd, uint32_t frame_format);
		/// write message, copy or no copy data to internal buffer
		bool (*write)(SERIAL e, uint8_t* data, uint32_t size, bool copy_data);
		/// true is write compleited
		bool (*is_write_compleited)(SERIAL e);
		/// wait write
		void (*wait_write)(SERIAL e);
		/// is detect IDLE
		bool (*IDLE_detect)(SERIAL e);
		/// wait IDLE
		void (*wait_IDLE)(SERIAL e);
		/// count bytes received
		uint32_t(*bytes_to_read)(SERIAL e);
		/// read one byte
		uint8_t(*read_byte)(SERIAL e);
		/// read bytes
		void (*read)(SERIAL e, uint8_t* out_buffer, uint32_t size);
		/// baudrate
		uint8_t(*BD)(SERIAL e);
		/// callback
		void(*set_call_back)(SERIAL e, void(*call_back)(void));
	} TypeDef_SerialPort;

	typedef const TypeDef_SerialPort* SerialPort_Handle;

	extern const SerialPort_Handle SerialPort;

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __SERIAL_H
