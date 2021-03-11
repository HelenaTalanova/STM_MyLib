// #########################################################################################################
// Define to prevent recursive inclusion _________________________________________________________________

#ifndef __TIM_H
#define __TIM_H

#include <stdint.h>
#include <stdbool.h>
#include "custom.h"

#ifdef __cplusplus
extern "C" {
#endif 

#define __TIMx(TIMx)										((TIM_TypeDef*)TIMx)
#define __REG_WRITE(REG, VALUE)								(*((uint32_t*)REG) = VALUE)

	/** @brief  Enable tim counter.
	*	reg CR1 set CEN
	*/
#define iHAL_TIM_ENABLE(TIMx)						            __TIMx(TIMx)->CR1 |= TIM_CR1_CEN

	/** @brief  Output compare channel disable
	*   reg CCER clear CCxE
	*/
#define iHAL_TIM_CHANNEL_DISABLE(TIMx, CH)	                    __TIMx(TIMx)->CCER &= ~(TIM_CCER_CC1E << CH)

	/** @brief  Output compare channel enable
	*   reg CCER set CCxE
	*/
#define iHAL_TIM_CHANNEL_ENABLE(TIMx, CH)	                    __TIMx(TIMx)->CCER |= (TIM_CCER_CC1E << CH)

	/** @brief Write value output compare channel
	*   reg CCEx write VALUE
	*/
#define iHAL_TIM_WRITE_COMPARE_VALUE(TIMx, CH, VALUE)			__REG_WRITE(((uint32_t)&__TIMx(TIMx)->CCR1 + CH), VALUE)


#define iHAL_TIM_READ_COMPARE_VALUE(TIMx, CH)					(*((uint32_t*)((uint32_t)&__TIMx(TIMx)->CCR1 + CH)))

	///**	@brief timer initialization without starting
	//*/
	//void X_TIM_Init(TIM_TypeDef* xtim, bool pulse, void(*callback)(void));	

	typedef void(*callback_f)(void);

	/** @brief set the interval and start
	*/
	void iHAL_TIM_Start(TIM_TypeDef* xtim, uint16_t Prescaler, uint16_t ARRvalue, bool pulse, callback_f callback);

	/** @brief counter stop, restart
	*/
	void iHAL_TIM_StartOver(TIM_TypeDef* xtim);

	/** @brief counter stop
	*/
	void iHAL_TIM_Stop(TIM_TypeDef* xtim);

	/** @brief Configure output compare channel
	*/
	void iHAL_TIM_OC_Config(TIM_TypeDef* xtim, uint32_t ch, TIM_OC_InitTypeDef* config);

	void iHAL_TIM_clock_enable(TIM_TypeDef* tim);

	void iHAL_TIM_clock_disable(TIM_TypeDef* tim);

	/**	@brief  Disable tim counter.
	*	reg CR1 clear CEN
	*/
	void iHAL_TIM_disable(TIM_TypeDef* tim);

#ifdef __cplusplus
}
#endif

#endif // __TIM_H
