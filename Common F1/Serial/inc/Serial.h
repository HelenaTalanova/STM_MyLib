// #########################################################################################################
// Define to prevent recursive inclusion _________________________________________________________________

#ifndef __SERIAL_H
#define __SERIAL_H

#include <stdint.h>
#include <stdbool.h>
#include "Serial_config.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus	

	typedef enum SERIAL {
		SERIAL_NO = __SERIAL_NO_USED,
		SERIAL_1 = (__USED_SERIAL_1),
		SERIAL_2 = (__USED_SERIAL_1 + __USED_SERIAL_2) * __USED_SERIAL_2,
		SERIAL_3 = (__USED_SERIAL_1 + __USED_SERIAL_2 + __USED_SERIAL_3) * __USED_SERIAL_3,
		SERIAL_4 = (__USED_SERIAL_1 + __USED_SERIAL_2 + __USED_SERIAL_3 + __USED_SERIAL_4) * __USED_SERIAL_4,
		SERIAL_5 = (__USED_SERIAL_1 + __USED_SERIAL_2 + __USED_SERIAL_3 + __USED_SERIAL_4 + __USED_SERIAL_5) * __USED_SERIAL_5,
		SERIAL_COUNT = (__USED_SERIAL_1 + __USED_SERIAL_2 + __USED_SERIAL_3 + __USED_SERIAL_4 + __USED_SERIAL_5),
	}SERIAL;

	typedef enum FRAME_FORMAT {
		SERIAL_WORD_LEN_8 = 0x00,
		SERIAL_WORD_LEN_9 = 0x01,
		SERIAL_PARITY_NO = 0x00,
		SERIAL_PARITY_EVEN = 0x02,
		SERIAL_PARITY_ODD = 0x04,
		SERIAL_STOP_0_5 = 0x08,
		SERIAL_STOP_1 = 0x00,
		SERIAL_STOP_1_5 = 0x10,
		SERIAL_STOP_2 = 0x20,
	}FRAME_FORMAT;

	typedef void(*callback)(SERIAL e);

	typedef struct SERIAL_APP {
		/// configure serial port
		bool (*config)(SERIAL e, uint32_t baudrate, FRAME_FORMAT frame_format, callback IDLE_callback);
		/// write message, copy data to internal buffer
		bool (*write_to_buff)(SERIAL e, uint8_t* data, uint32_t size);
		/// write message, no copy data to internal buffer
		bool (*write_to_stream)(SERIAL e, uint8_t* data, uint32_t size);
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
	} SERIAL_APP;

	extern const SERIAL_APP Serial;

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __SERIAL_H
