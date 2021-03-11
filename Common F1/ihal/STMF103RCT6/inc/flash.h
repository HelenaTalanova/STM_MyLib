// #########################################################################################################
// Define to prevent recursive inclusion _________________________________________________________________

#ifndef __FLASH_H
#define __FLASH_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif 

	bool iFLASH_unlock(void);
	bool iFLASH_erase(uint32_t addr);
	bool iFLASH_write(uint32_t addr, uint8_t* data, uint32_t size);

#ifdef __cplusplus
}
#endif

#endif // __FLASH_H
