/***************************************************************************//**
 * @file
 * @author     lizhijun
 * @version    V1.1
 * @date       2019-08-29 17:16:47
 * @brief      The most important part of Shell with command parsing and run.
 * @details    Below dependents must be satisfied to make Shell run properly:
 *             - #Shell_RecvData should be put into uart receiver handle.
 *             - #Shell_Command_Task should be put into main task loop.
 * @note
 ******************************************************************************/
#include "Shell.h"
#ifdef CONFIG_CLI

#include "ShellCMDList.h"
#include "ShellPrintf.h"

#include <string.h>
#include <stdarg.h>

#if CONFIG_ECHO_ON
#define shell_echo(c)       shell_putc(c)
#else
#define shell_echo(c)
#endif

/// To Support lower compiler
typedef unsigned char bool;
#define false 0
#define true  1

/*-------------------------------------------------------------*
 *      Internal Macros & definitions                          *
 *-------------------------------------------------------------*/
/**
*@name ASCII Control Bytes
*@{
*/
#define SHELL_ASCII_NUL                  0x00
#define SHELL_ASCII_BEL                  0x07
#define SHELL_ASCII_BS                   0x08
#define SHELL_ASCII_HT                   0x09
#define SHELL_ASCII_LF                   0x0A
#define SHELL_ASCII_CR                   0x0D
#define SHELL_ASCII_ESC                  0x1B
#define SHELL_ASCII_DEL                  0x7F
#define SHELL_ASCII_US                   0x1F
#define SHELL_ASCII_SP                   0x20
#define SHELL_ASCII_CANCLE               0x18  //Ctrl + X
/** @}*/

/*-------------------------------------------------------------*
 *      Internal funtions                                      *
 *-------------------------------------------------------------*/
static int shell_parse(char *buf, char **argv, unsigned short maxargs);
static void shell_process_escape(int argc, char **argv);
static void shell_prompt(void);


/*-------------------------------------------------------------*
 *      Internal values                                        *
 *-------------------------------------------------------------*/
/// main buffer to store received characters from the user´s terminal
static RAM_TYPE char shellbuf[CONFIG_SHELL_MAX_INPUT];
static RAM_TYPE uint16_t s_count = 0;
static volatile bool bCompleteCommand = false;
static RAM_TYPE char *argv_list[CONFIG_SHELL_MAX_COMMAND_ARGS];
static volatile bool bUserBreak = false;
static RAM_TYPE bool bCommandProcessing = false;
static RAM_TYPE shell_program_t pLastRunCommand = NULL;
static RAM_TYPE int last_argc = 0;


/*-------------------------------------------------------------*
 *      Internal functions                                     *
 *-------------------------------------------------------------*/
static int shell_parse(char *buf, char **argv, unsigned short maxargs)
{
    int i = 0;
    int argc = 0;
    int length = strlen(buf) + 1; //String lenght to parse = strlen + 1
    char toggle = 0;
    bool escape = false;

    argv[argc] = &buf[0];

    for(i = 0; i < length && argc < maxargs; i++) {
        switch(buf[i]) {
            // Handle special char: String terminator
            case '\0':
                i = length;
                argc++;
                break;

            // Handle special char: Backslash for escape sequences
            case '\\':
                // Begin of escape sequence, the following char will get
                // to it´s case with the boolean "escape" flag set
                escape = true;
                continue;

            // Handle special char: Double quotes
            case '\"':
                // If double quote is not escaped, process it as string start and end character
                // If it´s escaped then we do nothing and let the next step remove character escaping
                if(!escape) {
                    if(toggle == 0) {
                        toggle = 1;
                        buf[i] = '\0';
                        argv[argc] = &buf[i + 1];
                    } else {
                        toggle = 0;
                        buf[i] = '\0';
                    }
                }
                break;

            // Handle special char: Space is token separator
            case ' ':
                if(toggle == 0) {
                    while(buf[i] == ' ' && i < length) {
                        buf[i] = '\0';
                        i++;
                    };
                    argc++;
                    argv[argc] = &buf[i];
                }
                break;

        }
        // For character escaping
        escape = false;
    }
    return argc;
}


static void shell_process_escape(int argc, char **argv)
{
    uint8_t i, j;
    int sl;
    uint8_t readindex = 0;

    // loop for every parameter
    for(i = 0; i < argc; i++) {
        // get the length of the current arg
        sl = strlen(argv[i]);
        // loop through every character inside this argument
        for(j = 0; j < sl; j++) {
            // search for backslash character
            if(argv[i][j + readindex] == '\\') {
                // Process escaped characters here
                if(argv[i][j + readindex + 1] == '"') {
                    // Write quote double quote on the current position
                    argv[i][j] = '"';
                    readindex++;
                }
            }
            // read ahead and copy to current position only if escaped characters found
            if(readindex) {
                argv[i][j] = argv[i][j + readindex];
            }
        }
        // Reset for the next arg
        readindex = 0;
    }
}

static void shell_prompt(void)
{
    shell_print((const char *) "# ");
}


/*-------------------------------------------------------------*
 *      Global functions                                       *
 *-------------------------------------------------------------*/
void Shell_RecvData(char rxchar)
{
    if(bCompleteCommand) { //last command in processing
        return;
    }

    if(bCommandProcessing && rxchar != SHELL_ASCII_ESC && rxchar != SHELL_ASCII_CANCLE) {
        return;
    }

    // Process each one of the received characters
    switch(rxchar) {
        case SHELL_ASCII_DEL:
            shell_echo(SHELL_ASCII_BEL);
            break;

        case SHELL_ASCII_HT:
            shell_echo(SHELL_ASCII_BEL);
            break;

        case SHELL_ASCII_ESC: // For VT100 escape sequences
        case SHELL_ASCII_CANCLE:
            bUserBreak = true;
        // No break;
        case SHELL_ASCII_LF:
        case SHELL_ASCII_CR: // Enter key pressed
            bCompleteCommand = true;
            shellbuf[s_count] = '\0';
            shell_echo('\n');
            break;

        case SHELL_ASCII_BS: // Backspace pressed
            if(s_count > 0) {
                s_count--;
                shell_echo(SHELL_ASCII_BS);
                shell_echo(SHELL_ASCII_SP);
                shell_echo(SHELL_ASCII_BS);
            } else {
                shell_echo(SHELL_ASCII_BEL);
            }
            break;
        default:
            // Process printable characters, but ignore other ASCII chars
            if(s_count < (CONFIG_SHELL_MAX_INPUT - 1) && rxchar >= 0x20 && rxchar < 0x7F) {
                shellbuf[s_count] = rxchar;
                shell_echo(rxchar);
                s_count++;
            }
            break;
    }
}

void Shell_Command_Task(void)
{
    unsigned int i = 0;
    int argc = last_argc;
    COMMAND_STATE retval = COMMAND_PROCESSED;

    if(!bCommandProcessing && !bCompleteCommand) {
        return;
    }

    //Last Command In Processing
    if(bCommandProcessing && pLastRunCommand != NULL) {
        retval = pLastRunCommand(last_argc, argv_list);
    } else if(bCompleteCommand) {
        argc = shell_parse(shellbuf, argv_list, CONFIG_SHELL_MAX_COMMAND_ARGS);
        // Process escape sequences before giving args to command implementation
        shell_process_escape(argc, argv_list);
        // sequential search on command table
        for(i = 0; (cmdList[i].shell_program != NULL) && (cmdList[i].shell_command_string != NULL); i++) {
            // If string matches one on the cmdList
            if(!strcmp(argv_list[0], cmdList[i].shell_command_string)) {
                // Run the appropriate function
                retval = cmdList[i].shell_program(argc, argv_list);
                pLastRunCommand =  cmdList[i].shell_program;
                bCompleteCommand = false;
                break;
            }
        }
        // If no command found and buffer not empty
        if(bCompleteCommand != false && s_count != 0) {
            shell_println((const char *) "Command Not Found.");
        }
    }

    if(bUserBreak) {
        bUserBreak = false;
        retval = COMMAND_PROCESSED;
    }

    switch(retval) {
        case COMMAND_IN_PROCESSING:
            bCommandProcessing = true;
            last_argc = argc;
            break;
        case COMMAND_ERROR:
            shell_print("\n Error in runing ");
            shell_println(cmdList[i].shell_command_string);
        //No break on purpose
        case COMMAND_PROCESSED:
        default:
            bCommandProcessing = false;
            bCompleteCommand = false;
            s_count = 0;
            pLastRunCommand = NULL;
            shell_prompt();
            break;
    }

    shell_message_handler();
}

#ifdef shell_device_init
int Shell_Console_init(void)
{
    shell_device_init();
    shell_prompt();
    return 0;
}
#endif
#endif //CONFIG_CLI
