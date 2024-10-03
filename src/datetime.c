#include "datetime.h"

#include <math.h>

#include "config.h"

static const uint64_t minuteSeconds = 60;
static const uint64_t hourSeconds = 3600;
static const uint64_t daySeconds = 86400;
static const double translationConst = 1.0027379093;

void secondsToTime(uint64_t seconds, uint64_t* hour, uint64_t* minute, uint64_t* second) {
    *hour = seconds / hourSeconds;
    seconds -= *hour * hourSeconds;

    *minute = seconds / minuteSeconds;
    seconds -= *minute * minuteSeconds;

    *second = seconds;
}

uint64_t getMeanSolarTime(uint64_t seconds) {
    double tempSeconds = (double) seconds;
    double longitudeSeconds = longitude / 360.0 * 24.0 * (double) hourSeconds;

    tempSeconds =
        tempSeconds - 3.0 * (double) hourSeconds + longitudeSeconds - 1.0 * (double) hourSeconds;

    /* Если вышли за границы одних суток, то возвращаем. */
    if (tempSeconds < 0.0)
        tempSeconds += (double) daySeconds;

    if (tempSeconds > (double) daySeconds)
        tempSeconds -= (double) daySeconds;

    return round(tempSeconds);
}

uint64_t getSiderialTime(uint64_t meanSolarSeconds) {
    double tempResult =
        siderialTime * (double) hourSeconds + translationConst * (double) meanSolarSeconds;

    return round(tempResult);
}
