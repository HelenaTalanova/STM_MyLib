/* Define to prevent recursive inclusion -------------------------------------*/

#ifndef __CUSTOM_H
#define __CUSTOM_H

#include <stdint.h>
#include <stdbool.h>
#include "stm32f0xx.h"

/* use modules */
#include "dma.h"
#include "flash.h"
#include "io.h"
#include "tim.h"
#include "uart.h"

#ifdef __cplusplus
extern "C" {
#endif

#define __NOT	!
#define RESET_TIMEOUT(counter)		(counter = 0UL)	
#define USE_FULL_ASSERT 1 

	/// clock
	extern uint32_t SystemCoreClock;
	extern const uint8_t APBPrescTable[];
	extern const uint8_t AHBPrescTable[];

	uint32_t RCC_PCLK1_freq_get(void);
	// HAL
	uint32_t HAL_GetTick(void);
	void HAL_Delay(uint32_t Delay);
	// xHAL mSec
	uint32_t xHAL_Get_mSec(void);
	void xHAL_Delay_mSec(uint32_t Delay);
	bool xHAL_is_time_has_passed_mSec(uint32_t* START, uint32_t DELAY);
	// xHAL mkSec
	uint32_t xHAL_Get_mkSec(void);
	void xHAL_Delay_mkSec(uint32_t Delay);
	bool xHAL_is_time_has_passed_mkSec(uint32_t* START, uint32_t DELAY);

	/// iwdg
	void xHAL_IWDG_Start(void);
	void xHAL_IWDG_Refresh(void);

	/// MainApp
	void xHAL_sleep(void);
	void _Error_Handler(char*, int);
	void _Error_Handler_Message(char* file, int line, const char* message);

	void xHAL_hardware_reset(void);
	void xHAL_software_reset(void);

#define Error_Handler() \
		_Error_Handler(__FILE__, __LINE__)

#define Error_Handler_Message(__MESSAGE__) \
		_Error_Handler_Message(__FILE__, __LINE__, __MESSAGE__)

#ifdef __cplusplus
}
#endif

#endif // __CUSTOM_H
