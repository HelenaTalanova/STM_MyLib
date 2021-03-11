// #########################################################################################################
// * Includes ____________________________________________________________________________________________

#include "custom.h"

extern void Reset_Handler(void);

void Setup(void);
void Loop(void);

void iHAL_IWDG_Start(void);
void iHAL_IWDG_Refresh(void);

// #########################################################################################################
// * Private: implementation ______________________________________________________________________________

static void NVIC_SetVectorTable(void);
static void SetReadProtected(void);
static void iHAL_sleep(void);

// #########################################################################################################
// * Public: implementation ______________________________________________________________________________

bool FIRST_START = false;

/// **************************************************************************************
int main(void)
{
#if !(USE_BOOT || DEBUG)
    iHAL_IWDG_Start();
#elif (USE_BOOT) && (!DEBUG)
    iHAL_IWDG_Refresh();
#endif // !(USE_BOOT) && !(DEBUG)	    

#if !(USE_BOOT || DEBUG)
    SetReadProtected();
#endif // !(USE_BOOT)    

    NVIC_SetVectorTable();

    Setup();

#if (!DEBUG)
    iHAL_IWDG_Refresh();
#endif // !(DEBUG)  

    for (;;)
    {
        Loop();

#if (!DEBUG)                
        iHAL_IWDG_Refresh();
        iHAL_sleep();
        iHAL_IWDG_Refresh();
#endif // !(DEBUG)  
    }

    xHAL_hardware_reset();
}

void xHAL_sleep(void)
{
    SCB->SCR &= ~(SCB_SCR_SLEEPONEXIT_Msk | SCB_SCR_SLEEPDEEP_Msk);
    __WFI();
}

void xHAL_hardware_reset(void)
{
    __NVIC_SystemReset();
}

void xHAL_software_reset(void)
{
    extern void* g_pfnVectors[0x79];

    __disable_irq();

    __IO uint32_t sp = (uint32_t)g_pfnVectors[0];

    __set_MSP(sp);
    Reset_Handler();
}

bool IsDelay(uint32_t* timer, uint32_t delay)
{
    if (HAL_GetTick() - *timer >= delay)
    {
        *timer = 0;
    }

    return (*timer == 0);
}

static void SetReadProtected(void)
{
    FLASH_OBProgramInitTypeDef OBInit;
    HAL_FLASHEx_OBGetConfig((FLASH_OBProgramInitTypeDef*)&OBInit);

    if (OBInit.RDPLevel != OB_RDP_LEVEL_1)
    {
        FIRST_START = true;

        FLASH_OBProgramInitTypeDef f;
        f.OptionType = OPTIONBYTE_RDP;
        f.RDPLevel = OB_RDP_LEVEL_1;

        HAL_FLASH_Unlock(); // unlock Flash
        HAL_FLASH_OB_Unlock(); // unlock Optionbytes
        HAL_FLASHEx_OBProgram(&f);
        HAL_FLASH_OB_Launch(); // write OB to Flash and reset
        HAL_FLASH_OB_Lock(); // Lock Optionbytes
        HAL_FLASH_Lock(); // lock Flash
    }
}

static void iHAL_sleep(void)
{
    SCB->SCR &= ~(SCB_SCR_SLEEPONEXIT_Msk | SCB_SCR_SLEEPDEEP_Msk);
    __WFI();
}

static void NVIC_SetVectorTable(void)
{
    extern void* g_pfnVectors[0x79];
    SCB->VTOR = (uint32_t)g_pfnVectors;
}

/// #########################################################################################################
/// * End file interrupt.c                                                                            #####
/// #####################################################################################################
