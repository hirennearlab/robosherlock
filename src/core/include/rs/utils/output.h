/*
 * output.h
 *
 *  Created on: Jul 26, 2013
 *      Author: raider
 */

#ifndef OUTPUT_H_
#define OUTPUT_H_

#include <iostream>
#include <string.h>

#define NO_COLOR        "\033[0m"

#define FG_BLACK        "\033[30m"
#define FG_RED          "\033[31m"
#define FG_GREEN        "\033[32m"
#define FG_YELLOW       "\033[33m"
#define FG_BLUE         "\033[34m"
#define FG_MAGENTA      "\033[35m"
#define FG_CYAN         "\033[36m"
#define FG_LIGHTGREY    "\033[37m"
#define FG_GREY         "\033[90m"
#define FG_LIGHTRED     "\033[91m"
#define FG_LIGHTGREEN   "\033[92m"
#define FG_LIGHTYELLOW  "\033[93m"
#define FG_LIGHTBLUE    "\033[94m"
#define FG_LIGHTMAGENTA "\033[95m"
#define FG_LIGHTCYAN    "\033[96m"
#define FG_WHITE        "\033[97m"

#define BG_BLACK        "\033[40m"
#define BG_RED          "\033[41m"
#define BG_GREEN        "\033[42m"
#define BG_YELLOW       "\033[43m"
#define BG_BLUE         "\033[44m"
#define BG_MAGENTA      "\033[45m"
#define BG_CYAN         "\033[46m"
#define BG_LIGHTGREY    "\033[47m"
#define BG_GREY         "\033[100m"
#define BG_LIGHTRED     "\033[101m"
#define BG_LIGHTGREEN   "\033[102m"
#define BG_LIGHTYELLOW  "\033[103m"
#define BG_LIGHTBLUE    "\033[104m"
#define BG_LIGHTMAGENTA "\033[105m"
#define BG_LIGHTCYAN    "\033[106m"
#define BG_WHITE        "\033[107m"

#define OUT_LEVEL_NOOUT 0
#define OUT_LEVEL_ERROR 1
#define OUT_LEVEL_INFO  2
#define OUT_LEVEL_DEBUG 3

#ifndef OUT_LEVEL
#define OUT_LEVEL OUT_LEVEL_INFO
#endif

#define OUT_STD_STREAM std::cout
#define OUT_ERR_STREAM std::cerr

#define OUT_FILENAME (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define OUT_AUX_INT(FILE_COLOR, MSG_COLOR, LEVEL, STREAM, MSG, FILE, LINE, FUNCTION) LEVEL <= OUT_LEVEL && STREAM << FILE_COLOR << FILE << NO_COLOR "(" FG_CYAN << LINE << NO_COLOR ")[" FG_YELLOW << FUNCTION << NO_COLOR "] " MSG_COLOR << MSG << NO_COLOR << std::endl << std::flush
#define OUT_AUX(FILE_COLOR, MSG_COLOR, LEVEL, STREAM, MSG) OUT_AUX_INT(FILE_COLOR, MSG_COLOR, LEVEL, STREAM, MSG, OUT_FILENAME, __LINE__, __FUNCTION__)

#define outDebug(msg) OUT_AUX(FG_BLUE, NO_COLOR, OUT_LEVEL_DEBUG, OUT_STD_STREAM, msg)
#define outInfo(msg)  OUT_AUX(FG_GREEN, NO_COLOR, OUT_LEVEL_INFO,  OUT_STD_STREAM, msg)
#define outWarn(msg) OUT_AUX(FG_YELLOW, FG_YELLOW, OUT_LEVEL_ERROR, OUT_ERR_STREAM, msg)
#define outError(msg) OUT_AUX(FG_RED, FG_RED, OUT_LEVEL_ERROR, OUT_ERR_STREAM, msg)
#define outAssert(expr,msg) if (!(expr)) OUT_AUX(FG_MAGENTA, FG_MAGENTA, OUT_LEVEL_ERROR, OUT_ERR_STREAM, msg)

#endif /* OUTPUT_H_ */