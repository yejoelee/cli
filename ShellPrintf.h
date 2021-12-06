/***************************************************************************//**
 * @file
 * @author     lizhijun
 * @version    V1.0
 * @date       2020-06-09 21:19:04
 * @brief
 * @details
 * @note
 ******************************************************************************/
#ifndef __Shell_PRINTF_H__
#define __Shell_PRINTF_H__
#include "ShellPort.h"

/**
 * @brief      print 'str'
 *
 * @param[in]  string  The string
 */
#ifndef shell_print
extern void shell_print(const char *string);
#endif

/**
 * @brief      print 'str' end with '\n'
 *
 * @param[in]  string  The string
 */
#ifndef shell_println
extern void shell_println(const char *string);
#endif

/**
 * @brief      Message Output Handler
 * @details    Handle the output message to console.
 *             This function should be call every runtime.
 * @note       This funtion is required only if 'printf/putc' funtion
 *             is not print to console directly (Ref the STM32 implement).
 */
#ifndef shell_message_handler
extern int shell_message_handler(void);
#endif

#endif

