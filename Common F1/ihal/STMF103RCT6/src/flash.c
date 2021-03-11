// #########################################################################################################
// * Includes ____________________________________________________________________________________________

#include "custom.h"

#define TIMEOUT_OPERATION_MS    100

#define FLASH_CLEAR_END_OPERATION()     FLASH->SR = FLASH_SR_EOP
#define FLASH_IS_END_OPERATION          ((FLASH->SR & FLASH_SR_EOP) == FLASH_SR_EOP)
#define FLASH_IS_BUSY                   ((FLASH->SR & FLASH_SR_BSY) == FLASH_SR_BSY)
#define FLASH_WRITE_PROTECTION_ERROR    ((FLASH->SR & FLASH_SR_WRPRTERR) == FLASH_SR_WRPRTERR)
#define FLASH_PROGRAMMING_ERROR         ((FLASH->SR & FLASH_SR_PGERR) == FLASH_SR_PGERR)
#define FLASH_CLEAR_ERROR()             FLASH->SR = (FLASH_SR_WRPRTERR || FLASH_SR_PGERR)

// #########################################################################################################
// * Private: ____________________________________________________________________________________________

static bool iFLASH_wait_end_operation(void);

// #########################################################################################################
// * Public: implementation ______________________________________________________________________________

bool iFLASH_unlock(void)
{
    /* Authorize the FLASH Registers access */
    WRITE_REG(FLASH->KEYR, FLASH_KEY1);
    WRITE_REG(FLASH->KEYR, FLASH_KEY2);

    /* Verify Flash is unlocked */
    if (READ_BIT(FLASH->CR, FLASH_CR_LOCK) != RESET)
        NVIC_SystemReset();

    return true;// (READ_BIT(FLASH->CR, FLASH_CR_LOCK) == RESET);
}

/// **************************************************************************************
bool iFLASH_erase(uint32_t addr)
{
    if (iFLASH_wait_end_operation())
    {
        SET_BIT(FLASH->CR, FLASH_CR_PER);
        FLASH->AR = addr;
        SET_BIT(FLASH->CR, FLASH_CR_STRT);

        iFLASH_wait_end_operation();
        FLASH->CR = 0;

        return true;
    }
    return false;
}

/// **************************************************************************************
bool iFLASH_write(uint32_t addr, uint8_t* data, uint32_t size)
{
    volatile uint16_t* pFLASH = (uint16_t*)addr;
    uint32_t index = 0;
    uint32_t count  = 0;
    uint16_t tmp = 0;

    while (iFLASH_wait_end_operation() && (count < size))
    {
        SET_BIT(FLASH->CR, FLASH_CR_PG);

        tmp = data[index++];
        tmp |= data[index++] << 8;

        *pFLASH++ = tmp;

        count += 2;
    }

    return !(count < size);
}

// #########################################################################################################
// * Private: implementation ______________________________________________________________________________

/// **************************************************************************************
static bool iFLASH_wait_end_operation(void)
{
    /* Wait for the FLASH operation to complete by polling on BUSY flag to be reset.
   Even if the FLASH operation fails, the BUSY flag will be reset and an error
   flag will be set */
    uint32_t tick = HAL_GetTick();
    bool result = true;

    while (FLASH_IS_BUSY)
    {
        if ((HAL_GetTick() - tick) > TIMEOUT_OPERATION_MS)
        {
            result = false;
            break;
        }
    }

    /* Check FLASH End of Operation flag  */
    if (FLASH_IS_END_OPERATION)
    {
        /* Clear FLASH End of Operation pending bit */
        FLASH_CLEAR_END_OPERATION();
    }

    if (FLASH_PROGRAMMING_ERROR || FLASH_WRITE_PROTECTION_ERROR)
    {
        FLASH_CLEAR_ERROR();
        result = false;
    }

    FLASH->CR = 0;

    return result;
}

/// #########################################################################################################
/// * End file flash.c                                                                                #####
/// #####################################################################################################
