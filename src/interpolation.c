#include "interpolation.h"

#include <math.h>

#include "logger.h"

uint64_t linearInterpolation(uint64_t (*pArr)[2], size_t rowCount, uint64_t argument) {
    log_info("Начало работы функции linearInterpolation().");

    size_t index = 0;
    while (index < rowCount) {
        if (pArr[index][0] < argument && pArr[index + 1][0] > argument) {
            double dResult = (double) pArr[index][1] +
                ((double) pArr[index][1] - (double) pArr[index + 1][1]) /
                ((double) pArr[index][0] - (double) pArr[index + 1][0]) *
                ((double) argument - pArr[index][0]);

            return (uint64_t)round(dResult);
        }
        ++index;
    }

    return 0;
}
