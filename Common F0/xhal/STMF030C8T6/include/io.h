/* Define to prevent recursive inclusion -------------------------------------*/

/* Usage example module *******************************************************

	// Declare GPIO pin				(GPIOB , 12)
	#define IO_LED_TEST				((uint32_t)(__PORT_B | GPIO_PIN_12))

	// Initialization
	GPIO_Config(IO_LED_TEST, IO_MODE_OUT_OD__PULL_NO__SPEED_LOW);

	// Toggle state
	IO_TOGGLE(IO_LED_TEST);
*/

#ifndef __IO_H
#define __IO_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <stdint.h>
#include "stm32f0xx.h"

#define IO_NO_PIN_PORT							(0UL)				/* No pin port selected    */

#define GPIO_PIN_0								((uint16_t)0x0001U)  /* Pin 0 selected    */
#define GPIO_PIN_1								((uint16_t)0x0002U)  /* Pin 1 selected    */
#define GPIO_PIN_2								((uint16_t)0x0004U)  /* Pin 2 selected    */
#define GPIO_PIN_3								((uint16_t)0x0008U)  /* Pin 3 selected    */
#define GPIO_PIN_4								((uint16_t)0x0010U)  /* Pin 4 selected    */
#define GPIO_PIN_5								((uint16_t)0x0020U)  /* Pin 5 selected    */
#define GPIO_PIN_6								((uint16_t)0x0040U)  /* Pin 6 selected    */
#define GPIO_PIN_7								((uint16_t)0x0080U)  /* Pin 7 selected    */
#define GPIO_PIN_8								((uint16_t)0x0100U)  /* Pin 8 selected    */
#define GPIO_PIN_9								((uint16_t)0x0200U)  /* Pin 9 selected    */
#define GPIO_PIN_10								((uint16_t)0x0400U)  /* Pin 10 selected   */
#define GPIO_PIN_11								((uint16_t)0x0800U)  /* Pin 11 selected   */
#define GPIO_PIN_12								((uint16_t)0x1000U)  /* Pin 12 selected   */
#define GPIO_PIN_13								((uint16_t)0x2000U)  /* Pin 13 selected   */
#define GPIO_PIN_14								((uint16_t)0x4000U)  /* Pin 14 selected   */
#define GPIO_PIN_15								((uint16_t)0x8000U)  /* Pin 15 selected   */
#define GPIO_PIN_All							((uint16_t)0xFFFFU)  /* All pins selected */

#define __IO_SHIFT_GPIO_BASE					16
#define __IO_PIN_MASK							0x0000FFFFUL

#define __PORT_A								(GPIOA_BASE << __IO_SHIFT_GPIO_BASE)
#define __PORT_B								(GPIOB_BASE << __IO_SHIFT_GPIO_BASE)
#define __PORT_C								(GPIOC_BASE << __IO_SHIFT_GPIO_BASE)
#define __PORT_D								(GPIOD_BASE << __IO_SHIFT_GPIO_BASE)
#define __PORT_F								(GPIOF_BASE << __IO_SHIFT_GPIO_BASE)

	/*############################ Get CMSIS value ####################################*/

#define IO_PIN(VALUE)							((uint16_t)(VALUE & __IO_PIN_MASK))
#define IO_PORT(VALUE)							((GPIO_TypeDef *)(AHB2PERIPH_BASE + (VALUE >> __IO_SHIFT_GPIO_BASE)))

	/*############################## Application ######################################*/

#define IO_READ(VALUE)							((IO_PORT(VALUE)->IDR & IO_PIN(VALUE)) != 0U)
#define IO_READ_INVERSE(VALUE)					((IO_PORT(VALUE)->IDR & IO_PIN(VALUE)) == 0U)

#define IO_OUT_SET								0
#define IO_OUT_RESET							16
#define IO_WRITE(VALUE, STATE)					(IO_PORT(VALUE)->BSRR = ((uint32_t)IO_PIN(VALUE) << (STATE)))
#define IO_TOGGLE(VALUE)						(IO_PORT(VALUE)->ODR ^= IO_PIN(VALUE))

	/*############################## Configuration #####################################*/

	/* |MODER	|OTYPER		|OSPEEDR	|PUPDR
	 * |01		|0			|..			|00		>>	out PP
	 * |01		|0			|..			|01		>>	out PP + PU
	 * |01		|0			|..			|10		>>	out PP + PD
	 * |01		|0			|..			|11		>>	reserve
	 *
	 * |01		|1			|..			|00		>>	out OD
	 * |01		|1			|..			|01		>>	out OD + PU
	 * |01		|1			|..			|10		>>	out OD + PD
	 * |01		|1			|..			|11		>>	reserve
	 *
	 * |10		|0			|..			|00		>>	AF PP
	 * |10		|0			|..			|01		>>	AF PP + PU
	 * |10		|0			|..			|10		>>	AF PP + PD
	 * |10		|0			|..			|11		>>	reserved
	 *
	 * |10		|1			|..			|00		>>	AF OD
	 * |10		|1			|..			|01		>>	AF OD + PU
	 * |10		|1			|..			|10		>>	AF OD + PD
	 * |10		|1			|..			|11		>>	reserved
	 *
	 * |00		|x			|xx			|00		>>	in Floating
	 * |00		|x			|xx			|01		>>	in PU
	 * |00		|x			|xx			|10		>>	in PD
	 * |00		|x			|xx			|11		>>	reserved(Input floating)
	 * ---------------------------------------------------------------------
	 * |11		|x			|xx			|00		>>	in/out Analog
	 * |11		|x			|xx			|01		>>	reserved
	 * |11		|x			|xx			|10		>>	reserved
	 * |11		|x			|xx			|11		>>	reserved
	 *
	 * GPIO port configuration lock register (GPIOx_LCKR)
	 **/

	 // Config mode
#define __IO_SHIFT_MODE							0
#define __IO_MODE_IN							(0x00000000UL << __IO_SHIFT_MODE)
#define __IO_MODE_OUT							(0x00000001UL << __IO_SHIFT_MODE)
#define __IO_MODE_AF							(0x00000002UL << __IO_SHIFT_MODE)
#define __IO_MODE_ANALOG						(0x00000003UL << __IO_SHIFT_MODE)
// Config out type
#define __IO_SHIFT_TYPE_OUT						4
#define __IO_OUT_PP								(0x00000000UL << __IO_SHIFT_TYPE_OUT)
#define __IO_OUT_OD								(0x00000001UL << __IO_SHIFT_TYPE_OUT)
// Config speed
#define __IO_SHIFT_SPEED						8
#define __IO_SPEED_LOW							(0x00000000UL << __IO_SHIFT_SPEED)
#define __IO_SPEED_MID							(0x00000001UL << __IO_SHIFT_SPEED)
#define __IO_SPEED_HI							(0x00000002UL << __IO_SHIFT_SPEED)
// Config pull type
#define __IO_SHIFT_PULL_TYPE					12
#define __IO_PULL_NO							(0x00000000UL << __IO_SHIFT_PULL_TYPE)
#define __IO_PULL_UP							(0x00000001UL << __IO_SHIFT_PULL_TYPE)
#define __IO_PULL_DOWN							(0x00000002UL << __IO_SHIFT_PULL_TYPE)
// Config alternative num
#define __IO_SHIFT_AF							16
#define IO_AF0									(0x00000000UL << __IO_SHIFT_AF)
#define IO_AF1									(0x00000001UL << __IO_SHIFT_AF)
#define IO_AF2									(0x00000002UL << __IO_SHIFT_AF)
#define IO_AF3									(0x00000003UL << __IO_SHIFT_AF)
#define IO_AF4									(0x00000004UL << __IO_SHIFT_AF)
#define IO_AF5									(0x00000005UL << __IO_SHIFT_AF)
#define IO_AF6									(0x00000006UL << __IO_SHIFT_AF)
#define IO_AF7									(0x00000007UL << __IO_SHIFT_AF)

#define __IO_SHIFT_LOCK							31
#define LOCK_PIN								(0x00000001UL << __IO_SHIFT_LOCK)

// Default config Mode analog *****************************************************************************
#define IO_CONF__ANALOG							(IO_Config)(IO_MODE_ANALOG)

// Default config Mode In *********************************************************************************
// Default config mode In floating
#define IO_MODE_IN__FLOATING__SPEED_LOW			(IO_Config)(IO_MODE_IN | IO_PULL_NO | IO_SPEED_LOW)
#define IO_MODE_IN__FLOATING__SPEED_MID			(IO_Config)(IO_MODE_IN | IO_PULL_NO | IO_SPEED_MID)
#define IO_MODE_IN__FLOATING__SPEED_HI			(IO_Config)(IO_MODE_IN | IO_PULL_NO | IO_SPEED_HI)
// Default config mode in pull up
#define IO_MODE_IN__PULL_UP__SPEED_LOW			(IO_Config)(IO_MODE_IN | IO_PULL_UP | IO_SPEED_LOW)
#define IO_MODE_IN__PULL_UP__SPEED_MID			(IO_Config)(IO_MODE_IN | IO_PULL_UP | IO_SPEED_MID)
#define IO_MODE_IN__PULL_UP__SPEED_HI			(IO_Config)(IO_MODE_IN | IO_PULL_UP | IO_SPEED_HI)
// Default config mode in pull down
#define IO_MODE_IN__PULL_DOWN__SPEED_LOW			(IO_Config)(IO_MODE_IN | IO_PULL_DOWN | IO_SPEED_LOW)
#define IO_MODE_IN__PULL_DOWN__SPEED_MID			(IO_Config)(IO_MODE_IN | IO_PULL_DOWN | IO_SPEED_MID)
#define IO_MODE_IN__PULL_DOWN__SPEED_HI			(IO_Config)(IO_MODE_IN | IO_PULL_DOWN | IO_SPEED_HI)

// Default config Mode Out ********************************************************************************
// Default config mode out open drain pull none
#define IO_MODE_OUT_OD__PULL_NO__SPEED_LOW		(IO_Config)(IO_MODE_OUT_OD | IO_PULL_NO | IO_SPEED_LOW)
#define IO_MODE_OUT_OD__PULL_NO__SPEED_MID		(IO_Config)(IO_MODE_OUT_OD | IO_PULL_NO | IO_SPEED_MID)
#define IO_MODE_OUT_OD__PULL_NO__SPEED_HI		(IO_Config)(IO_MODE_OUT_OD | IO_PULL_NO | IO_SPEED_HI)
// Default config mode out open drain pull up
#define IO_MODE_OUT_OD__PULL_UP__SPEED_LOW		(IO_Config)(IO_MODE_OUT_OD | IO_PULL_UP | IO_SPEED_LOW)
#define IO_MODE_OUT_OD__PULL_UP__SPEED_MID		(IO_Config)(IO_MODE_OUT_OD | IO_PULL_UP | IO_SPEED_MID)
#define IO_MODE_OUT_OD__PULL_UP__SPEED_HI		(IO_Config)(IO_MODE_OUT_OD | IO_PULL_UP | IO_SPEED_HI)
// Default config mode out open drain pull down
#define IO_MODE_OUT_OD__PULL_DOWN__SPEED_LOW	(IO_Config)(IO_MODE_OUT_OD | IO_PULL_DOWN | IO_SPEED_LOW)
#define IO_MODE_OUT_OD__PULL_DOWN__SPEED_MID	(IO_Config)(IO_MODE_OUT_OD | IO_PULL_DOWN | IO_SPEED_MID)
#define IO_MODE_OUT_OD__PULL_DOWN__SPEED_HI		(IO_Config)(IO_MODE_OUT_OD | IO_PULL_DOWN | IO_SPEED_HI)
// Default config mode push pull pull none
#define IO_MODE_OUT_PP__PULL_NO__SPEED_LOW		(IO_Config)(IO_MODE_OUT_PP | IO_PULL_NO | IO_SPEED_LOW)
#define IO_MODE_OUT_PP__PULL_NO__SPEED_MID		(IO_Config)(IO_MODE_OUT_PP | IO_PULL_NO | IO_SPEED_MID)
#define IO_MODE_OUT_PP__PULL_NO__SPEED_HI		(IO_Config)(IO_MODE_OUT_PP | IO_PULL_NO | IO_SPEED_HI)
// Default config mode push pull pull up
#define IO_MODE_OUT_PP__PULL_UP__SPEED_LOW		(IO_Config)(IO_MODE_OUT_PP | IO_PULL_UP | IO_SPEED_LOW)
#define IO_MODE_OUT_PP__PULL_UP__SPEED_MID		(IO_Config)(IO_MODE_OUT_PP | IO_PULL_UP | IO_SPEED_MID)
#define IO_MODE_OUT_PP__PULL_UP__SPEED_HI		(IO_Config)(IO_MODE_OUT_PP | IO_PULL_UP | IO_SPEED_HI)
// Default config mode push pull pull down
#define IO_MODE_OUT_PP__PULL_DOWN__SPEED_LOW	(IO_Config)(IO_MODE_OUT_PP | IO_PULL_DOWN | IO_SPEED_LOW)
#define IO_MODE_OUT_PP__PULL_DOWN__SPEED_MID	(IO_Config)(IO_MODE_OUT_PP | IO_PULL_DOWN | IO_SPEED_MID)
#define IO_MODE_OUT_PP__PULL_DOWN__SPEED_HI		(IO_Config)(IO_MODE_OUT_PP | IO_PULL_DOWN | IO_SPEED_HI)

// Default config Mode alternative ************************************************************************
// Default config alternative mode open drain pull none
#define IO_MODE_AFx_OD__PULL_NO__SPEED_LOW		(IO_Config)(IO_MODE_OUT_OD_AF | IO_PULL_NO | IO_SPEED_LOW)
#define IO_MODE_AFx_OD__PULL_NO__SPEED_MID		(IO_Config)(IO_MODE_OUT_OD_AF | IO_PULL_NO | IO_SPEED_MID)
#define IO_MODE_AFx_OD__PULL_NO__SPEED_HI		(IO_Config)(IO_MODE_OUT_OD_AF | IO_PULL_NO | IO_SPEED_HI)
// Default config alternative mode open drain pull up
#define IO_MODE_AFx_OD__PULL_UP__SPEED_LOW		(IO_Config)(IO_MODE_OUT_OD_AF | IO_PULL_UP | IO_SPEED_LOW)
#define IO_MODE_AFx_OD__PULL_UP__SPEED_MID		(IO_Config)(IO_MODE_OUT_OD_AF | IO_PULL_UP | IO_SPEED_MID)
#define IO_MODE_AFx_OD__PULL_UP__SPEED_HI		(IO_Config)(IO_MODE_OUT_OD_AF | IO_PULL_UP | IO_SPEED_HI)
// Default config alternative mode open drain pull down
#define IO_MODE_AFx_OD__PULL_DOWN__SPEED_LOW	(IO_Config)(IO_MODE_OUT_OD_AF | IO_PULL_DOWN | IO_SPEED_LOW)
#define IO_MODE_AFx_OD__PULL_DOWN__SPEED_MID	(IO_Config)(IO_MODE_OUT_OD_AF | IO_PULL_DOWN | IO_SPEED_MID)
#define IO_MODE_AFx_OD__PULL_DOWN__SPEED_HI		(IO_Config)(IO_MODE_OUT_OD_AF | IO_PULL_DOWN | IO_SPEED_HI)
// Default config alternative mode push pull pull none
#define IO_MODE_AFx_PP__PULL_NO__SPEED_LOW		(IO_Config)(IO_MODE_OUT_PP_AF | IO_PULL_NO | IO_SPEED_LOW)
#define IO_MODE_AFx_PP__PULL_NO__SPEED_MID		(IO_Config)(IO_MODE_OUT_PP_AF | IO_PULL_NO | IO_SPEED_MID)
#define IO_MODE_AFx_PP__PULL_NO__SPEED_HI		(IO_Config)(IO_MODE_OUT_PP_AF | IO_PULL_NO | IO_SPEED_HI)
// Default config alternative mode push pull pull up
#define IO_MODE_AFx_PP__PULL_UP__SPEED_LOW		(IO_Config)(IO_MODE_OUT_PP_AF | IO_PULL_UP | IO_SPEED_LOW)
#define IO_MODE_AFx_PP__PULL_UP__SPEED_MID		(IO_Config)(IO_MODE_OUT_PP_AF | IO_PULL_UP | IO_SPEED_MID)
#define IO_MODE_AFx_PP__PULL_UP__SPEED_HI		(IO_Config)(IO_MODE_OUT_PP_AF | IO_PULL_UP | IO_SPEED_HI)
// Default config alternative mode push pull pull down
#define IO_MODE_AFx_PP__PULL_DOWN__SPEED_LOW	(IO_Config)(IO_MODE_OUT_PP_AF | IO_PULL_DOWN | IO_SPEED_LOW)
#define IO_MODE_AFx_PP__PULL_DOWN__SPEED_MID	(IO_Config)(IO_MODE_OUT_PP_AF | IO_PULL_DOWN | IO_SPEED_MID)
#define IO_MODE_AFx_PP__PULL_DOWN__SPEED_HI		(IO_Config)(IO_MODE_OUT_PP_AF | IO_PULL_DOWN | IO_SPEED_HI)

	typedef enum
	{
		IO_MODE_IN = __IO_MODE_IN,
		IO_MODE_OUT_OD = __IO_MODE_OUT | __IO_OUT_OD,
		IO_MODE_OUT_PP = __IO_MODE_OUT | __IO_OUT_PP,
		IO_MODE_ANALOG = __IO_MODE_ANALOG,

		IO_MODE_OUT_OD_AF = __IO_MODE_AF | __IO_OUT_OD,
		IO_MODE_OUT_OD_AF0 = __IO_MODE_AF | __IO_OUT_OD | IO_AF0,
		IO_MODE_OUT_OD_AF1 = __IO_MODE_AF | __IO_OUT_OD | IO_AF1,
		IO_MODE_OUT_OD_AF2 = __IO_MODE_AF | __IO_OUT_OD | IO_AF2,
		IO_MODE_OUT_OD_AF3 = __IO_MODE_AF | __IO_OUT_OD | IO_AF3,
		IO_MODE_OUT_OD_AF4 = __IO_MODE_AF | __IO_OUT_OD | IO_AF4,
		IO_MODE_OUT_OD_AF5 = __IO_MODE_AF | __IO_OUT_OD | IO_AF5,
		IO_MODE_OUT_OD_AF6 = __IO_MODE_AF | __IO_OUT_OD | IO_AF6,
		IO_MODE_OUT_OD_AF7 = __IO_MODE_AF | __IO_OUT_OD | IO_AF7,

		IO_MODE_OUT_PP_AF = __IO_MODE_AF | __IO_OUT_PP,
		IO_MODE_OUT_PP_AF0 = __IO_MODE_AF | __IO_OUT_PP | IO_AF0,
		IO_MODE_OUT_PP_AF1 = __IO_MODE_AF | __IO_OUT_PP | IO_AF1,
		IO_MODE_OUT_PP_AF2 = __IO_MODE_AF | __IO_OUT_PP | IO_AF2,
		IO_MODE_OUT_PP_AF3 = __IO_MODE_AF | __IO_OUT_PP | IO_AF3,
		IO_MODE_OUT_PP_AF4 = __IO_MODE_AF | __IO_OUT_PP | IO_AF4,
		IO_MODE_OUT_PP_AF5 = __IO_MODE_AF | __IO_OUT_PP | IO_AF5,
		IO_MODE_OUT_PP_AF6 = __IO_MODE_AF | __IO_OUT_PP | IO_AF6,
		IO_MODE_OUT_PP_AF7 = __IO_MODE_AF | __IO_OUT_PP | IO_AF7,
	}IO_MODE;

	typedef enum {
		IO_PULL_NO = __IO_PULL_NO,
		IO_PULL_UP = __IO_PULL_UP,
		IO_PULL_DOWN = __IO_PULL_DOWN,
	}IO_PULL;

	typedef enum {
		IO_SPEED_LOW = __IO_SPEED_LOW,
		IO_SPEED_MID = __IO_SPEED_MID,
		IO_SPEED_HI = __IO_SPEED_HI,
	}IO_SPEED;

	typedef unsigned int IO_t;
	typedef unsigned int IO_Config;

	/* - - - - - - - - - - - - - - - - - - - - - - - - -
	Configure GPIO one pin
	param [io] - PORT & PIN -> struct IO_t
	param [mode] - in / out / analog / alternative / open drain / push pull
	param [pull] - internal resistor
	param [speed] - speed pin
	*/
	void xHAL_IO_Init(IO_t io, IO_MODE mode, IO_PULL pull, IO_SPEED speed);

	/* - - - - - - - - - - - - - - - - - - - - - - - - -
	Configure GPIO one pin
	param [io] - PORT & PIN -> struct IO_t
	param [config] - config -> struct IO_Config_t
	*/
	void xHAL_IO_Config(IO_t io, IO_Config config);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __IO_H
