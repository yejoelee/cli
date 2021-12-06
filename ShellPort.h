/***************************************************************************//**
 * @file
 * @author     lizhijun
 * @version    V1.0
 * @date       2019-09-02 12:01:28
 * @brief      Define shell IO depends on platform
 * @details    Belows API is depends on platform:
 *             - #shell_writer  Write a char to console.
 *             - #shell_printf  Stdc `printf` function using on console.e
 * @note
 ******************************************************************************/
#include "ShellCustomerPort.h"

//Only for C51
#ifndef RAM_TYPE
#define RAM_TYPE
#endif

#ifndef CONFIG_USING_STD_PRINTF
#define CONFIG_USING_STD_PRINTF     0
#endif

#ifndef CONFIG_ECHO_ON
#define CONFIG_ECHO_ON              0
#endif

#ifndef __Shell_PORT_H__
#define __Shell_PORT_H__

#include <stdio.h>
#include <stdint.h>

/// Optional Defines may need by specific platforms
//#define shell_device_init()                 USART_init()       /// Using on console init

//--------------------------  Print Functions about ----------------------------------------
#if (1==CONFIG_USING_STD_PRINTF) /// Define Your Implements, otherwise, using `Shell_PRINTF`
//#define shell_print(str)                    USART_print(str)   /// print 'str'
//#define shell_println(str)                  USART_println(str) /// print 'str' end with '\n'
//#define shell_message_handler()             USART_SendBuff()   /// Flush/update ouput cache

/// Stdc `printf` function using on console
//#define shell_printf(format, args...)       your_printf(format, ##args)
#endif

/// Write a char to console
#ifndef shell_putc
#define shell_putc(x)                       putchar(x)
#endif

#ifndef shell_printf
#define shell_printf(format, args...)       printf(format, ##args)
#endif


//-------------------------------  Configuration  ---------------------------------------------
#define CONFIG_SHELL_MAX_INPUT               50  /// Max input charts for one command. Reduce this could save memory.
#define CONFIG_SHELL_MAX_COMMAND_ARGS        10  /// Max arguments for one command.

#endif
