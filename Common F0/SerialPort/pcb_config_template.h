/* Define to prevent recursive inclusion -------------------------------------*/

#ifndef __PCB_CONFIG_H
#define __PCB_CONFIG_H

#include "io.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Ports UART1 */
#define IO_TX						    ((IO_t)(__PORT_A | GPIO_PIN_9))
#define IO_RX						    ((IO_t)(__PORT_A | GPIO_PIN_10))

/* Ports RS485 */
#define IO_RS485_RX						((IO_t)(__PORT_A | GPIO_PIN_3))
#define IO_RS485_TX						((IO_t)(__PORT_A | GPIO_PIN_2))
#define IO_RS485_DE						((IO_t)(__PORT_A | GPIO_PIN_1))

#define GPIO_Ports_Clock_Enable()	do{ \
										__HAL_RCC_GPIOA_CLK_ENABLE(); \
										__HAL_RCC_GPIOB_CLK_ENABLE(); \
									}while(0)


#ifdef __cplusplus
}
#endif

#endif // __PCB_CONFIG_H
