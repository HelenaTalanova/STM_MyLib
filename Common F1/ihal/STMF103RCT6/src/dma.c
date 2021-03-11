// #########################################################################################################
// * Includes ____________________________________________________________________________________________

#include "custom.h"

// #########################################################################################################
// * Defines config ______________________________________________________________________________________

// uncomment for using
#define USED_DMA1_CH1
#define USED_DMA1_CH2
#define USED_DMA1_CH3
#define USED_DMA1_CH4
#define USED_DMA1_CH5
#define USED_DMA1_CH6
#define USED_DMA1_CH7

#define USED_DMA2_CH1
#define USED_DMA2_CH2
#define USED_DMA2_CH3
#define USED_DMA2_CH4_5

// #########################################################################################################
// * Defines _____________________________________________________________________________________________

#define COUNT_DMA										2U
#define COUNT_CHANNEL_DMA								7U

#define __Enable_Channel(CHx)							do { CHx->CCR |= DMA_CCR_EN; __ASM("nop"); } while(0U);
#define __Disable_Channel(CHx)							do { CHx->CCR &= ~(DMA_CCR_EN); __ASM("nop"); } while(0U);

#define __Is_Interrupt_Transfer_Error(ISR_CH)			((ISR & DMA_ISR_TEIF1) != RESET)
#define __Is_Interrupt_Half_Transfer(ISR_CH)			((ISR & DMA_ISR_HTIF1) != RESET)
#define __Is_Interrupt_Transfer_Complete(ISR_CH)		((ISR & DMA_ISR_TCIF1) != RESET)

#define __Clear_Interrypt_Msk							(DMA_IFCR_CTCIF1 | DMA_IFCR_CHTIF1 | DMA_IFCR_CTCIF1 | DMA_IFCR_CGIF1)
#define __Clear_Interrypt(DMAx, CHn)					DMAx->IFCR |= __Clear_Interrypt_Msk << (CHn << 2)

// #########################################################################################################
// * Private _____________________________________________________________________________________________

static iHAL_DMA_INTERRYPTS handle_interrypts[COUNT_DMA][COUNT_CHANNEL_DMA] __attribute__((unused)) = { 0 };

static void enable_clock(DMA_TypeDef* DMAx);
static DMA_TypeDef* get_DMA(DMA_Channel_TypeDef* ch);
static uint8_t get_num_ch(DMA_Channel_TypeDef* ch);
static void enable_IRQ(DMA_Channel_TypeDef* ch, iHAL_DMA_INTERRYPTS* it);
static IRQn_Type get_IRQn(DMA_TypeDef* DMAx, uint32_t ch);
static void iterrypt_exe(DMA_Channel_TypeDef* ch, iHAL_DMA_INTERRYPTS* it) __attribute__((unused));

// #########################################################################################################
// * Public: implementation ______________________________________________________________________________

/// **************************************************************************************
void iHAL_DMA_Start(DMA_Channel_TypeDef* ch, DMA_Config* config, uint16_t count_data)
{
	assert_null(ch);
	assert_null(config);

	iHAL_DMA_INTERRYPTS* IT = &config->hardware.handle;
	DMA_TypeDef* DMAx = get_DMA(ch);
	uint32_t tmp = 0UL;

	enable_clock(DMAx);

	__Disable_Channel(ch);

	ch->CNDTR = count_data;													// DMA channel x number of data register		
	ch->CMAR = (uint32_t)config->memory.addr;								// DMA channel x memory address register				
	ch->CPAR = (uint32_t)config->periphery.addr;								// DMA channel x peripheral address register

	// DMA channel x configuration register  ----------------------------------------------------------------------------------				
	if (IT)
	{
		tmp |= IT->Error ? DMA_CCR_TEIE : 0UL; 								// Transfer error interrupt enable
		tmp |= IT->Half_Transfer ? DMA_CCR_HTIE : 0UL;  					// Half Transfer interrupt enable
		tmp |= IT->Transfer_Complete ? DMA_CCR_TCIE : 0UL;					// Transfer complete interrupt enable		
	}
	tmp |= config->hardware.mode & DMA_CCR_MEM2MEM;							// Memory to memory mode
	tmp |= config->hardware.mode & DMA_CCR_DIR;								// Data transfer direction
	tmp |= config->hardware.mode & DMA_CCR_CIRC; 							// Circular mode			
	tmp |= config->hardware.priory << DMA_CCR_PL_Pos; 						// PL[1:0] bits(Channel Priority level)	
	tmp |= config->memory.size << DMA_CCR_MSIZE_Pos; 						// MSIZE[1:0] bits (Memory size)
	tmp |= config->memory.addr_inc ? DMA_CCR_MINC : 0UL; 					// Memory increment mode	
	tmp |= config->periphery.size << DMA_CCR_PSIZE_Pos; 						// PSIZE[1:0] bits (Peripheral size)
	tmp |= config->periphery.addr_inc ? DMA_CCR_PINC : 0UL; 					// Peripheral increment mode
	tmp = tmp & (~DMA_CCR_EN);												// Channel enable
	ch->CCR = tmp;

	enable_IRQ(ch, IT);
	__Enable_Channel(ch);
}

/// **************************************************************************************
void iHAL_DMA_DeInit(DMA_Channel_TypeDef* ch)
{
	assert_null(ch);

	__Disable_Channel(ch);

	uint32_t num = get_num_ch(ch);
	DMA_TypeDef* DMAx = get_DMA(ch);

	__Clear_Interrypt(DMAx, num);

	IRQn_Type IRQn = get_IRQn(DMAx, num);

	if (IRQn)
		NVIC_DisableIRQ(IRQn);
}

// #########################################################################################################
// * Private: Implementation _____________________________________________________________________________

/// **************************************************************************************
static void enable_clock(DMA_TypeDef* DMAx)
{
	assert_null(DMAx);

	if (DMAx == DMA1)
	{
		RCC->AHBENR |= RCC_AHBENR_DMA1EN;
	}
	else if (DMAx == DMA2)
	{
		RCC->AHBENR |= RCC_AHBENR_DMA2EN;
	}
}

/// **************************************************************************************
static DMA_TypeDef* get_DMA(DMA_Channel_TypeDef* ch)
{
	assert_null(ch);

	return ch < DMA2_Channel1 ? DMA1 : DMA2;
}

/// **************************************************************************************
static uint8_t get_num_ch(DMA_Channel_TypeDef* ch)
{
	assert_null(ch);

	uint32_t num_ch = ((uint32_t)ch & 0xFF) + 0x0C;

	return (num_ch / 0x14UL) - 1;
}

/// **************************************************************************************
static void enable_IRQ(DMA_Channel_TypeDef* ch, iHAL_DMA_INTERRYPTS* it)
{
	assert_null(ch);
	assert_null(it);

	if ((it == 0) ? false : (
		it->Error ||
		it->Half_Transfer ||
		it->Transfer_Complete))
	{
		DMA_TypeDef* DMAx = get_DMA(ch);
		uint8_t NumCh = get_num_ch(ch);

		IRQn_Type IRQn = get_IRQn(DMAx, NumCh);

		if (IRQn)
		{
			handle_interrypts[DMAx == DMA1 ? 0 : 1][NumCh].Error = it->Error;
			handle_interrypts[DMAx == DMA1 ? 0 : 1][NumCh].Half_Transfer = it->Half_Transfer;
			handle_interrypts[DMAx == DMA1 ? 0 : 1][NumCh].Transfer_Complete = it->Transfer_Complete;
			NVIC_SetPriority(IRQn, 0);
			NVIC_EnableIRQ(IRQn);
		}
	}
}

/// **************************************************************************************
static IRQn_Type get_IRQn(DMA_TypeDef* DMAx, uint32_t num)
{
	assert_null(DMAx);

	num++;
	if (DMAx == DMA1)
		switch (num)
		{
		case 1: { return DMA1_Channel1_IRQn; }
		case 2: { return DMA1_Channel2_IRQn; }
		case 3: { return DMA1_Channel3_IRQn; }
		case 4: { return DMA1_Channel4_IRQn; }
		case 5: { return DMA1_Channel5_IRQn; }
		case 6: { return DMA1_Channel6_IRQn; }
		case 7: { return DMA1_Channel7_IRQn; }
		}
	if (DMAx == DMA2)
		switch (num)
		{
		case 1: { return DMA2_Channel1_IRQn; }
		case 2: { return DMA2_Channel2_IRQn; }
		case 3: { return DMA2_Channel3_IRQn; }
		case 4: { return DMA2_Channel4_5_IRQn; }
		case 5: { return DMA2_Channel4_5_IRQn; }
		}

	return (IRQn_Type)0UL;
}

/// **************************************************************************************
static void iterrypt_exe(DMA_Channel_TypeDef* ch, iHAL_DMA_INTERRYPTS* it)
{
	assert_null(ch);
	assert_null(it);

	DMA_TypeDef* DMAx = get_DMA(ch);
	uint32_t NumCh = get_num_ch(ch);
	uint32_t ISR = (DMAx->ISR >> (NumCh << 2)) & 0x0FU;

	if (__Is_Interrupt_Transfer_Error(ISR) ? it->Error : false)
		it->Error();

	if (__Is_Interrupt_Half_Transfer(ISR) ? it->Half_Transfer : false)
		it->Half_Transfer();

	if (__Is_Interrupt_Transfer_Complete(ISR) ? it->Transfer_Complete : false)
		it->Transfer_Complete();

	__Clear_Interrypt(DMAx, NumCh);
}

// #########################################################################################################
// * Interrupt ___________________________________________________________________________________________

/// DMA1 - Interrupt Channels 1 - 7

#ifdef USED_DMA1_CH1
/// **************************************************************************************
void DMA1_Channel1_IRQHandler(void)
{
	iterrypt_exe(DMA1_Channel1, &handle_interrypts[0][0]);
}
#endif // USED_DMA1_CH1

#ifdef USED_DMA1_CH2
/// **************************************************************************************
void DMA1_Channel2_IRQHandler(void)
{
	iterrypt_exe(DMA1_Channel2, &handle_interrypts[0][1]);
}
#endif // USED_DMA1_CH2

#ifdef USED_DMA1_CH3
/// **************************************************************************************
void DMA1_Channel3_IRQHandler(void)
{
	iterrypt_exe(DMA1_Channel3, &handle_interrypts[0][2]);
}
#endif // USED_DMA1_CH3

#ifdef USED_DMA1_CH4
/// **************************************************************************************
void DMA1_Channel4_IRQHandler(void)
{
	iterrypt_exe(DMA1_Channel4, &handle_interrypts[0][3]);
}
#endif // USED_DMA1_CH4

#ifdef USED_DMA1_CH5
/// **************************************************************************************
void DMA1_Channel5_IRQHandler(void)
{
	iterrypt_exe(DMA1_Channel5, &handle_interrypts[0][4]);
}
#endif // USED_DMA1_CH5

#ifdef USED_DMA1_CH6
/// **************************************************************************************
void DMA1_Channel6_IRQHandler(void)
{
	iterrypt_exe(DMA1_Channel6, &handle_interrypts[0][5]);
}
#endif // USED_DMA1_CH6

#ifdef USED_DMA1_CH7
/// **************************************************************************************
void DMA1_Channel7_IRQHandler(void)
{
	iterrypt_exe(DMA1_Channel7, &handle_interrypts[0][6]);
}
#endif // USED_DMA1_CH7

///* DMA2 - Interrupt Channels 1 - 5 */

#ifdef USED_DMA2_CH1
/// **************************************************************************************
void DMA2_Channel1_IRQHandler(void)
{
	iterrypt_exe(DMA2_Channel1, &handle_interrypts[1][0]);
}
#endif // USED_DMA2_CH1

#ifdef USED_DMA2_CH2
/// **************************************************************************************
void DMA2_Channel2_IRQHandler(void)
{
	iterrypt_exe(DMA2_Channel2, &handle_interrypts[1][1]);
}
#endif // USED_DMA2_CH2

#ifdef USED_DMA2_CH3
/// **************************************************************************************
void DMA2_Channel3_IRQHandler(void)
{
	iterrypt_exe(DMA2_Channel3, &handle_interrypts[1][2]);
}
#endif // USED_DMA2_CH3

#ifdef USED_DMA2_CH4_5
/// **************************************************************************************
void DMA2_Channel4_5_IRQHandler(void)
{
	iterrypt_exe(DMA2_Channel4, &handle_interrypts[1][3]);
	iterrypt_exe(DMA2_Channel5, &handle_interrypts[1][4]);
}
#endif // USED_DMA2_CH4_5

/// #########################################################################################################
/// * End file dma.c                                                                                 #####
/// #####################################################################################################
