/* --------------------------------------------------------------------------------------------------------------------------------------- */
/* Includes ------------------------------------------------------------------------------------------------------------------------------ */
/* ....................................................................................................................................... */

#include "custom.h"
#include "uart.h"

/* --------------------------------------------------------------------------------------------------------------------------------------- */
/* Defines ------------------------------------------------------------------------------------------------------------------------------- */
/* ....................................................................................................................................... */

#define COUNT_USART			 						2U

#define __Enable(UARTx)								(UARTx->CR1 |= USART_CR1_UE)
#define __Disable(UARTx)							(UARTx->CR1 &= ~USART_CR1_UE)

/* Check status register */
#define __Is_ERR(UARTx)								((UARTx->ISR & USART_ISR_PE | USART_ISR_FE | USART_ISR_NE | USART_ISR_ORE) != RESET)
#define __Is_ERR_Parity(UARTx)						((UARTx->ISR & USART_ISR_PE) != RESET)
#define __Is_IDLE(UARTx)							((UARTx->ISR & USART_ISR_IDLE) != RESET)
#define __Is_TX_Complete(UARTx)						((UARTx->ISR & USART_ISR_TC) != RESET)
#define __Is_TX_Empty(UARTx)						((UARTx->ISR & USART_ISR_TXE) != RESET)
#define __Is_RX_Not_Empty(UARTx)					((UARTx->ISR & USART_ISR_RXNE) != RESET)

/* Clear Status register */
#define __Clear_Error(UARTx)						(UARTx->ICR |= (USART_ICR_PECF | USART_ICR_FECF | USART_ICR_NCF | USART_ICR_ORECF))
#define __Clear_Interrypt_ERR_Parity(UARTx)			(UARTx->ICR |= USART_ICR_PECF)
#define __Clear_Interrypt_ERR_Framing(UARTx)		(UARTx->ICR |= USART_ICR_FECF)
#define __Clear_Interrypt_ERR_Noise(UARTx)			(UARTx->ICR |= USART_ICR_NCF)
#define __Clear_Interrypt_ERR_OverRun(UARTx)		(UARTx->ICR |= USART_ICR_ORECF)
#define __Clear_Interrypt_IDLE(UARTx)				(UARTx->ICR |= USART_ICR_IDLECF)
#define __Clear_Interrypt_TX_Complete(UARTx)		(UARTx->ICR |= USART_ICR_TCCF)

#define __UART_DIV_SAMPLING16(PCLK, BAUD)			((PCLK + (BAUD / 2U)) / BAUD)
#define __UART_InterryptFirst(UARTx)				(UARTx == USART1 ? 0x0F : 0xF0)

/* --------------------------------------------------------------------------------------------------------------------------------------- */
/* Private ------------------------------------------------------------------------------------------------------------------------------- */
/* ....................................................................................................................................... */

static xHAL_UART_INTERRYPTS handles[COUNT_USART];
static uint8_t IsInterryptFirst = 0x00;

static void CLK_enable(USART_TypeDef* UARTx);
static void CLK_disable(USART_TypeDef* UARTx);
static void clear_interrypst(USART_TypeDef* UARTx);
static void IRQ_enable(USART_TypeDef* UARTx, xHAL_UART_INTERRYPTS* iHandle);
static void save_interrypts_handle(USART_TypeDef* UARTx, xHAL_UART_INTERRYPTS* iHandle);
static IRQn_Type get_IRQn(USART_TypeDef* UARTx);

/* --------------------------------------------------------------------------------------------------------------------------------------- */
/* Public Implementation ----------------------------------------------------------------------------------------------------------------- */
/* ....................................................................................................................................... */

//###################################################################################################################
void xHAL_UART_Init(xHAL_UART_Config* config)
{			
	USART_TypeDef* UARTx = config->UARTx;
	xHAL_UART_INTERRYPTS* IR = &config->handle;
	uint32_t tmp = 0UL;
	
	save_interrypts_handle(UARTx, IR);
	
	xHAL_UART_DeInit(UARTx);	
	CLK_enable(UARTx);	
	
	__Disable(UARTx);
			
	/* Control register 1 ----------------------------------------------------------------------------------
	tmp |= USART_CR1_SBK;														// Send Break
	tmp |= USART_CR1_RWU;														// Receiver wakeup
	tmp |= USART_CR1_WAKE;														// Wakeup method: 0 - Idle Line; 1 - Address Mark */		
	if (IR)
	{			
		tmp |= (bool)IR->IDLE_IdleLineDetected ? USART_CR1_IDLEIE : 0UL;  		// IDLE Interrupt Enable
		tmp |= IR->RXNE_Received_DataReadyToBeRead ? USART_CR1_RXNEIE : 0UL;  	// RXNE Interrupt Enable
		tmp |= IR->TXC_Transmission_Complete ? USART_CR1_TCIE : 0UL;  			// Transmission Complete Interrupt Enable
		tmp |= IR->TXE_Transmit_DataRegEmpty ? USART_CR1_TXEIE : 0UL;  			// TXE interrupt enable
		tmp |= IR->PE_ParityError ? USART_CR1_PEIE : 0UL;  						// PE Interrupt Enable	
	}
	tmp |= config->mode & USART_CR1_RE_Msk; 									// Receiver Enable
	tmp |= config->mode & USART_CR1_TE_Msk; 									// Transmitter Enable		
	tmp |= config->frame_format & USART_CR1_PS_Msk;  							// Parity Selection: 0 - Even parity; 1 - Odd parity
	tmp |= config->frame_format & USART_CR1_PCE_Msk;  							// Parity Control Enable
	tmp |= config->frame_format & USART_CR1_M_Msk;  							// Word length: 0 - 8 Data bits, n Stop bit; 9 Data bits, n Stop bit					
	tmp = tmp & (~USART_CR1_UE); 												// USART Enable	
	UARTx->CR1 = tmp; 															// Control register 1
	tmp = 0UL;	
	
	/* Control register 2 ----------------------------------------------------------------------------------
	tmp |= 0x0U << USART_CR2_ADD_Msk;											// [3,0] 0..F Address of the USART node
	tmp |= USART_CR2_LBDL;														// LIN Break Detection Length
	tmp |= USART_CR2_LBDIE;														// LIN Break Detection Interrupt Enable
	tmp |= USART_CR2_LBCL;														// Last Bit Clock pulse
	tmp |= USART_CR2_CPHA;														// Clock Phase
	tmp |= USART_CR2_CPOL;														// Clock Polarity
	tmp |= USART_CR2_CLKEN;														// Clock Enable
	tmp |= USART_CR2_LINEN;														// LIN mode enable */					
	tmp |= config->frame_format & USART_CR2_STOP_Msk;   						// STOP[1:0] (STOP bits): 00 - 1 bit; 01 - 0.5 bit; 10 - 2 bits; 11 - 1.5 bit	
	UARTx->CR2 = tmp; 															// Control register 2	
	tmp = 0UL;	
	
	/* Control register 3 ----------------------------------------------------------------------------------	 
	tmp |= USART_CR3_IREN;														// IrDA mode Enable
	tmp |= USART_CR3_IRLP;														// IrDA Low-Power
	tmp	|= USART_CR3_HDSEL;														// Half-Duplex Selection
	tmp |= USART_CR3_NACK;														// Smartcard NACK enable
	tmp |= USART_CR3_SCEN;														// Smartcard mode enable */				
	if (IR)
	{			
		tmp |= IR->EIE_Error_NE_ORE_FE ? USART_CR3_EIE : 0UL;   				// Error Interrupt Enable
		tmp |= IR->CTS_flag ? USART_CR3_CTSIE : 0UL;   							// CTS Interrupt Enable	
		tmp |= (!IR->ORE_EverrunErrorDetected) ? USART_CR3_OVRDIS : 0UL; 		// Overrun Disable	
	}
	if (config->flow_control)
	{
		tmp |= USART_CR3_DEM;													// Driver Enable Mode
	}
	tmp |= config->mode & USART_CR3_DMAR; 										// DMA Enable Receiver
	tmp |= config->mode & USART_CR3_DMAT; 										// DMA Enable Transmitter	
	tmp |= config->flow_control & USART_CR3_RTSE; 								// RTS Enable
	tmp |= config->flow_control & USART_CR3_CTSE; 								// CTS Enable						
	UARTx->CR3 = tmp; 															// Control register 3
	tmp = 0UL;	
	
	/* Guard time and prescaler register --------------------------------------------------------------------
	tmp |= (0U << USART_GTPR_PSC_Pos);											// PSC[7:0] bits (Prescaler value)
	tmp |= (0U << USART_GTPR_GT_Pos);											// [7:0] Guard time value */		
	UARTx->GTPR = tmp;  	 													// Guard time and prescaler register
	tmp = 0UL;	
	
	// BaudRate register		
	UARTx->BRR = __UART_DIV_SAMPLING16(RCC_PCLK1_freq_get(), config->baudrate);
	
	tmp = UARTx->RDR;
	
	clear_interrypst(UARTx);
	IRQ_enable(UARTx, IR);	
	__Enable(UARTx);
}

//###################################################################################################################
void xHAL_UART_DeInit(USART_TypeDef* UARTx)
{	
	CLK_enable(UARTx);	
	__Disable(UARTx);
		
	UARTx->CR1 = 0UL;  		// Control register 1
	UARTx->CR2 = 0UL;  		// Control register 2	
	UARTx->CR3 = 0UL;  		// Control register 3		
	UARTx->GTPR = 0UL;  	// Guard time and prescaler register	
	UARTx->BRR = 0UL;  		// BaudRate register
	UARTx->RQR = 0UL;
	UARTx->RTOR = 0UL;	
	
	clear_interrypst(UARTx);	
	CLK_disable(UARTx);
	
	IRQn_Type IRQn = get_IRQn(UARTx);		
	if (IRQn)
		NVIC_DisableIRQ(IRQn);	
}

//###################################################################################################################
void xHAL_UART_Start(USART_TypeDef* UARTx)
{
	clear_interrypst(UARTx);
	__Enable(UARTx);
}

//###################################################################################################################
void xHAL_UART_Stop(USART_TypeDef* UARTx)
{
	__Disable(UARTx);
	clear_interrypst(UARTx);
}

/* --------------------------------------------------------------------------------------------------------------------------------------- */
/* Private Implementation ---------------------------------------------------------------------------------------------------------------- */
/* ....................................................................................................................................... */

//###################################################################################################################
static void CLK_enable(USART_TypeDef* UARTx)
{
	switch ((uint32_t)UARTx)
	{
	case (uint32_t)USART1: {RCC->APB2ENR |= RCC_APB2ENR_USART1EN; break;}
	case (uint32_t)USART2: {RCC->APB1ENR |= RCC_APB1ENR_USART2EN; break;}	
	}
}

//###################################################################################################################
static void CLK_disable(USART_TypeDef* UARTx)
{
	switch ((uint32_t)UARTx)
	{
	case (uint32_t)USART1: {RCC->APB2ENR &= ~(RCC_APB2ENR_USART1EN); break;}
	case (uint32_t)USART2: {RCC->APB1ENR &= ~(RCC_APB1ENR_USART2EN); break;}	
	}
}

//###################################################################################################################
static void clear_interrypst(USART_TypeDef* UARTx)
{
	__Clear_Error(UARTx);
	__Clear_Interrypt_IDLE(UARTx);
	__Clear_Interrypt_TX_Complete(UARTx);
}

//###################################################################################################################
static void IRQ_enable(USART_TypeDef* UARTx, xHAL_UART_INTERRYPTS* iHandle)
{
	if (iHandle)
		if (iHandle->CTS_flag || 
			iHandle->EIE_Error_NE_ORE_FE || 
			iHandle->IDLE_IdleLineDetected || 
			iHandle->LBD_BreakFlag ||
			iHandle->ORE_EverrunErrorDetected || 
			iHandle->PE_ParityError ||
			iHandle->RXNE_Received_DataReadyToBeRead ||
			iHandle->TXC_Transmission_Complete || 
			iHandle->TXE_Transmit_DataRegEmpty) 
		{
			IRQn_Type IRQn = get_IRQn(UARTx);
			if (IRQn)
			{
				__NVIC_SetPriority(IRQn, iHandle->priority);
				__NVIC_EnableIRQ(IRQn);
			}
		}
}

//###################################################################################################################
static void save_interrypts_handle(USART_TypeDef* UARTx, xHAL_UART_INTERRYPTS* iHandle)
{
	xHAL_UART_INTERRYPTS* IR;

	switch ((uint32_t)UARTx)
	{
	case (uint32_t)USART1: { IR = &handles[0]; break; }
	case (uint32_t)USART2: { IR = &handles[1]; break; }
	default: return;
	}

	IR->CTS_flag = iHandle->CTS_flag;
	IR->EIE_Error_NE_ORE_FE = iHandle->EIE_Error_NE_ORE_FE;
	IR->IDLE_IdleLineDetected = iHandle->IDLE_IdleLineDetected;
	IR->LBD_BreakFlag = iHandle->LBD_BreakFlag;
	IR->ORE_EverrunErrorDetected = iHandle->ORE_EverrunErrorDetected;
	IR->PE_ParityError = iHandle->PE_ParityError;
	IR->RXNE_Received_DataReadyToBeRead = iHandle->RXNE_Received_DataReadyToBeRead;
	IR->TXC_Transmission_Complete = iHandle->TXC_Transmission_Complete;
	IR->TXE_Transmit_DataRegEmpty = iHandle->TXE_Transmit_DataRegEmpty;
}

//###################################################################################################################
static IRQn_Type get_IRQn(USART_TypeDef* UARTx)
{
	switch ((uint32_t)UARTx)
	{
	case (uint32_t)USART1: {return USART1_IRQn; }
	case (uint32_t)USART2: {return USART2_IRQn; }
	default: return 0UL;
	}
}

//###################################################################################################################
void Iterrypt(USART_TypeDef* UARTx, xHAL_UART_INTERRYPTS* iHandle)
{	
	if (iHandle && (IsInterryptFirst & __UART_InterryptFirst(UARTx)))
	{
		if (__Is_RX_Not_Empty(UARTx) ? iHandle->RXNE_Received_DataReadyToBeRead : false)
			iHandle->RXNE_Received_DataReadyToBeRead(); 									//__Clear_Interrypt_RX_Not_Empty(UARTx);

		if(__Is_TX_Empty(UARTx) ? iHandle->TXE_Transmit_DataRegEmpty : false)
			iHandle->TXE_Transmit_DataRegEmpty(); 											//__Clear_Interrypt_TX_Empty(UARTx);

		if(__Is_TX_Complete(UARTx) ? iHandle->TXC_Transmission_Complete : false)
			iHandle->TXC_Transmission_Complete(); 											//__Clear_Interrypt_TX_Complete(UARTx);

		if(__Is_IDLE(UARTx) ? (bool)iHandle->IDLE_IdleLineDetected : false)
			iHandle->IDLE_IdleLineDetected(); 												//__Clear_Interrypt_IDLE(UARTx);	

		if(__Is_ERR(UARTx))
		{
			if (__Is_ERR_Parity(UARTx) ? iHandle->PE_ParityError : false)
				iHandle->PE_ParityError();
			else if (iHandle->EIE_Error_NE_ORE_FE)
				iHandle->EIE_Error_NE_ORE_FE();

		}
	}
		
	clear_interrypst(UARTx);
	IsInterryptFirst |= __UART_InterryptFirst(UARTx);
}

/* --------------------------------------------------------------------------------------------------------------------------------------- */
/* Interrupt ----------------------------------------------------------------------------------------------------------------------------- */
/* ....................................................................................................................................... */

void USART1_IRQHandler(void)
{
	Iterrypt(USART1, &handles[0]);
}

void USART2_IRQHandler(void)
{
	Iterrypt(USART2, &handles[1]);
}

/* End ----------------------------------------------------------------------------------------------------------------------------------- */
/* ....................................................................................................................................... */
