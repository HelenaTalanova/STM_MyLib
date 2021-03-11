/*
	This file contains the entry point (Reset_Handler) of your firmware project.
	The reset handled initializes the RAM and calls system library initializers as well as
	the platform-specific initializer and the main() function.
*/

#include <stddef.h>
extern void *_estack;

void Reset_Handler();
void Default_Handler();

#ifndef DEBUG_DEFAULT_INTERRUPT_HANDLERS

#ifdef DEBUG
#define DEBUG_DEFAULT_INTERRUPT_HANDLERS 1
#else
#define DEBUG_DEFAULT_INTERRUPT_HANDLERS 0
#endif

#endif

#if DEBUG_DEFAULT_INTERRUPT_HANDLERS
void __attribute__ ((weak, naked)) NMI_Handler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void NMI_Handler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) HardFault_Handler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void HardFault_Handler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) SVC_Handler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void SVC_Handler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) PendSV_Handler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void PendSV_Handler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) SysTick_Handler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void SysTick_Handler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) WWDG_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void WWDG_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) PVD_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void PVD_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) RTC_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void RTC_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) FLASH_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void FLASH_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) RCC_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void RCC_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) EXTI0_1_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void EXTI0_1_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) EXTI2_3_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void EXTI2_3_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) EXTI4_15_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void EXTI4_15_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) DMA1_Channel1_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void DMA1_Channel1_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) DMA1_Channel2_3_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void DMA1_Channel2_3_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) DMA1_Channel4_5_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void DMA1_Channel4_5_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) ADC1_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void ADC1_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) TIM1_BRK_UP_TRG_COM_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void TIM1_BRK_UP_TRG_COM_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) TIM1_CC_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void TIM1_CC_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) TIM2_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void TIM2_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) TIM3_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void TIM3_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) TIM14_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void TIM14_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) TIM16_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void TIM16_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) TIM17_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void TIM17_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) I2C1_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void I2C1_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) SPI1_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void SPI1_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}
void __attribute__ ((weak, naked)) USART1_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void USART1_IRQHandler();
	__asm("bkpt 255");
	__asm("bx lr");
}

#else
void NMI_Handler()                     __attribute__ ((weak, alias ("Default_Handler")));
void HardFault_Handler()               __attribute__ ((weak, alias ("Default_Handler")));
void SVC_Handler()                     __attribute__ ((weak, alias ("Default_Handler")));
void PendSV_Handler()                  __attribute__ ((weak, alias ("Default_Handler")));
void SysTick_Handler()                 __attribute__ ((weak, alias ("Default_Handler")));
void WWDG_IRQHandler()                 __attribute__ ((weak, alias ("Default_Handler")));
void PVD_IRQHandler()                  __attribute__ ((weak, alias ("Default_Handler")));
void RTC_IRQHandler()                  __attribute__ ((weak, alias ("Default_Handler")));
void FLASH_IRQHandler()                __attribute__ ((weak, alias ("Default_Handler")));
void RCC_IRQHandler()                  __attribute__ ((weak, alias ("Default_Handler")));
void EXTI0_1_IRQHandler()              __attribute__ ((weak, alias ("Default_Handler")));
void EXTI2_3_IRQHandler()              __attribute__ ((weak, alias ("Default_Handler")));
void EXTI4_15_IRQHandler()             __attribute__ ((weak, alias ("Default_Handler")));
void DMA1_Channel1_IRQHandler()        __attribute__ ((weak, alias ("Default_Handler")));
void DMA1_Channel2_3_IRQHandler()      __attribute__ ((weak, alias ("Default_Handler")));
void DMA1_Channel4_5_IRQHandler()      __attribute__ ((weak, alias ("Default_Handler")));
void ADC1_IRQHandler()                 __attribute__ ((weak, alias ("Default_Handler")));
void TIM1_BRK_UP_TRG_COM_IRQHandler()  __attribute__ ((weak, alias ("Default_Handler")));
void TIM1_CC_IRQHandler()              __attribute__ ((weak, alias ("Default_Handler")));
void TIM2_IRQHandler()                 __attribute__ ((weak, alias ("Default_Handler")));
void TIM3_IRQHandler()                 __attribute__ ((weak, alias ("Default_Handler")));
void TIM14_IRQHandler()                __attribute__ ((weak, alias ("Default_Handler")));
void TIM16_IRQHandler()                __attribute__ ((weak, alias ("Default_Handler")));
void TIM17_IRQHandler()                __attribute__ ((weak, alias ("Default_Handler")));
void I2C1_IRQHandler()                 __attribute__ ((weak, alias ("Default_Handler")));
void SPI1_IRQHandler()                 __attribute__ ((weak, alias ("Default_Handler")));
void USART1_IRQHandler()               __attribute__ ((weak, alias ("Default_Handler")));
#endif

void * g_pfnVectors[0x30] __attribute__ ((section (".isr_vector"), used)) = 
{
	&_estack,
	&Reset_Handler,
	&NMI_Handler,
	&HardFault_Handler,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	&SVC_Handler,
	NULL,
	NULL,
	&PendSV_Handler,
	&SysTick_Handler,
	&WWDG_IRQHandler,
	&PVD_IRQHandler,
	&RTC_IRQHandler,
	&FLASH_IRQHandler,
	&RCC_IRQHandler,
	&EXTI0_1_IRQHandler,
	&EXTI2_3_IRQHandler,
	&EXTI4_15_IRQHandler,
	NULL,
	&DMA1_Channel1_IRQHandler,
	&DMA1_Channel2_3_IRQHandler,
	&DMA1_Channel4_5_IRQHandler,
	&ADC1_IRQHandler,
	&TIM1_BRK_UP_TRG_COM_IRQHandler,
	&TIM1_CC_IRQHandler,
	&TIM2_IRQHandler,
	&TIM3_IRQHandler,
	NULL,
	NULL,
	&TIM14_IRQHandler,
	NULL,
	&TIM16_IRQHandler,
	&TIM17_IRQHandler,
	&I2C1_IRQHandler,
	NULL,
	&SPI1_IRQHandler,
	NULL,
	&USART1_IRQHandler,
	NULL,
	NULL,
	NULL,
	NULL,
};

void SystemInit();
void __libc_init_array();
int main();

extern void *_sidata, *_sdata, *_edata;
extern void *_sbss, *_ebss;

void __attribute__((naked, noreturn)) Reset_Handler()
{
	//Normally the CPU should will setup the based on the value from the first entry in the vector table.
	//If you encounter problems with accessing stack variables during initialization, ensure the line below is enabled.
	#if defined(sram_layout) || defined(INITIALIZE_SP_AT_RESET)
	__asm ("ldr sp, =_estack");
	#endif

	void **pSource, **pDest;
	for (pSource = &_sidata, pDest = &_sdata; pDest != &_edata; pSource++, pDest++)
		*pDest = *pSource;

	for (pDest = &_sbss; pDest != &_ebss; pDest++)
		*pDest = 0;

	SystemInit();
	__libc_init_array();
	(void)main();
	for (;;) ;
}

void __attribute__((naked, noreturn)) Default_Handler()
{
	//If you get stuck here, your code is missing a handler for some interrupt.
	//Define a 'DEBUG_DEFAULT_INTERRUPT_HANDLERS' macro via VisualGDB Project Properties and rebuild your project.
	//This will pinpoint a specific missing vector.
	for (;;) ;
}
