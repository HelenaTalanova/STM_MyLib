// #########################################################################################################
// * Includes ____________________________________________________________________________________________

#include "custom.h"

/** @brief  Update interrupt enable
*	reg DIER set UIE
*/
#define __TIM_UI_ENABLE(TIMx)			    	            __TIMx(TIMx)->DIER |= TIM_DIER_UIE

/** @brief  Update interrupt enable
*	reg DIER clear UIE
*/
#define __TIM_UI_DISABLE(TIMx)	        		            __TIMx(TIMx)->DIER &= ~(TIM_DIER_UIE)

/** @brief  Update interrupt clear flag
*   reg SR clear UIF
*/
#define __TIM_UI_CLEAR(TIMx)	    				         __TIMx(TIMx)->SR &= ~(TIM_SR_UIF)

#define TIM_READ_CCMRx(TIMx, CH)							((CH < TIM_CHANNEL_3) ? __TIMx(TIMx)->CCMR1 : __TIMx(TIMx)->CCMR2)
#define TIM_WRITE_CCMRx(TIMx, CH, VALUE)					__REG_WRITE(((CH < TIM_CHANNEL_3) ? &__TIMx(TIMx)->CCMR1 : &__TIMx(TIMx)->CCMR2), VALUE)
#define TIM_UI_HANDLER(TIM)									{ __TIM_UI_CLEAR(TIMERs[TIM]); if (callbacks[TIM]) callbacks[TIM]();}

typedef enum {
	TIM_1,
	TIM_2,
	TIM_3,
	TIM_4,
	TIM_5,
	TIM_6,
	TIM_7,
	TIM_8,
	TIM_COUNT,
}TIMERe;

// #########################################################################################################
// * Private: ____________________________________________________________________________________________

static const TIM_TypeDef* TIMERs[TIM_COUNT] = { TIM1, TIM2, TIM3, TIM4, TIM5, TIM6, TIM7, TIM8 };

static callback_f callbacks[TIM_COUNT] = { 0 };

static void update(TIM_TypeDef* xtim);
static void IRQ_set(TIM_TypeDef* xtim, callback_f callback);
static void callback_save(TIM_TypeDef* xtim, callback_f callback);
static IRQn_Type get_IRQn(TIM_TypeDef* xtim);

/// **************************************************************************************

// #########################################################################################################
// * Public: implementation ______________________________________________________________________________

/// **************************************************************************************
/*	@brief set the interval and start
*/
void iHAL_TIM_Start(TIM_TypeDef* xtim, uint16_t Prescaler, uint16_t ARRvalue, bool pulse, callback_f callback)
{
	iHAL_TIM_clock_enable(xtim);
	iHAL_TIM_disable(xtim);

	/**	---------------------------
	 **		CNT counter
	 **		PSC prescaler
	 **		ARR Auto-reload value
	 ***/
	xtim->CNT = 0;
	xtim->PSC = Prescaler - 1;
	xtim->ARR = ARRvalue - 1;

	/* Update generation */
	update(xtim);

	xtim->CR1 =							/** CR1 control register 1 **/
		(0 << TIM_CR1_UDIS_Pos) |		/* Off - Update disable */
		(0 << TIM_CR1_URS_Pos) |		/* Off - Update request source */
		(1 << TIM_CR1_ARPE_Pos) |		/* On  - Auto-reload preload enable */
		(pulse << TIM_CR1_OPM_Pos)     	/* On  - One - pulse mode */
		;

	xtim->CR2 =							/**	CR2 control register 2 **/
		(0 << TIM_CR2_MMS_Pos)			/* Off	- Master mode selection */
		;

	xtim->DIER =						/**	DIER DMA/Interrupt enable register **/
		(0 << TIM_DIER_UDE_Pos)			/* Off	- Update DMA request enable */
		;

	IRQ_set(xtim, callback);

	iHAL_TIM_ENABLE(xtim);
}

/// **************************************************************************************
/*	@brief counter stop, restart
*/
void iHAL_TIM_StartOver(TIM_TypeDef* xtim)
{
	xtim->CNT = 0;
	__TIM_UI_CLEAR(xtim);
	iHAL_TIM_ENABLE(xtim);
}

/// **************************************************************************************
/*	@brief counter stop
*/
void iHAL_TIM_Stop(TIM_TypeDef* xtim)
{
	iHAL_TIM_disable(xtim);
	__TIM_UI_CLEAR(xtim);
}

/// **************************************************************************************
/*	@brief Configure output compare channel
*/
void iHAL_TIM_OC_Config(TIM_TypeDef* xtim, uint32_t ch, TIM_OC_InitTypeDef* config)
{
	iHAL_TIM_CHANNEL_DISABLE(xtim, ch);							/* Disable the Channel: Reset the CCxE Bit */

	uint32_t ccmrx = TIM_READ_CCMRx(xtim, ch);					/* Get the TIMx CCMRx register value */
	uint32_t ccer = xtim->CCER;									/* Get the TIMx CCER register value */
	uint32_t cr2 = xtim->CR2;									/* Get the TIMx CR2 register value */
	bool even_ch = !((ch / 4) % 2);

	/* Reset the Output Compare Mode Bits */
	ccmrx &= ~(0x7UL << (even_ch ? 4UL : 12UL));				/* TIM_CCMRX_OCxM */
	ccmrx &= ~(0x3UL << (even_ch ? 0UL : 8UL));					/* TIM_CCMR1_CCxS */

	ccmrx |= (config->OCMode << (even_ch ? 0UL : 8UL));			/* Select the Output Compare Mode */

	ccer &= ~(TIM_CCER_CC1P << ch);								/* Reset the Output Polarity level */
	ccer |= (config->OCPolarity << ch);							/* Set the Output Compare Polarity */

	if (IS_TIM_CCXN_INSTANCE(xtim, ch) && (ch < TIM_CHANNEL_4))
	{
		ccer &= ~(TIM_CCER_CC1NP << ch);						/* Reset the Output N Polarity level */
		ccer |= (config->OCNPolarity << ch);					/* Set the Output N Polarity */
		ccer &= ~(TIM_CCER_CC1NE << ch);						/* Reset the Output N State */
	}

	if (IS_TIM_BREAK_INSTANCE(xtim) && (ch < TIM_CHANNEL_4))
	{
		cr2 &= ~(TIM_CR2_OIS1 << (ch / 2));
		cr2 &= ~(TIM_CR2_OIS1N << (ch / 2));
		cr2 |= (config->OCIdleState << (ch / 2));				/* Set the Output Idle state */
		cr2 |= (config->OCNIdleState << (ch / 2));				/* Set the Output N Idle state */
	}
	else if (IS_TIM_BREAK_INSTANCE(xtim) && (ch == TIM_CHANNEL_4))
	{
		cr2 &= ~TIM_CR2_OIS4;									/* Reset the Output Compare IDLE State */
		cr2 |= (config->OCIdleState << 6U);						/* Set the Output Idle state */
	}

	if (IS_TIM_BREAK_INSTANCE(xtim))							/* Enable the main output */
	{
		xtim->BDTR |= TIM_BDTR_MOE;
	}

	xtim->CR2 = cr2;											/* Write to TIMx CR2 */
	TIM_WRITE_CCMRx(xtim, ch, ccmrx);							/* Write to TIMx CCMRx */
	iHAL_TIM_WRITE_COMPARE_VALUE(xtim, ch, config->Pulse);		/* Set the Capture Compare Register value */
	xtim->CCER = ccer;											/* Write to TIMx CCER */
}

/// **************************************************************************************
void iHAL_TIM_clock_enable(TIM_TypeDef* tim)
{
	switch ((uint32_t)tim)
	{
	case (uint32_t)TIM1: {__HAL_RCC_TIM1_CLK_ENABLE(); break; }
	case (uint32_t)TIM2: {__HAL_RCC_TIM2_CLK_ENABLE(); break; }
	case (uint32_t)TIM3: {__HAL_RCC_TIM3_CLK_ENABLE(); break; }
	case (uint32_t)TIM4: {__HAL_RCC_TIM4_CLK_ENABLE(); break; }
	case (uint32_t)TIM5: {__HAL_RCC_TIM5_CLK_ENABLE(); break; }
	case (uint32_t)TIM6: {__HAL_RCC_TIM6_CLK_ENABLE(); break; }
	case (uint32_t)TIM7: {__HAL_RCC_TIM7_CLK_ENABLE(); break; }
	case (uint32_t)TIM8: {__HAL_RCC_TIM8_CLK_ENABLE(); break; }
	}
}

/// **************************************************************************************
void iHAL_TIM_clock_disable(TIM_TypeDef* tim)
{
	switch ((uint32_t)tim)
	{
	case (uint32_t)TIM1: {__HAL_RCC_TIM1_CLK_DISABLE(); break; }
	case (uint32_t)TIM2: {__HAL_RCC_TIM2_CLK_DISABLE(); break; }
	case (uint32_t)TIM3: {__HAL_RCC_TIM3_CLK_DISABLE(); break; }
	case (uint32_t)TIM4: {__HAL_RCC_TIM4_CLK_DISABLE(); break; }
	case (uint32_t)TIM5: {__HAL_RCC_TIM5_CLK_DISABLE(); break; }
	case (uint32_t)TIM6: {__HAL_RCC_TIM6_CLK_DISABLE(); break; }
	case (uint32_t)TIM7: {__HAL_RCC_TIM7_CLK_DISABLE(); break; }
	case (uint32_t)TIM8: {__HAL_RCC_TIM8_CLK_DISABLE(); break; }
	}
}

/// **************************************************************************************
void iHAL_TIM_disable(TIM_TypeDef* tim)
{
	tim->CR1 &= ~(TIM_CR1_CEN);
}

// #########################################################################################################
// * Private: implementation ______________________________________________________________________________

/// **************************************************************************************
static void update(TIM_TypeDef* xtim)
{
	__TIM_UI_DISABLE(xtim);

	/* Update generation */
	xtim->EGR |= TIM_EGR_UG;
	__ASM("nop");
	__ASM("nop");

	/* Clear - Update interrupt flag */
	__TIM_UI_CLEAR(xtim);
}

/// **************************************************************************************
static void IRQ_set(TIM_TypeDef* xtim, callback_f callback)
{
	IRQn_Type IRQn = get_IRQn(xtim);

	__TIM_UI_DISABLE(xtim);

	callback_save(xtim, callback);

	if (callback && IRQn)
	{
		NVIC_EnableIRQ(IRQn);
		__TIM_UI_ENABLE(xtim);
	}
	else if (IRQn)
	{
		NVIC_DisableIRQ(IRQn);
	}
}

/// **************************************************************************************
static void callback_save(TIM_TypeDef* xtim, callback_f callback)
{
	TIMERe tim;
	switch ((uint32_t)xtim)
	{
	case (uint32_t)TIM1: {tim = TIM_1; break; }
	case (uint32_t)TIM2: {tim = TIM_2; break; }
	case (uint32_t)TIM3: {tim = TIM_3; break; }
	case (uint32_t)TIM4: {tim = TIM_4; break; }
	case (uint32_t)TIM5: {tim = TIM_5; break; }
	case (uint32_t)TIM6: {tim = TIM_6; break; }
	case (uint32_t)TIM7: {tim = TIM_7; break; }
	case (uint32_t)TIM8: {tim = TIM_8; break; }
	default: return;
	}

	callbacks[tim] = callback;
}

static IRQn_Type get_IRQn(TIM_TypeDef* xtim)
{
	switch ((uint32_t)xtim)
	{
	case (uint32_t)TIM1: {return TIM1_UP_IRQn; }
	case (uint32_t)TIM2: {return TIM2_IRQn; }
	case (uint32_t)TIM3: {return TIM3_IRQn; }
	case (uint32_t)TIM4: {return TIM4_IRQn; }
	case (uint32_t)TIM5: {return TIM5_IRQn; }
	case (uint32_t)TIM6: {return TIM6_IRQn; }
	case (uint32_t)TIM7: {return TIM7_IRQn; }
	case (uint32_t)TIM8: {return TIM8_UP_IRQn; }
	default: return 0UL;
	}
}

void TIM1_BRK_IRQHandler(void)
{
	//TIM_UI_HANDLER(TIM_1);
}

void TIM1_UP_IRQHandler(void)
{
	TIM_UI_HANDLER(TIM_1);
}

void TIM1_TRG_COM_IRQHandler(void)
{
	//TIM_UI_HANDLER(TIM_1);
}

void TIM1_CC_IRQHandler(void)
{
	//TIM_UI_HANDLER(TIM_1);
}

void TIM2_IRQHandler(void)
{
	TIM_UI_HANDLER(TIM_2);
}

void TIM3_IRQHandler(void)
{
	TIM_UI_HANDLER(TIM_3);
}

void TIM4_IRQHandler(void)
{
	TIM_UI_HANDLER(TIM_4);
}

void TIM5_IRQHandler(void)
{
	TIM_UI_HANDLER(TIM_5);
}

void TIM6_IRQHandler(void)
{
	TIM_UI_HANDLER(TIM_6);
}

void TIM7_IRQHandler(void)
{
	TIM_UI_HANDLER(TIM_7);
}

void TIM8_BRK_IRQHandler(void)
{
	//TIM_UI_HANDLER(TIM_7);
}

void TIM8_UP_IRQHandler(void)
{
	TIM_UI_HANDLER(TIM_7);
}

void TIM8_TRG_COM_IRQHandler(void)
{
	//TIM_UI_HANDLER(TIM_7);
}

void TIM8_CC_IRQHandler(void)
{
	//TIM_UI_HANDLER(TIM_7);
}

/// #########################################################################################################
/// * End file tim.c                                                                                  #####
/// #####################################################################################################
