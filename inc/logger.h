/**
 *  Макросы для логгирования. Логгирование работает только если добавить флаг компиляции -DLOGGER.
 *  В ином случае функции заменяются ничем, и править или удалять в самом коде ничего не надо.
 *
 *  Автор: Ахметгалиев Адель
 *  Почта: adel.ahmetgaliev@yandex.ru
 */

#ifndef LOGGER_H
#define LOGGER_H

#ifdef LOGGER

#include <stdio.h>

#define ANSI_COLOR_INFO     "\x1b[32m"
#define ANSI_COLOR_ERROR    "\x1b[31m"
#define ANSI_COLOR_RESET    "\x1b[0m"

#define log_format(tag, message, ...) \
    printf("[%s] <%s:%4d> ", tag, __FILE__, __LINE__); \
    printf(message, ##__VA_ARGS__); \
    printf("\n"); \
    fflush(stdout)

#define log_info(message, ...) \
    printf(ANSI_COLOR_INFO "[INFO] <%s:%4d> ", __FILE__, __LINE__); \
    printf(message, ##__VA_ARGS__); \
    printf("\n" ANSI_COLOR_RESET); \
    fflush(stdout)

#define log_error(message, ...) \
    printf(ANSI_COLOR_ERROR "[ERROR] <%s:%4d> ", __FILE__, __LINE__); \
    printf(message, ##__VA_ARGS__); \
    printf("\n" ANSI_COLOR_RESET); \
    fflush(stdout)

#else

#define log_format(tag, message, ...) (void) 0
#define log_info(message, ...) (void) 0
#define log_error(message, ...) (void) 0

#endif /* DEBUG */

#endif /* LOGGER_H */
