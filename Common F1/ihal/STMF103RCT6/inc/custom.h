// #########################################################################################################
// Define to prevent recursive inclusion _________________________________________________________________

#ifndef __CUSTOM_H
#define __CUSTOM_H

#include <stdint.h>
#include <stdbool.h>
#include "stm32f1xx.h"
#include "assert.h"

#include "io.h"
#include "uart.h"
#include "dma.h"
#include "tim.h"
#include "flash.h"

#ifdef __cplusplus
extern "C" {
#endif 

#define IS_DELAY(TIMER, DELAY)		(IsDelay(&TIMER, DELAY))

	extern bool FIRST_START;

	void xHAL_software_reset(void);
	void xHAL_hardware_reset(void);
	bool IsDelay(uint32_t* timer, uint32_t delay);

#ifdef __cplusplus
}
#endif

#endif // __CUSTOM_H
