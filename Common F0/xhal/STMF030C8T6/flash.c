/* --------------------------------------------------------------------------------------------------------------------------------------- */
/* Includes ------------------------------------------------------------------------------------------------------------------------------ */
/* ....................................................................................................................................... */

#include "flash.h"

/* --------------------------------------------------------------------------------------------------------------------------------------- */
/* Defines ------------------------------------------------------------------------------------------------------------------------------- */
/* ....................................................................................................................................... */

#define FLASH_Timeout									5000U	/* 50 s */

#define FLASH_IS_Busy									((FLASH->SR & FLASH_SR_BSY) == FLASH_SR_BSY)
#define FLASH_IS_ErrorProgramming						((FLASH->SR & FLASH_SR_PGERR) == FLASH_SR_PGERR)
#define FLASH_IS_ErrorWriteProtection					((FLASH->SR & FLASH_SR_WRPRTERR) == FLASH_SR_WRPRTERR)
#define FLASH_IS_EndOperation							((FLASH->SR & FLASH_SR_EOP) == FLASH_SR_EOP)

#define FLASH_CLEAR_ErrorProgramming()					(FLASH->SR |= FLASH_SR_PGERR)
#define FLASH_CLEAR_ErrorWriteProtection()				(FLASH->SR |= FLASH_SR_WRPRTERR)
#define FLASH_CLEAR_EndOperation()						(FLASH->SR |= FLASH_SR_EOP)

#define FLASH_IS_Lock									((FLASH->CR & FLASH_CR_LOCK) == FLASH_CR_LOCK)
#define FLASH_IS_UnLock									((FLASH->CR & FLASH_CR_LOCK) == RESET)
#define __FLASH_Lock()									(FLASH->CR |= FLASH_CR_LOCK)

#define FLASH_IS_OptionByte_Lock						((FLASH->CR & FLASH_CR_OPTWRE) == RESET)
#define FLASH_IS_OptionByte_UnLock						((FLASH->CR & FLASH_CR_OPTWRE) == FLASH_CR_OPTWRE)
#define FLASH_OptionByte_Lock()							(FLASH->CR &= ~(FLASH_CR_OPTWRE))

#define FLASH_Programming_Enable()						(FLASH->CR |= FLASH_CR_PG)
#define FLASH_ProgrammingOptionByte_Enable()			(FLASH->CR |= FLASH_CR_OPTPG)
#define FLASH_EraseFull_Enable()						(FLASH->CR |= FLASH_CR_MER)
#define FLASH_ErasePage_Enable()						(FLASH->CR |= FLASH_CR_PER)
#define FLASH_EraseOptionByte_Enable()					(FLASH->CR |= FLASH_CR_OPTER)
#define FLASH_EraseStart()								(FLASH->CR |= FLASH_CR_STRT)
#define FLASH_ErrorInterrupt_Enable()					(FLASH->CR |= FLASH_CR_ERRIE)
#define FLASH_EndOperationInterrupt_Enable()			(FLASH->CR |= FLASH_CR_EOPIE)

#define FLASH_Programming_Disable()						(FLASH->CR &= ~(FLASH_CR_PG))
#define FLASH_ProgrammingOptionByte_Disable()			(FLASH->CR &= ~(FLASH_CR_OPTPG))
#define FLASH_EraseFull_Disable()						(FLASH->CR &= ~(FLASH_CR_MER))
#define FLASH_ErasePage_Disable()						(FLASH->CR &= ~(FLASH_CR_PER))
#define FLASH_EraseOptionByte_Disable()					(FLASH->CR &= ~(FLASH_CR_OPTER))
#define FLASH_ErrorInterrupt_Disable()					(FLASH->CR &= ~(FLASH_CR_ERRIE))
#define FLASH_EndOperationInterrupt_Disable()			(FLASH->CR &= ~(FLASH_CR_EOPIE))

/* --------------------------------------------------------------------------------------------------------------------------------------- */
/* Private ------------------------------------------------------------------------------------------------------------------------------- */
/* ....................................................................................................................................... */

static bool FLASH_OB_Unlock(void);
static bool FLASH_Wait(void);
static void FLASH_OB_RDP_LevelConfig(X_FLASH_OB_RDP_LEVEL_e level);
static X_FLASH_OB_RDP_LEVEL_e FLASH_OB_RDP_Read(void);

/* --------------------------------------------------------------------------------------------------------------------------------------- */
/* Public Implementation ----------------------------------------------------------------------------------------------------------------- */
/* ....................................................................................................................................... */

void xHAL_FLASH_Erase(uint32_t address)
{
	while (FLASH_IS_Busy) ;
	
	if (FLASH_IS_EndOperation) 
		FLASH_CLEAR_EndOperation();
	

	FLASH_ErasePage_Enable();
	FLASH->AR = address;
	FLASH_EraseStart();
	
	while (!(FLASH_IS_EndOperation)) ;
	
	FLASH_CLEAR_EndOperation();
	FLASH_ErasePage_Disable();
}

void xHAL_FLASH_Write(uint8_t* data, uint32_t address, uint32_t count)
{	
	__IO uint16_t* pFLASH = (uint16_t*)address;	
	
	while (FLASH_IS_Busy) ;
	
	if (FLASH_IS_EndOperation)	
		FLASH_CLEAR_EndOperation();	

	FLASH_Programming_Enable();
	
	for (uint32_t i = 0; i < count; i += 2)
	{
		*pFLASH++ = (data[i + 1] << 8) + data[i];

		if (FLASH_IS_ErrorWriteProtection || FLASH_IS_ErrorProgramming)
		{
			FLASH_CLEAR_ErrorWriteProtection();
			FLASH_CLEAR_ErrorProgramming();
			FLASH_CLEAR_EndOperation();
			continue;
		}

		while (!(FLASH_IS_EndOperation));
		FLASH_CLEAR_EndOperation();
	}

	FLASH_Programming_Disable();
}

//###################################################################################################################
void xHAL_FLASH_RDP_Write(X_FLASH_OB_RDP_LEVEL_e level)
{
	if (FLASH_OB_RDP_Read() != level)
	{				
		xHAL_FLASH_Unlock();
		FLASH_OB_Unlock();		
		FLASH_OB_RDP_LevelConfig(level);		
			
		FLASH_OptionByte_Lock();
		__FLASH_Lock();						
	}
}

//###################################################################################################################
bool xHAL_FLASH_Unlock(void)
{
	if (FLASH_IS_Lock)
	{
		WRITE_REG(FLASH->KEYR, FLASH_KEY1);
		WRITE_REG(FLASH->KEYR, FLASH_KEY2);
	}
	return FLASH_IS_UnLock;
}

//###################################################################################################################
void xHAL_FLASH_Lock(void)
{
	__FLASH_Lock();
}

//###################################################################################################################
bool xHAL_FLASH_IsAddress(uint32_t address)
{
	return (FLASH_ADDR + FLASH_OFFSET <= address) && (address <= FLASH_ADDR + FLASH_SIZE);
}

/* --------------------------------------------------------------------------------------------------------------------------------------- */
/* Private Implementation ---------------------------------------------------------------------------------------------------------------- */
/* ....................................................................................................................................... */

//###################################################################################################################
static bool FLASH_OB_Unlock(void)
{
	if (FLASH_IS_OptionByte_Lock)
	{
		WRITE_REG(FLASH->OPTKEYR, FLASH_OPTKEY1);
		WRITE_REG(FLASH->OPTKEYR, FLASH_OPTKEY2);
	}
	return FLASH_IS_OptionByte_UnLock;
}

//###################################################################################################################
static bool FLASH_Wait(void)
{
	uint32_t tickstart = HAL_GetTick();

	while (FLASH_IS_Busy)
		if (tickstart + FLASH_Timeout < HAL_GetTick())
			return false;

	if (FLASH_IS_EndOperation)
		FLASH_CLEAR_EndOperation();

	if (FLASH_IS_ErrorWriteProtection || FLASH_IS_ErrorProgramming)
	{
		FLASH_CLEAR_ErrorWriteProtection();
		FLASH_CLEAR_ErrorProgramming();
		return false;
	}

	return true;
}

//###################################################################################################################
static void FLASH_OB_RDP_LevelConfig(X_FLASH_OB_RDP_LEVEL_e level)
{
	if (FLASH_Wait())
	{
		FLASH_EraseOptionByte_Enable();
		FLASH_EraseStart();

		bool status = FLASH_Wait();
		FLASH_EraseOptionByte_Disable();

		if (status)
		{
			FLASH_ProgrammingOptionByte_Enable();
			WRITE_REG(OB->RDP, level);
			FLASH_Wait();
			FLASH_ProgrammingOptionByte_Disable();
		}
	}
}

//###################################################################################################################
static X_FLASH_OB_RDP_LEVEL_e FLASH_OB_RDP_Read(void)
{
	uint32_t tmp = READ_BIT(FLASH->OBR, (FLASH_OBR_RDPRT1 | FLASH_OBR_RDPRT2));

	return 
		(tmp == 0) ? X_FLASH_OB_RDP_LEVEL_0 :
		(tmp & FLASH_OBR_RDPRT2) == FLASH_OBR_RDPRT2 ? X_FLASH_OB_RDP_LEVEL_2 :
		X_FLASH_OB_RDP_LEVEL_1;
}

/* --------------------------------------------------------------------------------------------------------------------------------------- */
/* Interrupt ----------------------------------------------------------------------------------------------------------------------------- */
/* ....................................................................................................................................... */

/* End ----------------------------------------------------------------------------------------------------------------------------------- */
/* ....................................................................................................................................... */
