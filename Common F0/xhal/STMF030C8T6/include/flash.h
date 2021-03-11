/* Define to prevent recursive inclusion -------------------------------------*/

#ifndef __FLASH_CONFIG_H
#define __FLASH_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "stm32f0xx.h"

#define FLASH_ADDR							0x08000000UL
#define FLASH_OFFSET						0x000002000UL
#define FLASH_PAGE_COUNT					64

#ifndef FLASH_PAGE_SIZE
#define FLASH_PAGE_SIZE						0x400UL
#endif // !FLASH_PAGE_SIZE

#define FLASH_SIZE							(FLASH_PAGE_SIZE * FLASH_PAGE_COUNT)
#define FLASH_ADDR_END						(FLASH_ADDR + FLASH_SIZE)
	
#define FLASH_READ_int8(__ADDR__)			*((__IO int8_t*)(__ADDR__))
#define FLASH_READ_int16(__ADDR__)			*((__IO int16_t*)(__ADDR__))
#define FLASH_READ_int32(__ADDR__)			*((__IO int32_t*)(__ADDR__))

#define FLASH_READ_uint8(__ADDR__)			*((__IO uint8_t*)(__ADDR__))
#define FLASH_READ_uint16(__ADDR__)			*((__IO uint16_t*)(__ADDR__))
#define FLASH_READ_uint32(__ADDR__)			*((__IO uint32_t*)(__ADDR__))

	typedef enum {
		X_FLASH_OB_RDP_LEVEL_0 = 0xAA,
		X_FLASH_OB_RDP_LEVEL_1 = 0xBB,
		X_FLASH_OB_RDP_LEVEL_2 = 0xCC,
	}X_FLASH_OB_RDP_LEVEL_e;

	void xHAL_FLASH_Erase(uint32_t address);
	void xHAL_FLASH_Write(uint8_t* data, uint32_t address, uint32_t count);
	void xHAL_FLASH_RDP_Write(X_FLASH_OB_RDP_LEVEL_e level);
	bool xHAL_FLASH_Unlock(void);
	void xHAL_FLASH_Lock(void);
	bool xHAL_FLASH_IsAddress(uint32_t address);
	
#ifdef __cplusplus
}
#endif

#endif // __FLASH_CONFIG_H
