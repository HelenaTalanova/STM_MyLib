// #########################################################################################################
// * Includes ____________________________________________________________________________________________

#include "assert.h"

// #########################################################################################################
// * Private: implementation ______________________________________________________________________________

/// **************************************************************************************
static void __show_error(const char* mes)
{
	for (;;);
}

// #########################################################################################################
// * Public: implementation ______________________________________________________________________________

/// **************************************************************************************
void __Error_Handler(char* file, int line)
{
	__show_error("Error_Handler");
}

/// **************************************************************************************
void __Error_Handler_Message(char* file, int line, const char* mes)
{
	__show_error(mes);
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
	   tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	   /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/// #########################################################################################################
/// * End file assert.c                                                                               #####
/// #####################################################################################################
