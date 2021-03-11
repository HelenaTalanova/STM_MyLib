// #########################################################################################################
// * Includes ____________________________________________________________________________________________

#include "custom.h"

// #########################################################################################################
// * Defines config ______________________________________________________________________________________

// uncomment for using
#define USED_UART1
#define USED_UART2
#define USED_UART3
#define USED_UART4
#define USED_UART5

// #########################################################################################################
// * Defines _____________________________________________________________________________________________

#define COUNT_USART			 						5U

#define __Enable(UARTx)								(UARTx->CR1 |= USART_CR1_UE)
#define __Disable(UARTx)							(UARTx->CR1 &= ~USART_CR1_UE)

/* Check status register */
#define __Is_ERR(UARTx)								((UARTx->SR & USART_SR_PE | USART_SR_FE | USART_SR_NE | USART_SR_ORE) != RESET)
#define __Is_ERR_Parity(UARTx)						((UARTx->SR & USART_SR_PE) != RESET)
#define __Is_ERR_Framing(UARTx)						((UARTx->SR & USART_SR_FE) != RESET)
#define __Is_ERR_Noise(UARTx)						((UARTx->SR & USART_SR_NE) != RESET)
#define __Is_ERR_OverRun(UARTx)						((UARTx->SR & USART_SR_ORE) != RESET)
#define __Is_IDLE(UARTx)							((UARTx->SR & USART_SR_IDLE) != RESET)
#define __Is_TX_Complete(UARTx)						((UARTx->SR & USART_SR_TC) != RESET)
#define __Is_TX_Empty(UARTx)						((UARTx->SR & USART_SR_TXE) != RESET)
#define __Is_LIN_Break(UARTx)						((UARTx->SR & USART_SR_LBD) != RESET)
#define __Is_CTS_Flag(UARTx)						((UARTx->SR & USART_SR_CTS) != RESET)
#define __Is_RX_Not_Empty(UARTx)					((UARTx->SR & USART_SR_RXNE) != RESET)

/* Clear Status register */
#define __Clear_Error(UARTx)						do{ uint32_t tmp = UARTx->SR; __ASM("nop"); tmp = UARTx->DR; __ASM("nop"); }while(0U);
#define __Clear_Interrypt_ERR_Parity(UARTx)			__Clear_Error(UARTx)
#define __Clear_Interrypt_ERR_Framing(UARTx)		__Clear_Error(UARTx)
#define __Clear_Interrypt_ERR_Noise(UARTx)			__Clear_Error(UARTx)
#define __Clear_Interrypt_ERR_OverRun(UARTx)		__Clear_Error(UARTx)
#define __Clear_Interrypt_IDLE(UARTx)				__Clear_Error(UARTx)
#define __Clear_Interrypt_TX_Complete(UARTx)		CLEAR_BIT(UARTx->SR, USART_SR_TC)
#define __Clear_Interrypt_TX_Empty(UARTx)			CLEAR_BIT(UARTx->SR, USART_SR_TXE)
#define __Clear_Interrypt_LIN_Break(UARTx)			CLEAR_BIT(UARTx->SR, USART_SR_LBD)
#define __Clear_Interrypt_CTS_Flag(UARTx)			CLEAR_BIT(UARTx->SR, USART_SR_CTS)
#define __Clear_Interrypt_RX_Not_Empty(UARTx)		do{ __Clear_Error(UARTx); CLEAR_BIT(UARTx->SR, USART_SR_RXNE); }while(0U);

// #########################################################################################################
// * Private _____________________________________________________________________________________________

static iHAL_UART_INTERRYPTS handle_interrypts[COUNT_USART] __attribute__((unused)) = { 0 };

static void enable_clock(USART_TypeDef* UARTx);
static void disable_clock(USART_TypeDef* UARTx);
static void clear_it(USART_TypeDef* UARTx);
static void enable_IRQ(USART_TypeDef* UARTx, iHAL_UART_INTERRYPTS* it_handle);
static void save_it_handle(USART_TypeDef* UARTx, iHAL_UART_INTERRYPTS* it_handle);
static IRQn_Type get_IRQn(USART_TypeDef* UARTx);
static void iterrypt_exe(USART_TypeDef* UARTx, iHAL_UART_INTERRYPTS* it_handle) __attribute__((unused));

// #########################################################################################################
// * Public: implementation ______________________________________________________________________________

/// **************************************************************************************
void iHAL_UART_Config(UART_Config* config)
{
	assert_null(config);
	assert_null(config->UARTx);

	USART_TypeDef* UARTx = config->UARTx;
	iHAL_UART_INTERRYPTS* IT = &config->handle;
	uint32_t tmp = 0UL;

	save_it_handle(UARTx, IT);

	iHAL_UART_DeConfig(UARTx);
	enable_clock(UARTx);

	__Disable(UARTx);

	/* Control register 1 ----------------------------------------------------------------------------------
	tmp |= USART_CR1_SBK;														// Send Break
	tmp |= USART_CR1_RWU;														// Receiver wakeup
	tmp |= USART_CR1_WAKE;														// Wakeup method: 0 - Idle Line; 1 - Address Mark */
	if (IT)
	{
		tmp |= IT->IDLE_IdleLineDetected ? USART_CR1_IDLEIE : 0UL; 				// IDLE Interrupt Enable
		tmp |= IT->RXNE_Received_DataReadyToBeRead ? USART_CR1_RXNEIE : 0UL; 	// RXNE Interrupt Enable
		tmp |= IT->TXC_Transmission_Complete ? USART_CR1_TCIE : 0UL; 			// Transmission Complete Interrupt Enable
		tmp |= IT->TXE_Transmit_DataRegEmpty ? USART_CR1_TXEIE : 0UL; 			// TXE interrupt enable
		tmp |= IT->PE_ParityError ? USART_CR1_PEIE : 0UL; 						// PE Interrupt Enable	
	}
	tmp |= config->mode & USART_CR1_RE_Msk;										// Receiver Enable
	tmp |= config->mode & USART_CR1_TE_Msk;										// Transmitter Enable		
	tmp |= config->frame_format.parity & USART_CR1_PS_Msk; 						// Parity Selection: 0 - Even parity; 1 - Odd parity
	tmp |= config->frame_format.parity & USART_CR1_PCE_Msk; 					// Parity Control Enable
	tmp |= config->frame_format.word_len & USART_CR1_M_Msk; 					// Word length: 0 - 8 Data bits, n Stop bit; 9 Data bits, n Stop bit					
	tmp = tmp & (~USART_CR1_UE);												// USART Enable	
	UARTx->CR1 = tmp;															// Control register 1
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
	tmp |= config->frame_format.stop_byte & USART_CR2_STOP_Msk;  				// STOP[1:0] (STOP bits): 00 - 1 bit; 01 - 0.5 bit; 10 - 2 bits; 11 - 1.5 bit	
	UARTx->CR2 = tmp;															// Control register 2	
	tmp = 0UL;

	/* Control register 3 ----------------------------------------------------------------------------------
	tmp |= USART_CR3_IREN;														// IrDA mode Enable
	tmp |= USART_CR3_IRLP;														// IrDA Low-Power
	tmp	|= USART_CR3_HDSEL;														// Half-Duplex Selection
	tmp |= USART_CR3_NACK;														// Smartcard NACK enable
	tmp |= USART_CR3_SCEN;														// Smartcard mode enable */
	if (IT)
	{
		tmp |= IT->EIE_Error_NE_ORE_FE ? USART_CR3_EIE : 0UL;  					// Error Interrupt Enable
		tmp |= IT->CTS_flag ? USART_CR3_CTSIE : 0UL;  							// CTS Interrupt Enable	
	}
	tmp |= config->mode & USART_CR3_DMAR;										// DMA Enable Receiver
	tmp |= config->mode & USART_CR3_DMAT;										// DMA Enable Transmitter	
	tmp |= config->flow_control & USART_CR3_RTSE;								// RTS Enable
	tmp |= config->flow_control & USART_CR3_CTSE;								// CTS Enable						
	UARTx->CR3 = tmp;															// Control register 3
	tmp = 0UL;

	/* Guard time and prescaler register --------------------------------------------------------------------
	tmp |= (0U << USART_GTPR_PSC_Pos);											// PSC[7:0] bits (Prescaler value)
	tmp |= (0U << USART_GTPR_GT_Pos);											// [7:0] Guard time value */
	UARTx->GTPR = tmp; 	 														// Guard time and prescaler register
	tmp = 0UL;

	tmp = UARTx == USART1 ? HAL_RCC_GetPCLK2Freq() : HAL_RCC_GetPCLK1Freq();
	UARTx->BRR = UART_BRR_SAMPLING16(tmp, config->frame_format.baudrate);		// BaudRate register

	clear_it(UARTx);
	enable_IRQ(UARTx, IT);
	__Enable(UARTx);
}

/// **************************************************************************************
void iHAL_UART_DeConfig(USART_TypeDef* UARTx)
{
	assert_null(UARTx);

	enable_clock(UARTx);
	__Disable(UARTx);

	UARTx->CR1 = 0UL; 		// Control register 1
	UARTx->CR2 = 0UL; 		// Control register 2	
	UARTx->CR3 = 0UL; 		// Control register 3		
	UARTx->GTPR = 0UL; 		// Guard time and prescaler register	
	UARTx->BRR = 0UL; 		// BaudRate register
	UARTx->GTPR = 0UL;

	clear_it(UARTx);
	disable_clock(UARTx);

	IRQn_Type IRQn = get_IRQn(UARTx);

	assert_null(IRQn);

	NVIC_DisableIRQ(IRQn);
}

/// **************************************************************************************
void iHAL_UART_Start(USART_TypeDef* UARTx)
{
	assert_null(UARTx);

	clear_it(UARTx);
	__Enable(UARTx);
}

/// **************************************************************************************
void iHAL_UART_Stop(USART_TypeDef* UARTx)
{
	assert_null(UARTx);

	__Disable(UARTx);
	clear_it(UARTx);
}

/// **************************************************************************************
bool iHAL_UART_IsTX_Free(USART_TypeDef* UARTx)
{
	assert_null(UARTx);

	return __Is_TX_Empty(UARTx) || __Is_TX_Complete(UARTx);
}

/// **************************************************************************************
bool iHAL_UART_IsRX_Free(USART_TypeDef* UARTx)
{
	assert_null(UARTx);

	if (__Is_IDLE(UARTx))
	{
		__Clear_Interrypt_IDLE(UARTx);
		return true;
	}
	return false;
}

// #########################################################################################################
// * Private: Implementation _____________________________________________________________________________

/// **************************************************************************************
static void enable_clock(USART_TypeDef* UARTx)
{
	assert_null(UARTx);

	switch ((uint32_t)UARTx)
	{
	case USART1_BASE: {__HAL_RCC_USART1_CLK_ENABLE(); break; }
	case USART2_BASE: {__HAL_RCC_USART2_CLK_ENABLE(); break; }
	case USART3_BASE: {__HAL_RCC_USART3_CLK_ENABLE(); break; }
	case UART4_BASE: {__HAL_RCC_UART4_CLK_ENABLE(); break; }
	case UART5_BASE: {__HAL_RCC_UART5_CLK_ENABLE(); break; }
	}
}

/// **************************************************************************************
static void disable_clock(USART_TypeDef* UARTx)
{
	assert_null(UARTx);

	switch ((uint32_t)UARTx)
	{
	case USART1_BASE: {__HAL_RCC_USART1_CLK_DISABLE(); break; }
	case USART2_BASE: {__HAL_RCC_USART2_CLK_DISABLE(); break; }
	case USART3_BASE: {__HAL_RCC_USART3_CLK_DISABLE(); break; }
	case UART4_BASE: {__HAL_RCC_UART4_CLK_DISABLE(); break; }
	case UART5_BASE: {__HAL_RCC_UART5_CLK_DISABLE(); break; }
	}
}

/// **************************************************************************************
static void clear_it(USART_TypeDef* UARTx)
{
	assert_null(UARTx);

	__Clear_Error(UARTx);
	__Clear_Interrypt_IDLE(UARTx);
	__Clear_Interrypt_TX_Complete(UARTx);
	__Clear_Interrypt_TX_Empty(UARTx);
	__Clear_Interrypt_LIN_Break(UARTx);
	__Clear_Interrypt_CTS_Flag(UARTx);
	__Clear_Interrypt_RX_Not_Empty(UARTx);
}

/// **************************************************************************************
static void enable_IRQ(USART_TypeDef* UARTx, iHAL_UART_INTERRYPTS* it_handle)
{
	assert_null(UARTx);
	assert_null(it_handle);

	if (it_handle->CTS_flag ||
		it_handle->EIE_Error_NE_ORE_FE ||
		it_handle->IDLE_IdleLineDetected ||
		it_handle->LBD_BreakFlag ||
		it_handle->ORE_EverrunErrorDetected ||
		it_handle->PE_ParityError ||
		it_handle->RXNE_Received_DataReadyToBeRead ||
		it_handle->TXC_Transmission_Complete ||
		it_handle->TXE_Transmit_DataRegEmpty)
	{
		IRQn_Type IRQn = get_IRQn(UARTx);

		assert_null(IRQn);

		NVIC_SetPriority(IRQn, it_handle->priority);
		NVIC_EnableIRQ(IRQn);
	}
}

/// **************************************************************************************
static void save_it_handle(USART_TypeDef* UARTx, iHAL_UART_INTERRYPTS* it_handle)
{
	assert_null(UARTx);
	assert_null(it_handle);

	iHAL_UART_INTERRYPTS* it;

	switch ((uint32_t)UARTx)
	{
	case USART1_BASE: { it = &handle_interrypts[0]; break; }
	case USART2_BASE: { it = &handle_interrypts[1]; break; }
	case USART3_BASE: { it = &handle_interrypts[2]; break; }
	case UART4_BASE: { it = &handle_interrypts[3]; break; }
	case UART5_BASE: { it = &handle_interrypts[4]; break; }
	default: {it = 0U; break; }
	}

	assert_null(it);

	it->CTS_flag = it_handle->CTS_flag;
	it->EIE_Error_NE_ORE_FE = it_handle->EIE_Error_NE_ORE_FE;
	it->IDLE_IdleLineDetected = it_handle->IDLE_IdleLineDetected;
	it->LBD_BreakFlag = it_handle->LBD_BreakFlag;
	it->ORE_EverrunErrorDetected = it_handle->ORE_EverrunErrorDetected;
	it->PE_ParityError = it_handle->PE_ParityError;
	it->RXNE_Received_DataReadyToBeRead = it_handle->RXNE_Received_DataReadyToBeRead;
	it->TXC_Transmission_Complete = it_handle->TXC_Transmission_Complete;
	it->TXE_Transmit_DataRegEmpty = it_handle->TXE_Transmit_DataRegEmpty;
}

/// **************************************************************************************
static IRQn_Type get_IRQn(USART_TypeDef* UARTx)
{
	assert_null(UARTx);

	switch ((uint32_t)UARTx)
	{
	case USART1_BASE: {return USART1_IRQn; }
	case USART2_BASE: {return USART2_IRQn; }
	case USART3_BASE: {return USART3_IRQn; }
	case UART4_BASE: {return UART4_IRQn; }
	case UART5_BASE: {return UART5_IRQn; }
	}

	return (IRQn_Type)0UL;
}

/// **************************************************************************************
static void iterrypt_exe(USART_TypeDef* UARTx, iHAL_UART_INTERRYPTS* it_handle)
{
	assert_null(UARTx);
	assert_null(it_handle);

	if (__Is_RX_Not_Empty(UARTx) ? it_handle->RXNE_Received_DataReadyToBeRead : false)
		it_handle->RXNE_Received_DataReadyToBeRead();									//__Clear_Interrypt_RX_Not_Empty(UARTx);

	if (__Is_TX_Empty(UARTx) ? it_handle->TXE_Transmit_DataRegEmpty : false)
		it_handle->TXE_Transmit_DataRegEmpty();											//__Clear_Interrypt_TX_Empty(UARTx);

	if (__Is_TX_Complete(UARTx) ? it_handle->TXC_Transmission_Complete : false)
		it_handle->TXC_Transmission_Complete();											//__Clear_Interrypt_TX_Complete(UARTx);

	if (__Is_IDLE(UARTx) ? it_handle->IDLE_IdleLineDetected : false)
		it_handle->IDLE_IdleLineDetected();												//__Clear_Interrypt_IDLE(UARTx);	

	if (__Is_LIN_Break(UARTx) ? it_handle->LBD_BreakFlag : false)
		it_handle->LBD_BreakFlag();														//__Clear_Interrypt_LIN_Break(UARTx);

	if (__Is_CTS_Flag(UARTx) ? it_handle->CTS_flag : false)
		it_handle->CTS_flag();															//__Clear_Interrypt_CTS_Flag(UARTx);


	if (__Is_ERR(UARTx))
	{
		if (__Is_ERR_Parity(UARTx) ? it_handle->PE_ParityError : false)
			it_handle->PE_ParityError();
		else if (it_handle->EIE_Error_NE_ORE_FE)
			it_handle->EIE_Error_NE_ORE_FE();

	}

	clear_it(UARTx);
}

// #########################################################################################################
// * Interrupt ___________________________________________________________________________________________

#ifdef USED_UART1
/// **************************************************************************************
void USART1_IRQHandler(void)
{
	iterrypt_exe(USART1, &handle_interrypts[0]);
}
#endif // USED_UART1

#ifdef USED_UART2
/// **************************************************************************************
void USART2_IRQHandler(void)
{
	iterrypt_exe(USART2, &handle_interrypts[1]);
}
#endif // USED_UART2

#ifdef USED_UART3
/// **************************************************************************************
void USART3_IRQHandler(void)
{
	iterrypt_exe(USART3, &handle_interrypts[2]);
}
#endif // USED_UART3

#ifdef USED_UART4
/// **************************************************************************************
void UART4_IRQHandler(void)
{
	iterrypt_exe(UART4, &handle_interrypts[3]);
}
#endif // USED_UART4

#ifdef USED_UART5
/// **************************************************************************************
void UART5_IRQHandler(void)
{
	iterrypt_exe(UART5, &handle_interrypts[4]);
}
#endif // USED_UART5

/// #########################################################################################################
/// * End file uart.c                                                                                 #####
/// #####################################################################################################
