/* Define to prevent recursive inclusion -------------------------------------*/

#ifndef __DMA_CONFIG_H
#define __DMA_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif
	
#include <stdint.h>
#include <stdbool.h>
#include "stm32f0xx.h"

	typedef enum
	{
		PERIPHERAL_TO_MEMORY          = 0UL,
		PERIPHERAL_TO_MEMORY_CIRCULAR = DMA_CCR_CIRC,
		MEMORY_TO_PERIPHERAL          = DMA_CCR_DIR,
		MEMORY_TO_PERIPHERAL_CIRCULAR = DMA_CCR_DIR | DMA_CCR_CIRC,
		MEMORY_TO_MEMORY              = DMA_CCR_MEM2MEM,
		MEMORY_TO_MEMORY_CIRCULAR     = DMA_CCR_MEM2MEM | DMA_CCR_CIRC,
	}xHAL_DMA_MODE;

	typedef enum
	{
		SIZE_8_Bit  = 0UL,
		SIZE_16_Bit = 1UL,
		SIZE_32_Bit = 2UL,
	}xHAL_DMA_DATA_SIZE;

	typedef enum
	{
		CHANNEL_PRIORITY_LOW       = 0UL,
		CHANNEL_PRIORITY_MEDIUM    = 1UL,
		CHANNEL_PRIORITY_HIGH      = 2UL,
		CHANNEL_PRIORITY_VERY_HIGH = 3UL,
	}xHAL_DMA_CH_PRIORY;


	typedef void(*xHAL_DMA_HANDLE_f)(void);

	typedef struct
	{
		xHAL_DMA_HANDLE_f Error;
		xHAL_DMA_HANDLE_f Half_Transfer;
		xHAL_DMA_HANDLE_f Transfer_Complete;
	}xHAL_DMA_Interrupts;

	typedef struct
	{
		xHAL_DMA_MODE		mode;
		xHAL_DMA_CH_PRIORY	channel_priority;
		uint16_t			count_bytes;
		xHAL_DMA_Interrupts	handle;
		struct
		{
			uint32_t*			addr;
			xHAL_DMA_DATA_SIZE	size;
			bool				inc;
		}memory;
		struct
		{
			uint32_t*			addr;
			xHAL_DMA_DATA_SIZE	size;
			bool				inc;
		}periph;
	}xHAL_DMA_Config;

	void xHAL_DMA_Start(DMA_Channel_TypeDef* ch, xHAL_DMA_Config* config);
	void xHAL_DMA_DeInit(DMA_Channel_TypeDef* ch);
	
#ifdef __cplusplus
}
#endif

#endif // __DMA_CONFIG_H
