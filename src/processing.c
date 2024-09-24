#include "processing.h"

#include <math.h>
#include <stdint.h>
#include <stdio.h>

#include "logger.h"
#include "interpolation.h"

static const uint64_t minuteSeconds = 60;
static const uint64_t hourSeconds = 3600;

/* Максимальное кол-во секунд, когда измерения уже не будут усредняться. */
static const uint64_t deltaSeconds = 50;
/* Число зарезервированных элементов для временного массива. */
static const size_t tempArrBuf = 100;
/* Максимальное кол-во элементов, которое будет усреднено. */
static const size_t chunkBuf = 5;

/* Усредняет значения чанка и помещает усредненные значения в chunkSecond и chunkValue. */
static void averageChunk(
        uint64_t (*pChunkArr)[2],
        size_t chunkRowCount,
        uint64_t* chunkSecond,
        uint64_t* chunkValue,
        const uint64_t (*pExpTimeArr)[2],
        size_t expTimeArrRowCount
);

void averageValues(uint64_t (*pArr)[2], size_t* rowCount,
        const uint64_t (*pExpTimeArr)[2], size_t expTimeArrRowCount) {
    log_info("Начало работы функции averageValues().");

    size_t tempArrIndex = 0;
    /* Временный массив, где будут храниться усредненные элементы. */
    uint64_t tempArr[tempArrBuf][2];
    for (size_t i = 0; i < tempArrBuf; ++i) {
        tempArr[i][0] = (uint64_t) 0;
        tempArr[i][1] = (uint64_t) 0;
    }

    size_t chunkStartIndex = 0;
    uint64_t chunkStartSeconds = pArr[chunkStartIndex][0];

    /* Массив, где будут храниться близкие по времени значения. */
    uint64_t chunkArr[chunkBuf][2];
    for (size_t i = 0; i < chunkBuf; ++i) {
        chunkArr[i][0] = (uint64_t) 0;
        chunkArr[i][1] = (uint64_t) 0;
    }

    size_t chunkIndex = 0;

    size_t index = 0;
    while (index < *rowCount) {
        size_t currentSeconds = pArr[index][0];

        if (currentSeconds - chunkStartSeconds <= deltaSeconds) {
            chunkArr[chunkIndex][0] = pArr[index][0];
            chunkArr[chunkIndex][1] = pArr[index][1];
            ++chunkIndex;
            ++index;
        }
        else {
            uint64_t chunkSecond, chunkValue;
            averageChunk(chunkArr, chunkIndex,
                    &chunkSecond, &chunkValue,
                    pExpTimeArr, expTimeArrRowCount);

            tempArr[tempArrIndex][0] = chunkSecond;
            tempArr[tempArrIndex][1] = chunkValue;
            ++tempArrIndex;

            chunkStartIndex = index;
            chunkStartSeconds = pArr[chunkStartIndex][0];
            chunkIndex = 0;
       }
   }

    /* Если в массиве-чанке еще остались элементы. */
    if (0 != chunkIndex) {
        uint64_t chunkSecond, chunkValue;
        averageChunk(chunkArr, chunkIndex,
                &chunkSecond, &chunkValue,
                pExpTimeArr, expTimeArrRowCount);

        tempArr[tempArrIndex][0] = chunkSecond;
        tempArr[tempArrIndex][1] = chunkValue;
        ++tempArrIndex;
    }

    /* Копируем элементы из временного массива в основной. */
    size_t i = 0;
    while (i < tempArrIndex) {
        pArr[i][0] = tempArr[i][0];
        pArr[i][1] = tempArr[i][1];

        ++i;
    }

    /* Оставшиеся элементы основного массива зануляем. */
    while (i < *rowCount) {
        pArr[i][0] = (uint64_t) 0;
        pArr[i][1] = (uint64_t) 0;

        ++i;
    }

    /* Новое значение размера основного массива. */
    *rowCount = tempArrIndex;
}

void substractBackground(uint64_t (*pCompArr)[2], size_t compArrRowCount,
        uint64_t (*pFonArr)[2], size_t fonArrRowCount) {
    for (size_t i = 0; i < compArrRowCount; ++i) {
        uint64_t interpolatedFonValue =
            linearInterpolation(pFonArr, fonArrRowCount, pCompArr[i][0]);
        pCompArr[i][1] -= interpolatedFonValue;
    }
}

double degreesToRadians(double degrees) {
    return degrees * (M_PI / 180.0);
}

double secondsToRadians(double seconds) {
    double hours, degrees;
    uint64_t hour, minute;

    /* Сначала переведем общие секунды в часы, минуты и секунды. */
    hour = trunc(seconds / (double) hourSeconds);
    seconds -= hour * hourSeconds;

    minute = trunc(seconds / (double) minuteSeconds);
    seconds -= minute * minuteSeconds;
    
    /* Переведем в доли часов. */
    hours = hour + minute / (double) minuteSeconds + seconds / (double) hourSeconds;
    
    /* Переведем часовую величину в угловую. */
    degrees = hours * (15.0);
 
    return degrees * (M_PI / 180.0);
}

static void averageChunk(uint64_t (*pChunkArr)[2], size_t chunkRowCount,
        uint64_t* chunkSecond, uint64_t* chunkValue,
        const uint64_t (*pExpTimeArr)[2], size_t expTimeArrRowCount) {
    double dChunkSecondsSum = 0.0;
    double dChunkValuesSum = 0.0;
    for (size_t i = 0; i < chunkRowCount; ++i) {
        uint64_t currentSecond = pChunkArr[i][0];
        uint64_t currentValue = pChunkArr[i][1];

        /* Перед усреднением разделим значения отсчетов на время экспозиции. */
        double currentValueExpTime = 0.0;
        for (size_t j = 0; j < expTimeArrRowCount; ++j) {
            if (pExpTimeArr[j][0] == currentSecond) {
                currentValueExpTime = (double) pExpTimeArr[j][1];
            }
        }

        dChunkSecondsSum += (double) currentSecond;
        dChunkValuesSum += (double) currentValue / currentValueExpTime;

        pChunkArr[i][0] = (uint64_t) 0;
        pChunkArr[i][1] = (uint64_t) 0;
    }
    dChunkSecondsSum /= (double) chunkRowCount;
    dChunkValuesSum /= (double) chunkRowCount;

    *chunkSecond = (uint64_t) round(dChunkSecondsSum);
    *chunkValue = (uint64_t) round(dChunkValuesSum);
}
