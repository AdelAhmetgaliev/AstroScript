#ifndef INTERPOLATION_H
#define INTERPOLATION_H

#include <stdint.h>
#include <stdio.h>

/* Обычная линейная интерполяция. */
uint64_t linearInterpolation(uint64_t (*pArr)[2], size_t rowCount, uint64_t argument);

#endif /* INTERPOLATION_H */
