/* --------------------------------------------------------------------------------------------------------------------------------------- */
/* Includes ------------------------------------------------------------------------------------------------------------------------------ */
/* ....................................................................................................................................... */

#include "tim.h"

/* --------------------------------------------------------------------------------------------------------------------------------------- */
/* Defines ------------------------------------------------------------------------------------------------------------------------------- */
/* ....................................................................................................................................... */

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
	TIM_3,
	TIM_6,
	TIM_14,
	TIM_15,
	TIM_16,
	TIM_17,
	TIM_COUNT,
}TIMERe;

/* --------------------------------------------------------------------------------------------------------------------------------------- */
/* Private ------------------------------------------------------------------------------------------------------------------------------- */
/* ....................................................................................................................................... */

static const TIM_TypeDef* TIMERs[TIM_COUNT] = { TIM1, TIM3, TIM6, TIM14, TIM15, TIM16, TIM17 };

static callback_f callbacks[TIM_COUNT] = { 0 };

static void update(TIM_TypeDef* xtim);
static void IRQ_set(TIM_TypeDef* xtim, callback_f callback);
static void callback_save(TIM_TypeDef* xtim, callback_f callback);
static IRQn_Type get_IRQn(TIM_TypeDef* xtim);

/* --------------------------------------------------------------------------------------------------------------------------------------- */
/* Public Implementation ----------------------------------------------------------------------------------------------------------------- */
/* ....................................................................................................................................... */

/**###################################################################################################################
*	@brief set the interval and start
*/
void xHAL_TIM_Start(TIM_TypeDef* xtim, uint16_t Prescaler, uint16_t ARRvalue, bool pulse, callback_f callback)
{
	xHAL_TIM_disable(xtim);

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

	xHAL_TIM_ENABLE(xtim);
}

/**###################################################################################################################
*	@brief counter stop, restart
*/
void xHAL_TIM_StartOver(TIM_TypeDef* xtim)
{
	xtim->CNT = 0;
	__TIM_UI_CLEAR(xtim);
	xHAL_TIM_ENABLE(xtim);	
}

/**###################################################################################################################
*	@brief counter stop
*/
void xHAL_TIM_Stop(TIM_TypeDef* xtim)
{		
	xHAL_TIM_disable(xtim);
	__TIM_UI_CLEAR(xtim);
}

/**###################################################################################################################
*	@brief Configure output compare channel
*/
void xHAL_TIM_OC_Config(TIM_TypeDef* xtim, uint32_t ch, TIM_OC_InitTypeDef* config)
{
	xHAL_TIM_CHANNEL_DISABLE(xtim, ch);							/* Disable the Channel: Reset the CCxE Bit */

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
	ccer |= TIM_CCER_CC1E << ch;

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
	xHAL_TIM_WRITE_COMPARE_VALUE(xtim, ch, config->Pulse);		/* Set the Capture Compare Register value */
	xtim->CCER = ccer;											/* Write to TIMx CCER */
}

/* --------------------------------------------------------------------------------------------------------------------------------------- */
/* Private Implementation ---------------------------------------------------------------------------------------------------------------- */
/* ....................................................................................................................................... */

//###################################################################################################################
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

//###################################################################################################################
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

//###################################################################################################################
static void callback_save(TIM_TypeDef* xtim, callback_f callback)
{
	TIMERe tim;
	switch ((uint32_t)xtim)
	{
	case (uint32_t)TIM1: {tim = TIM_1; break; }
	case (uint32_t)TIM3: {tim = TIM_3; break; }
	case (uint32_t)TIM6: {tim = TIM_6; break; }
	case (uint32_t)TIM14: {tim = TIM_14; break; }
	case (uint32_t)TIM15: {tim = TIM_15; break; }
	case (uint32_t)TIM16: {tim = TIM_16; break; }
	case (uint32_t)TIM17: {tim = TIM_17; break; }
	default: return;
	}

	callbacks[tim] = callback;
}

//###################################################################################################################
static IRQn_Type get_IRQn(TIM_TypeDef* xtim)
{
	switch ((uint32_t)xtim)
	{
	case (uint32_t)TIM1: {return TIM1_BRK_UP_TRG_COM_IRQn; }
	case (uint32_t)TIM3: {return TIM3_IRQn; }
	case (uint32_t)TIM6: {return TIM6_IRQn; }
	case (uint32_t)TIM14: {return TIM14_IRQn; }
	case (uint32_t)TIM15: {return TIM15_IRQn; }
	case (uint32_t)TIM16: {return TIM16_IRQn; }
	case (uint32_t)TIM17: {return TIM17_IRQn; }
	default: return 0UL;
	}
}

//###################################################################################################################
void xHAL_TIM_clock_enable(TIM_TypeDef* tim)
{
	switch ((uint32_t)tim)
	{
	case (uint32_t)TIM1: {__HAL_RCC_TIM1_CLK_ENABLE(); break; }
	case (uint32_t)TIM3: {__HAL_RCC_TIM3_CLK_ENABLE(); break; }
	case (uint32_t)TIM6: {__HAL_RCC_TIM6_CLK_ENABLE(); break; }
	case (uint32_t)TIM14: {__HAL_RCC_TIM14_CLK_ENABLE(); break; }
	case (uint32_t)TIM15: {__HAL_RCC_TIM15_CLK_ENABLE(); break; }
	case (uint32_t)TIM16: {__HAL_RCC_TIM16_CLK_ENABLE(); break; }
	case (uint32_t)TIM17: {__HAL_RCC_TIM17_CLK_ENABLE(); break; }
	}
}

//###################################################################################################################
void xHAL_TIM_clock_disable(TIM_TypeDef* tim)
{
	switch ((uint32_t)tim)
	{
	case (uint32_t)TIM1: {__HAL_RCC_TIM1_CLK_DISABLE(); break; }
	case (uint32_t)TIM3: {__HAL_RCC_TIM3_CLK_DISABLE(); break; }
	case (uint32_t)TIM6: {__HAL_RCC_TIM6_CLK_DISABLE(); break; }
	case (uint32_t)TIM14: {__HAL_RCC_TIM14_CLK_DISABLE(); break; }
	case (uint32_t)TIM15: {__HAL_RCC_TIM15_CLK_DISABLE(); break; }
	case (uint32_t)TIM16: {__HAL_RCC_TIM16_CLK_DISABLE(); break; }
	case (uint32_t)TIM17: {__HAL_RCC_TIM17_CLK_DISABLE(); break; }
	}
}

void xHAL_TIM_disable(TIM_TypeDef* tim)
{
	tim->CR1 &= ~(TIM_CR1_CEN);
}

/* --------------------------------------------------------------------------------------------------------------------------------------- */
/* Interrupt ----------------------------------------------------------------------------------------------------------------------------- */
/* ....................................................................................................................................... */

void TIM1_BRK_UP_TRG_COM_IRQHandler(void)
{	
	TIM_UI_HANDLER(TIM_1);
}

void TIM1_CC_IRQHandler(void)
{
	//TIM_UI_HANDLER(TIM_1);
}

void TIM3_IRQHandler(void) 
{
	TIM_UI_HANDLER(TIM_3);
}

void TIM6_IRQHandler(void) 
{
	TIM_UI_HANDLER(TIM_6);
}

void TIM14_IRQHandler(void) 
{
	TIM_UI_HANDLER(TIM_14);
}

void TIM15_IRQHandler(void)
{
	TIM_UI_HANDLER(TIM_15);
}

void TIM16_IRQHandler(void)
{
	TIM_UI_HANDLER(TIM_16);
}

void TIM17_IRQHandler(void)
{
	TIM_UI_HANDLER(TIM_17);
}

/* End ----------------------------------------------------------------------------------------------------------------------------------- */
/* ....................................................................................................................................... */
