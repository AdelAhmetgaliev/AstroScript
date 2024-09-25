#ifndef PROCESSING_H
#define PROCESSING_H

#include <stdint.h>
#include <stdio.h>

/* Усредняет близкие по времени элементы массива. */
void averageValues(
        uint64_t (*pArr)[2],                /* Двумерный массив, который будет усреднен. */
        size_t* rowCount,                   /* Число строк в этом массиве, 
                                             * которое будет изменено после усреднения. */
        const uint64_t (*pExpTimeArr)[2],   /* Массив времен экспозиции. */
        size_t expTimeArrRowCount           /* Кол-во элементов в массиве времен экспозиции. */
);

/* Вычетает фон из отсчетов звезды. */
void substractBackground(
        uint64_t (*pCompArr)[2],    /* Двумерный массив отсчетов звезды. */
        size_t compArrRowCount,     /* Размер массива отсчетов звезды. */
        uint64_t (*pFonArr)[2],     /* Двумерный массив отсчетов фона. */
        size_t fonArrRowCount       /* Размер массива отсчетов фона. */
);

/* Переводит градусы в радианы. */
double degreesToRadians(double degrees);

/* Переводит секунды часовой дуги в радианы. */
double secondsToRadians(double seconds);

#endif /* PROCESSING_H */
