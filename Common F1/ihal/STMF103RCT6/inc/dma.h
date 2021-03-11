// #########################################################################################################
// Define to prevent recursive inclusion _________________________________________________________________

#ifndef __DMA_H
#define __DMA_H

#include <stdint.h>
#include <stdbool.h>
#include "stm32f1xx.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

	// DMA MODE ______________________________________________________________________________________________
	typedef enum DMA_MODE {
		PERIPH_TO_MEM_ONE = 0UL,
		PERIPH_TO_MEM_CIR = DMA_CCR_CIRC,
		MEM_TO_PERIPH_ONE = DMA_CCR_DIR,
		MEM_TO_PERIPH_CIR = DMA_CCR_DIR | DMA_CCR_CIRC,
		MEM_TO_MEM_ONE = DMA_CCR_MEM2MEM,
		MEM_TO_MEM_CIR = DMA_CCR_MEM2MEM | DMA_CCR_CIRC,
	}iHAL_DMA_MODE;

	// DATA SIZE _____________________________________________________________________________________________
	typedef enum DMA_DATA_SIZE {
		SIZE_8_Bit = 0UL,
		SIZE_16_Bit = 1UL,
		SIZE_32_Bit = 2UL,
	}iHAL_DMA_DATA_SIZE;

	// CHANNEL PRIORY ________________________________________________________________________________________
	typedef enum DMA_CH_PRIORY {
		CH_PRIORY_LOW = 0UL,
		CH_PRIORY_MEDIUM = 1UL,
		CH_PRIORY_HIGH = 2UL,
		CH_PRIORY_VERY_HIGH = 3UL,
	}iHAL_DMA_CH_PRIORY;

	// DMA INTERRYPTS ________________________________________________________________________________________
	typedef void(*iHAL_DMA_HANDLE)(void);
	typedef struct DMA_INTERRYPTS {
		iHAL_DMA_HANDLE Error;
		iHAL_DMA_HANDLE Half_Transfer;
		iHAL_DMA_HANDLE Transfer_Complete;
	}iHAL_DMA_INTERRYPTS;

	// DMA CONFIG ____________________________________________________________________________________________
	typedef struct DMA_Config {
		struct memory {
			uint32_t* addr;
			iHAL_DMA_DATA_SIZE size;
			bool addr_inc;
		}memory;
		struct periphery {
			uint32_t* addr;
			iHAL_DMA_DATA_SIZE size;
			bool addr_inc;
		}periphery;
		struct hardware {
			iHAL_DMA_MODE mode;
			iHAL_DMA_CH_PRIORY priory;
			iHAL_DMA_INTERRYPTS handle;
		}hardware;
	}DMA_Config;

	// DMA ___________________________________________________________________________________________________

	void iHAL_DMA_Start(DMA_Channel_TypeDef* ch, DMA_Config* config, uint16_t count_data);
	void iHAL_DMA_DeInit(DMA_Channel_TypeDef* ch);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __DMA_H
