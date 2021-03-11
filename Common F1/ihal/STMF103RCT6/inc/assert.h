// #########################################################################################################
// Define to prevent recursive inclusion _________________________________________________________________

#ifndef __ASSERT_H
#define __ASSERT_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif 

	void __Error_Handler(char* file, int line);
	void __Error_Handler_Message(char* file, int line, const char* mes);

#define Error_Handler()							__Error_Handler(__FILE__, __LINE__)
#define Error_Handler_Mes(__MESSAGE__)			__Error_Handler_Message(__FILE__, __LINE__, __MESSAGE__)


	// #########################################################################################################
	// assert_null(ptr) ______________________________________________________________________________________

#ifdef DEBUG
#define assert_null(ptr)	{ if (ptr == 0) Error_Handler_Mes("null pointer"); }
#else
#define assert_null(ptr)	((void)0U)
#endif // DEBUG



#ifdef __cplusplus
}
#endif

#endif // __ASSERT_H
