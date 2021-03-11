// #########################################################################################################
// Define to prevent recursive inclusion _________________________________________________________________

#ifndef __IO_H
#define __IO_H

#include <stdint.h>
#include <stdbool.h>
#include "stm32f1xx.h"

#ifdef __cplusplus
extern "C" {
#endif 

	typedef struct IO_t
	{
		GPIO_TypeDef* port;
		uint16_t pin;
	}IO_t;

#define IO_NOT	(IO_t){ .port = 0, .pin = 0 }

	IO_t iHAL_IO_Create(GPIO_TypeDef* const port, uint16_t pin);

	void iHAL_IO_Config(IO_t const io, GPIO_InitTypeDef* const config);
	void iHAL_IO_DeConfig(IO_t const io);

	bool iHAL_IO_Read(IO_t const io);
	bool iHAL_IO_ReadInverse(IO_t const io);

	void iHAL_IO_Set(IO_t const io);
	void iHAL_IO_Reset(IO_t const io);

	void iHAL_IO_Write(IO_t const io, bool state);

	void iHAL_IO_Toogle(IO_t const io);

#ifdef __cplusplus
}
#endif

#endif // __IO_H
