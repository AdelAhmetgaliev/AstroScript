#ifndef DATETIME_H
#define DATETIME_H

#include <stdint.h>

/* Переводит общие секунды в часы, минуты и секунды. */
void secondsToTime(uint64_t seconds, uint64_t* hour, uint64_t* minute, uint64_t* second);

/* Возвращает среднее солнечное время в секундах. */
uint64_t getMeanSolarTime(uint64_t seconds);

/* Возвращает звездное время в секундах. */
uint64_t getSiderialTime(uint64_t meanSolarSeconds);

#endif /* DATETIME_H */
