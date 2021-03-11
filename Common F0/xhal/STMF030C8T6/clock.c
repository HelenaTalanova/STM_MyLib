/* --------------------------------------------------------------------------------------------------------------------------------------- */
/* Includes ------------------------------------------------------------------------------------------------------------------------------ */
/* ....................................................................................................................................... */

#include "custom.h"

/* --------------------------------------------------------------------------------------------------------------------------------------- */
/* Defines ------------------------------------------------------------------------------------------------------------------------------- */
/* ....................................................................................................................................... */

#define STRIVE_FOR_FREQ							(uint32_t)48000000
#define SYSTEM_Tick_Value						(uint32_t)10000

#define SYSTEM_Tick_Prescale_MS(tick)			( tick / ( SYSTEM_Tick_Value / 1000 ) )

// Default value of the External oscillator in Hz. This value can be provided and adapted by the user application.
#undef HSE_VALUE
#define HSE_VALUE								((uint32_t)16000000) 

// Default value of the Internal oscillator in Hz. This value can be provided and adapted by the user application.
#undef HSI_VALUE
#define HSI_VALUE								((uint32_t)8000000) 

// Default value of the HSI48 Internal oscillator in Hz. This value can be provided and adapted by the user application.
#undef HSI48_VALUE
#define HSI48_VALUE								((uint32_t)48000000)

//---- HSE Configuration

#define RCC_HSE_Tick_Timeout					1000UL
#define RCC_HSE_IsEnable						((RCC->CR & RCC_CR_HSERDY) == RCC_CR_HSERDY)
#define RCC_HSE_IsDisable						((RCC->CR & RCC_CR_HSERDY) == RESET)
#define RCC_HSE_Enable()						(RCC->CR |= RCC_CR_HSEON)
#define RCC_HSE_Disable()						(RCC->CR &= ~RCC_CR_HSEON)

//---- HSI Configuration

#define RCC_HSI_Tick_Timeout					1000UL
#define RCC_HSI_IsEnable						((RCC->CR & RCC_CR_HSIRDY) == RCC_CR_HSIRDY)
#define RCC_HSI_IsDisable						((RCC->CR & RCC_CR_HSIRDY) == RESET)
#define RCC_HSI_Enable()						(RCC->CR |= RCC_CR_HSION)
#define RCC_HSI_Disable()						(RCC->CR &= ~RCC_CR_HSION)
#define RCC_HSI_Adjusts_SetDefault()			(RCC->CR = (RCC->CR & (~RCC_CR_HSITRIM_Msk)) | RCC_CR_HSITRIM_4)

//---- LSI Configuration

#define RCC_LSI_Tick_Timeout					1000UL
#define RCC_LSI_IsEnable						((RCC->CSR & RCC_CSR_LSIRDY) == RCC_CSR_LSIRDY)
#define RCC_LSI_IsDisable						((RCC->CSR & RCC_CSR_LSIRDY) == RESET)
#define RCC_LSI_Enable()						(RCC->CSR |= RCC_CSR_LSION)
#define RCC_LSI_Disable()						(RCC->CSR &= ~RCC_CSR_LSION)

//---- HSI14 Configuration

#define RCC_HSI14_Tick_Timeout					1000UL
#define RCC_HSI14_IsEnable						((RCC->CR2 & RCC_CR2_HSI14RDY) == RCC_CR2_HSI14RDY)
#define RCC_HSI14_IsDisable						((RCC->CR2 & RCC_CR2_HSI14RDY) == RESET)
#define RCC_HSI14_Enable()						(RCC->CR2 |= RCC_CR2_HSI14ON)
#define RCC_HSI14_Disable()						(RCC->CR2 &= ~RCC_CR2_HSI14ON)
#define RCC_HSI14_SetDefaultAdjusts()			(RCC->CR2 = (RCC->CR2 & (~RCC_CR2_HSI14TRIM_Msk)) | (0x10 << RCC_CR2_HSI14TRIM_Pos))

//---- PLL Configuration

#define RCC_PLL_Tick_Timeout					1000UL
#define RCC_PLL_IsEnable						((RCC->CR & RCC_CR_PLLRDY) == RCC_CR_PLLRDY)
#define RCC_PLL_IsDisable						((RCC->CR & RCC_CR_PLLRDY) == RESET)
#define RCC_PLL_Enable()						(RCC->CR |= RCC_CR_PLLON)
#define RCC_PLL_Disable()						(RCC->CR &= ~RCC_CR_PLLON)

#define RCC_PLL_MUL_Read()						(RCC->CFGR & RCC_CFGR_PLLMUL_Msk)
#define RCC_PLL_HSE_Prediv_Read()				(RCC->CFGR2 & RCC_CFGR2_PREDIV_Msk)
#define RCC_PLL_SourceClock_Read()				(RCC->CFGR & RCC_CFGR_PLLSRC_Msk)
#define RCC_PLL_MUL_Write(__VALUE__)			MODIFY_REG(RCC->CFGR, RCC_CFGR_PLLMUL_Msk, __VALUE__)
#define RCC_PLL_HSE_Prediv_Write(__VALUE__)		MODIFY_REG(RCC->CFGR2, RCC_CFGR2_PREDIV_Msk, __VALUE__)
#define RCC_PLL_SourceClock_Write(__VALUE__)	MODIFY_REG(RCC->CFGR, RCC_CFGR_PLLSRC_Msk, __VALUE__)

#define RCC_PLL_MUL_2							RCC_CFGR_PLLMUL2
#define RCC_PLL_MUL_3							RCC_CFGR_PLLMUL3
#define RCC_PLL_MUL_4							RCC_CFGR_PLLMUL4
#define RCC_PLL_MUL_5							RCC_CFGR_PLLMUL5
#define RCC_PLL_MUL_6							RCC_CFGR_PLLMUL6
#define RCC_PLL_MUL_7							RCC_CFGR_PLLMUL7
#define RCC_PLL_MUL_8							RCC_CFGR_PLLMUL8
#define RCC_PLL_MUL_9							RCC_CFGR_PLLMUL9
#define RCC_PLL_MUL_10							RCC_CFGR_PLLMUL10
#define RCC_PLL_MUL_11							RCC_CFGR_PLLMUL11
#define RCC_PLL_MUL_12							RCC_CFGR_PLLMUL12
#define RCC_PLL_MUL_13							RCC_CFGR_PLLMUL13
#define RCC_PLL_MUL_14							RCC_CFGR_PLLMUL14
#define RCC_PLL_MUL_15							RCC_CFGR_PLLMUL15
#define RCC_PLL_MUL_16							RCC_CFGR_PLLMUL16

#define RCC_PLL_HSE_Prediv_1					RCC_CFGR2_PREDIV_DIV1
#define RCC_PLL_HSE_Prediv_2					RCC_CFGR2_PREDIV_DIV2
#define RCC_PLL_HSE_Prediv_3					RCC_CFGR2_PREDIV_DIV3
#define RCC_PLL_HSE_Prediv_4					RCC_CFGR2_PREDIV_DIV4
#define RCC_PLL_HSE_Prediv_5					RCC_CFGR2_PREDIV_DIV5
#define RCC_PLL_HSE_Prediv_6					RCC_CFGR2_PREDIV_DIV6
#define RCC_PLL_HSE_Prediv_7					RCC_CFGR2_PREDIV_DIV7
#define RCC_PLL_HSE_Prediv_8					RCC_CFGR2_PREDIV_DIV8
#define RCC_PLL_HSE_Prediv_9					RCC_CFGR2_PREDIV_DIV9
#define RCC_PLL_HSE_Prediv_10					RCC_CFGR2_PREDIV_DIV10
#define RCC_PLL_HSE_Prediv_11					RCC_CFGR2_PREDIV_DIV11
#define RCC_PLL_HSE_Prediv_12					RCC_CFGR2_PREDIV_DIV12
#define RCC_PLL_HSE_Prediv_13					RCC_CFGR2_PREDIV_DIV13
#define RCC_PLL_HSE_Prediv_14					RCC_CFGR2_PREDIV_DIV14
#define RCC_PLL_HSE_Prediv_15					RCC_CFGR2_PREDIV_DIV15
#define RCC_PLL_HSE_Prediv_16					RCC_CFGR2_PREDIV_DIV16

#define RCC_PLL_SourceClock_HSI					RCC_CFGR_PLLSRC_HSI_DIV2
#define RCC_PLL_SourceClock_HSE					RCC_CFGR_PLLSRC_HSE_PREDIV

//---- FLASH Configuration

#define FLASH_PerfetchBuffer_Enable()			(FLASH->ACR |= FLASH_ACR_PRFTBE)
#define FLASH_PerfetchBuffer_Disable()			(FLASH->ACR &= FLASH_ACR_PRFTBE)

#define FLASH_LATENCY_ZeroWaitState()			MODIFY_REG(FLASH->ACR, 7UL, 0UL)
#define FLASH_LATENCY_OneWaitState()			MODIFY_REG(FLASH->ACR, 7UL, 1UL)

//---- HCLK Configuration

#define RCC_AHB_Prescaler_1						(0x00ULL | (0x00UL << RCC_CFGR_HPRE_Pos))
#define RCC_AHB_Prescaler_2						(0x04ULL | (0x00UL << RCC_CFGR_HPRE_Pos))
#define RCC_AHB_Prescaler_4						(0x04ULL | (0x01UL << RCC_CFGR_HPRE_Pos))
#define RCC_AHB_Prescaler_8						(0x04ULL | (0x02UL << RCC_CFGR_HPRE_Pos))
#define RCC_AHB_Prescaler_16					(0x04ULL | (0x03UL << RCC_CFGR_HPRE_Pos))
#define RCC_AHB_Prescaler_64					(0x04ULL | (0x04UL << RCC_CFGR_HPRE_Pos))
#define RCC_AHB_Prescaler_128					(0x04ULL | (0x05UL << RCC_CFGR_HPRE_Pos))
#define RCC_AHB_Prescaler_256					(0x04ULL | (0x06UL << RCC_CFGR_HPRE_Pos))
#define RCC_AHB_Prescaler_512					(0x04ULL | (0x07UL << RCC_CFGR_HPRE_Pos))

#define RCC_AHB_Prescaler_Read()				(RCC->CFGR & RCC_CFGR_HPRE_Msk)
#define RCC_AHB_Prescaler_Write(__VALUE__)		MODIFY_REG(RCC->CFGR, RCC_CFGR_HPRE_Msk, __VALUE__)

//---- SYSCLK Configuration

#define RCC_Switch_Timeout						5000UL
#define RCC_SourceClock_Read()					(RCC->CFGR & RCC_CFGR_SWS_Msk)
#define RCC_SourceClock_Write(__VALUE__)		MODIFY_REG(RCC->CFGR, RCC_CFGR_SW_Msk, (__VALUE__ >> 2))
#define RCC_SourceClock_HSI						RCC_CFGR_SWS_HSI
#define RCC_SourceClock_HSE						RCC_CFGR_SWS_HSE
#define RCC_SourceClock_PLL						RCC_CFGR_SWS_PLL

//---- PCLK1 Configuration

#define RCC_APB_Prescaler_1						(0x00ULL << RCC_CFGR_PPRE_Pos)
#define RCC_APB_Prescaler_2						(0x04ULL << RCC_CFGR_PPRE_Pos)
#define RCC_APB_Prescaler_4						(0x05ULL << RCC_CFGR_PPRE_Pos)
#define RCC_APB_Prescaler_8						(0x06ULL << RCC_CFGR_PPRE_Pos)
#define RCC_APB_Prescaler_16					(0x07ULL << RCC_CFGR_PPRE_Pos)

#define RCC_APB_Prescaler_Read()				(RCC->CFGR & RCC_CFGR_PPRE_Msk)
#define RCC_APB_Prescaler_Write(__VALUE__)		MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE_Msk, __VALUE__)

/* --------------------------------------------------------------------------------------------------------------------------------------- */
/* Public -------------------------------------------------------------------------------------------------------------------------------- */
/* ....................................................................................................................................... */

uint32_t SystemCoreClock = HSI_VALUE;

const uint8_t APBPrescTable[8] = { 0, 0, 0, 0, 1, 2, 3, 4 };
const uint8_t AHBPrescTable[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8, 9 };

uint32_t RCC_PCLK1_freq_get(void);

/* --------------------------------------------------------------------------------------------------------------------------------------- */
/* Private ------------------------------------------------------------------------------------------------------------------------------- */
/* ....................................................................................................................................... */

static void clock_config(void);
static void clock_reset(void);
static void clock_update(void);

/* --------------------------------------------------------------------------------------------------------------------------------------- */
/* Public Implementation ----------------------------------------------------------------------------------------------------------------- */
/* ....................................................................................................................................... */

//###################################################################################################################
void SystemInit(void)
{
	clock_update();	

	__enable_irq();

	while (SystemCoreClock != STRIVE_FOR_FREQ) 
	{
		clock_reset();
		clock_config();
		clock_update();
	}
}

//###################################################################################################################
uint32_t RCC_PCLK1_freq_get(void)
{
	/* Get HCLK source and Compute PCLK1 frequency ---------------------------*/
	return (SystemCoreClock >> APBPrescTable[(RCC->CFGR & RCC_CFGR_PPRE) >> 8U]);
}

uint32_t HAL_GetTick(void);
void HAL_Delay(uint32_t Delay);

uint32_t xHAL_Get_mSec(void);
void xHAL_Delay_mSec(uint32_t Delay);
bool xHAL_is_time_has_passed_mSec(uint32_t* START, uint32_t DELAY);

uint32_t xHAL_Get_mkSec(void);
void xHAL_Delay_mkSec(uint32_t Delay);
bool xHAL_is_time_has_passed_mkSec(uint32_t* START, uint32_t DELAY);

//###################################################################################################################
uint32_t HAL_GetTick(void)
{
	return xHAL_Get_mSec();
}

//###################################################################################################################
void HAL_Delay(uint32_t Delay)
{
	uint32_t tickstart = xHAL_Get_mSec();
	uint32_t wait = Delay;

	/* Add a freq to guarantee minimum wait */
	if (wait < HAL_MAX_DELAY)
	{
		wait += (uint32_t)(uwTickFreq);
	}

	while ((xHAL_Get_mSec() - tickstart) < wait)
	{
		xHAL_sleep();
	}
}

//###################################################################################################################
uint32_t xHAL_Get_mSec(void)
{
	return SYSTEM_Tick_Prescale_MS(uwTick);
}

//###################################################################################################################
void xHAL_Delay_mSec(uint32_t Delay)
{
	uint32_t tickstart = xHAL_Get_mSec();
	uint32_t wait = Delay;

	/* Add a freq to guarantee minimum wait */
	if (wait < HAL_MAX_DELAY)
	{
		wait += (uint32_t)(uwTickFreq);
	}

	while ((xHAL_Get_mSec() - tickstart) < wait)
	{
		xHAL_sleep();
	}
}

//###################################################################################################################
bool xHAL_is_time_has_passed_mSec(uint32_t* START, uint32_t DELAY)
{
	if (*START == 0UL)
		*START = xHAL_Get_mSec();

	if (xHAL_Get_mSec() - *START >= DELAY)
	{
		*START = 0UL;
		return true;
	}
	return false;
}

//###################################################################################################################
uint32_t xHAL_Get_mkSec(void)
{
	return uwTick;
}

//###################################################################################################################
void xHAL_Delay_mkSec(uint32_t Delay)
{
	uint32_t tickstart = xHAL_Get_mkSec();
	uint32_t wait = Delay;

	/* Add a freq to guarantee minimum wait */
	if (wait < HAL_MAX_DELAY)
	{
		wait += (uint32_t)(uwTickFreq);
	}

	while ((xHAL_Get_mkSec() - tickstart) < wait)
	{
		xHAL_sleep();
	}
}

//###################################################################################################################
bool xHAL_is_time_has_passed_mkSec(uint32_t* START, uint32_t DELAY)
{
	if (*START == 0UL)
		*START = xHAL_Get_mkSec();

	if (xHAL_Get_mkSec() - *START >= DELAY)
	{
		*START = 0UL;
		return true;
	}
	return false;
}

/* --------------------------------------------------------------------------------------------------------------------------------------- */
/* Private Implementation ---------------------------------------------------------------------------------------------------------------- */
/* ....................................................................................................................................... */

//###################################################################################################################
static void clock_config(void)
{
	uint32_t tick = 0UL;
	
	SET_BIT(RCC->APB2ENR, RCC_APB2ENR_SYSCFGEN);	//__HAL_RCC_SYSCFG_CLK_ENABLE();
	SET_BIT(RCC->APB1ENR, RCC_APB1ENR_PWREN);		//__HAL_RCC_PWR_CLK_ENABLE();

	/*------------------------------- HSE Configuration ------------------------*/
	{
		if (RCC_HSE_IsDisable)
			RCC_HSE_Enable();

		while (RCC_HSE_IsDisable)
			if (xHAL_is_time_has_passed_mkSec(&tick, RCC_HSE_Tick_Timeout))
				xHAL_hardware_reset();
	}
	/*----------------------------- HSI Configuration --------------------------*/
	{
		if (RCC_HSI_IsDisable)
			RCC_HSI_Enable();
		
		while (RCC_HSI_IsDisable)
			if (xHAL_is_time_has_passed_mkSec(&tick, RCC_HSI_Tick_Timeout))
				xHAL_hardware_reset();

		RCC_HSI_Adjusts_SetDefault();
	}
	/*------------------------------ LSI Configuration -------------------------*/
	{
		if (RCC_LSI_IsDisable)
			RCC_LSI_Enable();
		
		while (RCC_LSI_IsDisable)
			if (xHAL_is_time_has_passed_mkSec(&tick, RCC_LSI_Tick_Timeout))
				xHAL_hardware_reset();
	}
	/*----------------------------- HSI14 Configuration --------------------------*/
	{
		if (RCC_HSI14_IsDisable)
			RCC_HSI14_Enable();

		while (RCC_HSI14_IsDisable)
			if (xHAL_is_time_has_passed_mkSec(&tick, RCC_HSI14_Tick_Timeout))
				xHAL_hardware_reset();

		RCC_HSI14_SetDefaultAdjusts();
	}
	/*-------------------------------- PLL Configuration -----------------------*/
	{
		if (RCC_PLL_IsDisable ||
			(RCC_PLL_MUL_Read() != RCC_PLL_MUL_3) ||
			(RCC_PLL_HSE_Prediv_Read() != RCC_PLL_HSE_Prediv_1) ||
			(RCC_PLL_SourceClock_Read() != RCC_PLL_SourceClock_HSE))
		{
			if ((RCC_SourceClock_Read() == RCC_SourceClock_PLL))
			{
				RCC_SourceClock_Write(RCC_SourceClock_HSI);

				while (RCC_SourceClock_Read() != RCC_SourceClock_HSI)
					if (xHAL_is_time_has_passed_mkSec(&tick, RCC_Switch_Timeout))
						xHAL_hardware_reset();
			}

			RCC_PLL_Disable();

			while (RCC_PLL_IsEnable)
				if (xHAL_is_time_has_passed_mkSec(&tick, RCC_PLL_Tick_Timeout))
					xHAL_hardware_reset();

			/* Configure the main PLL clock source, predivider and multiplication factor. */
			RCC_PLL_MUL_Write(RCC_PLL_MUL_3);
			RCC_PLL_HSE_Prediv_Write(RCC_PLL_HSE_Prediv_1);
			RCC_PLL_SourceClock_Write(RCC_PLL_SourceClock_HSE);

			RCC_PLL_Enable();

			while (RCC_PLL_IsDisable)
				if (xHAL_is_time_has_passed_mkSec(&tick, RCC_PLL_Tick_Timeout))
					break;
		}
	}
	/*-------------------------- FLASH Configuration --------------------------*/
	{		
		FLASH_PerfetchBuffer_Enable();		
		FLASH_LATENCY_OneWaitState();		
	}
	/*-------------------------- HCLK Configuration --------------------------*/
	{
		RCC_APB_Prescaler_Write(RCC_APB_Prescaler_16);				
		RCC_AHB_Prescaler_Write(RCC_AHB_Prescaler_1);
	}
	/*------------------------- SYSCLK Configuration ---------------------------*/
	{
		if (RCC_PLL_IsDisable)
			xHAL_hardware_reset();

		RCC_SourceClock_Write(RCC_SourceClock_PLL);

		while (RCC_SourceClock_Read() != RCC_SourceClock_PLL)
			if (xHAL_is_time_has_passed_mkSec(&tick, RCC_Switch_Timeout))
				xHAL_hardware_reset();
	}
	/*-------------------------- PCLK1 Configuration ---------------------------*/
	{
		RCC_APB_Prescaler_Write(RCC_APB_Prescaler_1);
	}

	/* Update the SystemCoreClock global variable */
	clock_update();

	MODIFY_REG(RCC->CFGR3, RCC_CFGR3_USART1SW_Msk, RCC_CFGR3_USART1SW_PCLK);
	MODIFY_REG(RCC->CFGR3, RCC_CFGR3_I2C1SW_Msk, RCC_CFGR3_I2C1SW_HSI);
}

//###################################################################################################################
static void clock_reset(void)
{
	/* Reset the RCC clock configuration to the default reset state ------------*/
	/* Set HSION bit */
	RCC->CR |= (uint32_t)0x00000001U;

	/* Reset SW[1:0], HPRE[3:0], PPRE[2:0], ADCPRE, MCOSEL[2:0], MCOPRE[2:0] and PLLNODIV bits */
	RCC->CFGR &= (uint32_t)0x08FFB80CU;

	/* Reset HSEON, CSSON and PLLON bits */
	RCC->CR &= (uint32_t)0xFEF6FFFFU;

	/* Reset HSEBYP bit */
	RCC->CR &= (uint32_t)0xFFFBFFFFU;

	/* Reset PLLSRC, PLLXTPRE and PLLMUL[3:0] bits */
	RCC->CFGR &= (uint32_t)0xFFC0FFFFU;

	/* Reset PREDIV[3:0] bits */
	RCC->CFGR2 &= (uint32_t)0xFFFFFFF0U;

	/* Reset USART1SW[1:0], I2C1SW and ADCSW bits */
	RCC->CFGR3 &= (uint32_t)0xFFFFFEECU;

	/* Reset HSI14 bit */
	RCC->CR2 &= (uint32_t)0xFFFFFFFEU;

	/* Disable all interrupts */
	RCC->CIR = 0x00000000U;
}

//###################################################################################################################
static void clock_update(void)
{
	uint32_t SysFreq;

	switch (RCC_SourceClock_Read())
	{
	case RCC_SourceClock_HSE: {
		SysFreq = HSE_VALUE;
		break;
	}
	case RCC_SourceClock_HSI: {
		SysFreq = HSI_VALUE;
		break;
	}
	default: {
		const uint8_t PLL_MUL_FactorTable[16] = { 2U,  3U,  4U,  5U,  6U,  7U,  8U,  9U, 10U, 11U, 12U, 13U, 14U, 15U, 16U, 16U };
		const uint8_t PLL_Prediv_FactorTable[16] = { 1U, 2U,  3U,  4U,  5U,  6U,  7U,  8U, 9U,10U, 11U, 12U, 13U, 14U, 15U, 16U };

		uint32_t PLL_MUL = PLL_MUL_FactorTable[RCC_PLL_MUL_Read() >> RCC_CFGR_PLLMUL_Pos];
		uint32_t PLL_Prediv = PLL_Prediv_FactorTable[RCC_PLL_HSE_Prediv_Read() >> RCC_CFGR2_PREDIV_Pos];

		SysFreq =
			(RCC_PLL_SourceClock_Read() == RCC_PLL_SourceClock_HSE) ?
			(uint32_t)((uint64_t)HSE_VALUE / (uint64_t)(PLL_Prediv)) * ((uint64_t)PLL_MUL) :
			(uint32_t)((uint64_t)(HSI_VALUE >> 1U)* ((uint64_t)PLL_MUL));
	}
	}	

	SystemCoreClock = SysFreq >> AHBPrescTable[RCC_AHB_Prescaler_Read() >> RCC_CFGR_HPRE_Pos];		

	SysTick_Config(SystemCoreClock / SYSTEM_Tick_Value);	
}

/* --------------------------------------------------------------------------------------------------------------------------------------- */
/* Interrupt ----------------------------------------------------------------------------------------------------------------------------- */
/* ....................................................................................................................................... */

/* End ----------------------------------------------------------------------------------------------------------------------------------- */
/* ....................................................................................................................................... */
