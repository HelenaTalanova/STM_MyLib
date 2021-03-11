/* --------------------------------------------------------------------------------------------------------------------------------------- */
/* Includes ------------------------------------------------------------------------------------------------------------------------------ */
/* ....................................................................................................................................... */

#include "dma.h"

/* --------------------------------------------------------------------------------------------------------------------------------------- */
/* Defines ------------------------------------------------------------------------------------------------------------------------------- */
/* ....................................................................................................................................... */

#define COUNT_DMA										1U
#define COUNT_CHANNEL_DMA								5U

#define __Enable_CLK()									do { (RCC->AHBENR |= RCC_AHBENR_DMA1EN); __ASM("nop"); } while(0U);
#define __Disable_CLK()									do { (RCC->AHBENR &= ~(RCC_AHBENR_DMA1EN)); __ASM("nop"); } while(0U);
#define __Enable_Channel(CHx)							do { CHx->CCR |= DMA_CCR_EN; __ASM("nop"); } while(0U);
#define __Disable_Channel(CHx)							do { CHx->CCR &= ~(DMA_CCR_EN); __ASM("nop"); } while(0U);
#define __Channel_To_Number(CHx)						(((((uint32_t)ch & 0xFFUL) + 0x0CUL) / 0x14UL) - 1)

#define __Is_Interrupt_Transfer_Error(ISR_CH)			((ISR & DMA_ISR_TEIF1) != 0UL)
#define __Is_Interrupt_Half_Transfer(ISR_CH)			((ISR & DMA_ISR_HTIF1) != 0UL)
#define __Is_Interrupt_Transfer_Complete(ISR_CH)		((ISR & DMA_ISR_TCIF1) != 0UL)

#define __Clear_Interrypt_Msk							(DMA_IFCR_CTCIF1 | DMA_IFCR_CHTIF1 | DMA_IFCR_CTCIF1 | DMA_IFCR_CGIF1)
#define __Clear_Interrypt(CHn)							(DMA1->IFCR |= __Clear_Interrypt_Msk << (CHn << 2))

/* --------------------------------------------------------------------------------------------------------------------------------------- */
/* Private ------------------------------------------------------------------------------------------------------------------------------- */
/* ....................................................................................................................................... */

static xHAL_DMA_Interrupts handles[COUNT_CHANNEL_DMA];

static void IRQ_enable(DMA_Channel_TypeDef* ch, xHAL_DMA_Interrupts* iHandle);
static IRQn_Type get_IRQn(uint32_t ch);
static void iterrypt(DMA_Channel_TypeDef * ch, xHAL_DMA_Interrupts* iHandle);

/* --------------------------------------------------------------------------------------------------------------------------------------- */
/* Public Implementation ----------------------------------------------------------------------------------------------------------------- */
/* ....................................................................................................................................... */

//###################################################################################################################
void xHAL_DMA_Start(DMA_Channel_TypeDef* ch, xHAL_DMA_Config* config)
{		
	xHAL_DMA_Interrupts* IR = &config->handle;
		
	__Enable_CLK();
			
	__Disable_Channel(ch);
			
	ch->CNDTR = config->count_bytes; 										// DMA channel x number of data register		
	ch->CMAR = (uint32_t)config->memory.addr; 								// DMA channel x memory address register				
	ch->CPAR = (uint32_t)config->periph.addr; 								// DMA channel x peripheral address register

	// DMA channel x configuration register  ----------------------------------------------------------------------------------	
	uint32_t tmp = 0UL;			
	if (IR)
	{
		tmp |= IR->Error ? DMA_CCR_TEIE : 0UL;  							// Transfer error interrupt enable
		tmp |= IR->Half_Transfer ? DMA_CCR_HTIE : 0UL;   					// Half Transfer interrupt enable
		tmp |= IR->Transfer_Complete ? DMA_CCR_TCIE : 0UL; 					// Transfer complete interrupt enable		
	}					
	tmp |= config->mode & DMA_CCR_MEM2MEM;  								// Memory to memory mode
	tmp |= config->mode & DMA_CCR_DIR;  									// Data transfer direction
	tmp |= config->mode & DMA_CCR_CIRC;  									// Circular mode			
	tmp |= (config->channel_priority << DMA_CCR_PL_Pos) & DMA_CCR_PL_Msk;	// PL[1:0] bits(Channel Priority level)	
	tmp |= (config->memory.size << DMA_CCR_MSIZE_Pos) & DMA_CCR_MSIZE_Msk;	// MSIZE[1:0] bits (Memory size)
	tmp |= config->memory.inc ? DMA_CCR_MINC : 0UL;  						// Memory increment mode	
	tmp |= (config->periph.size << DMA_CCR_PSIZE_Pos) & DMA_CCR_PSIZE_Msk; 	// PSIZE[1:0] bits (Peripheral size)
	tmp |= config->periph.inc ? DMA_CCR_PINC : 0UL;  						// Peripheral increment mode
	tmp = tmp & (~DMA_CCR_EN); 												// Channel enable
	ch->CCR = tmp;
		
	IRQ_enable(ch, IR);	
	__Enable_Channel(ch);	
}

//###################################################################################################################
void xHAL_DMA_DeInit(DMA_Channel_TypeDef* ch)
{		
	uint32_t NumCh = __Channel_To_Number(ch);		

	__Disable_Channel(ch);	
	__Clear_Interrypt(NumCh);		
	
	IRQn_Type IRQn = get_IRQn(NumCh);
	if (IRQn)						
		NVIC_DisableIRQ(IRQn);		
}

/* --------------------------------------------------------------------------------------------------------------------------------------- */
/* Private Implementation ---------------------------------------------------------------------------------------------------------------- */
/* ....................................................................................................................................... */

//###################################################################################################################
static void IRQ_enable(DMA_Channel_TypeDef* ch, xHAL_DMA_Interrupts* iHandle)
{
	if (iHandle)
		if (iHandle->Error || iHandle->Half_Transfer || iHandle->Transfer_Complete)
		{
			uint8_t NumCh = __Channel_To_Number(ch);

			IRQn_Type IRQn = get_IRQn(NumCh);

			if (IRQn)
			{
				handles[NumCh].Error = iHandle->Error;
				handles[NumCh].Half_Transfer = iHandle->Half_Transfer;
				handles[NumCh].Transfer_Complete = iHandle->Transfer_Complete;
				NVIC_SetPriority(IRQn, 0);
				NVIC_EnableIRQ(IRQn);
			}
		}
}

//###################################################################################################################
static IRQn_Type get_IRQn(uint32_t NumCh)
{			
	switch (NumCh)
	{
	case 0: { return DMA1_Channel1_IRQn; }
	case 1: { return DMA1_Channel2_3_IRQn; }
	case 2: { return DMA1_Channel2_3_IRQn; }
	case 3: { return DMA1_Channel4_5_IRQn; }
	case 4: { return DMA1_Channel4_5_IRQn; }
	default: return 0UL;
	}
}

//###################################################################################################################
static void iterrypt(DMA_Channel_TypeDef* ch, xHAL_DMA_Interrupts* iHandle)
{
	uint32_t NumCh = __Channel_To_Number(ch);
	uint32_t ISR = (DMA1->ISR >> (NumCh << 2)) & 0x0FU;

	if (__Is_Interrupt_Transfer_Error(ISR) && iHandle->Error)
		iHandle->Error();

	if (__Is_Interrupt_Half_Transfer(ISR) && iHandle->Half_Transfer)
		iHandle->Half_Transfer();

	if (__Is_Interrupt_Transfer_Complete(ISR) && iHandle->Transfer_Complete)
		iHandle->Transfer_Complete();

	__Clear_Interrypt(NumCh);
}

/* --------------------------------------------------------------------------------------------------------------------------------------- */
/* Interrupt ----------------------------------------------------------------------------------------------------------------------------- */
/* ....................................................................................................................................... */

/* DMA1 - Interrupt Channels 1 - 5 */

void DMA1_Channel1_IRQHandler(void) 
{
	iterrypt(DMA1_Channel1, &handles[0]); 
}

void DMA1_Channel2_3_IRQHandler(void) 
{ 
	iterrypt(DMA1_Channel2, &handles[1]);
	iterrypt(DMA1_Channel2, &handles[2]);
}

void DMA1_Channel4_5_IRQHandler(void) 
{ 
	iterrypt(DMA1_Channel4, &handles[3]); 
	iterrypt(DMA1_Channel4, &handles[4]); 
}

/* End ----------------------------------------------------------------------------------------------------------------------------------- */
/* ....................................................................................................................................... */
