/**
 *  Программа для обработки данных наблюдения звезды к курсу "Общая Астрофизика".
 *
 *  Автор: Ахметгалиев Адель
 *  Почта: adel.ahmetgaliev@yandex.ru
 */

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "logger.h"
#include "reader.h"
#include "processing.h"
#include "datetime.h"
#include "config.h"

/* Зарезервируем 100 элементов для каждого из массивов. */
#define ARRAY_BUFFER_SIZE 100

/* Название файла, куда будут записываться промежуточные вычисления. */
static const char* const interResultFileName = "data/inter_result.dat";

/* Название файла, куда будет записана таблица вычисления воздушной массы. */
static const char* const tableFileName = "data/table.dat";

/* Название файла, куда будет записана зависимость блеска от воздушной массы. */
static const char* const brightnessToAirMassFileName = "data/brightness_to_air_mass.dat";

/* Название файла, куда будут записаны коэффициенты экстинкции. */
static const char* const extinctionCoeffFileName = "data/extinction_coeff.dat";

int main(void) {
    log_info("Программа запущена.");

    const char* const fileName = "data/v603aql.dat";    /* Файл с данными. */
    const char* const filter = "R";                     /* Фильтр. Одна буква: U, B, V, R. */

    uint64_t tokArr[ARRAY_BUFFER_SIZE][2];          /* Массив темновых токов. */  
    uint64_t fonArr[ARRAY_BUFFER_SIZE][2];          /* Массив отсчетов фона. */
    uint64_t compArr[ARRAY_BUFFER_SIZE][2];         /* Массив отсчетов звезды. */
    uint64_t expTimeArr[ARRAY_BUFFER_SIZE][2];      /* Массив времени экспозиции. */

    for (size_t i = 0; i < ARRAY_BUFFER_SIZE; ++i) {
        for (size_t j = 0; j < 2; ++j) {
            tokArr[i][j] = 0;
            fonArr[i][j] = 0;
            compArr[i][j] = 0;
            expTimeArr[i][j] = 0;
        }
    }

    if (!readFileAndFillArrays(fileName, filter, tokArr, fonArr, compArr, expTimeArr)) {
        log_info("Завершение программы. Код возврата: %d.", EXIT_FAILURE);
        return EXIT_FAILURE;
    }

    /* Найдем кол-во элементов в каждом из массивов. */
    size_t tokArrRowCount = 0;
    size_t fonArrRowCount = 0;
    size_t compArrRowCount = 0;
    size_t expTimeArrRowCount = 0;

    for (size_t i = 0; i < ARRAY_BUFFER_SIZE; ++i) {
        if (0 == tokArrRowCount && 0 == tokArr[i][1])
            tokArrRowCount = i;
        if (0 == fonArrRowCount && 0 == fonArr[i][1])
            fonArrRowCount = i;
        if (0 == compArrRowCount && 0 == compArr[i][1])
            compArrRowCount = i;
        if (0 == expTimeArrRowCount && 0 == expTimeArr[i][1])
            expTimeArrRowCount = i;
    }

    /* Запишем промежуточные вычисления обработки входных данных. */
    FILE* pOutputFile = fopen(interResultFileName, "w");
    if (!pOutputFile) {
        log_error("Неудается открыть файл: %s.", interResultFileName);
        return EXIT_FAILURE;
    }

    fprintf(pOutputFile, "---------------------------------------------\n");
    fprintf(pOutputFile, "Исходный массив отсчетов звезды.\n");
    fprintf(pOutputFile, "TIME\t\tVALUE\n");
    for (size_t i = 0; i < compArrRowCount; ++i) {
        uint64_t hour, minute, second;
        secondsToTime(compArr[i][0], &hour, &minute, &second);

        fprintf(pOutputFile, "%02lu:%02lu:%02lu\t%5lu\n", hour, minute, second, compArr[i][1]);
    }
    fprintf(pOutputFile, "---------------------------------------------\n");

    fprintf(pOutputFile, "Исходный массив отсчетов фона.\n");
    fprintf(pOutputFile, "TIME\t\tVALUE\n");
    for (size_t i = 0; i < fonArrRowCount; ++i) {
        uint64_t hour, minute, second;
        secondsToTime(fonArr[i][0], &hour, &minute, &second);

        fprintf(pOutputFile, "%02lu:%02lu:%02lu\t%5lu\n", hour, minute, second, fonArr[i][1]);
    }
    fprintf(pOutputFile, "---------------------------------------------\n");

    /* Усредним соседние элементы. */
    averageValues(tokArr, &tokArrRowCount, expTimeArr, expTimeArrRowCount);
    averageValues(fonArr, &fonArrRowCount, expTimeArr, expTimeArrRowCount);
    averageValues(compArr, &compArrRowCount, expTimeArr, expTimeArrRowCount);

    fprintf(pOutputFile, "Усредненный массив отсчетов звезды.\n");
    fprintf(pOutputFile, "TIME\t\tVALUE\n");
    for (size_t i = 0; i < compArrRowCount; ++i) {
        uint64_t hour, minute, second;
        secondsToTime(compArr[i][0], &hour, &minute, &second);
        
        fprintf(pOutputFile, "%02lu:%02lu:%02lu\t%5lu\n", hour, minute, second, compArr[i][1]);
    }
    fprintf(pOutputFile, "---------------------------------------------\n");

    fprintf(pOutputFile, "Усредненный массив отсчетов фона.\n");
    fprintf(pOutputFile, "TIME\t\tVALUE\n");
    for (size_t i = 0; i < fonArrRowCount; ++i) {
        uint64_t hour, minute, second;
        secondsToTime(fonArr[i][0], &hour, &minute, &second);
        
        fprintf(pOutputFile, "%02lu:%02lu:%02lu\t%5lu\n", hour, minute, second, fonArr[i][1]);
    }
    fprintf(pOutputFile, "---------------------------------------------\n");

    /* Вычтем фон из значений отсчетов звезды. */
    substractBackground(compArr, compArrRowCount, fonArr, fonArrRowCount);

    fprintf(pOutputFile, "Массив отсчетов звезды с вычтенным фоном.\n");
    fprintf(pOutputFile, "TIME\tVALUE\n");
    for (size_t i = 0; i < compArrRowCount; ++i) {
        uint64_t hour, minute, second;
        secondsToTime(compArr[i][0], &hour, &minute, &second);
        
        if (second >= 30) minute += 1;

        fprintf(pOutputFile, "%02lu:%02lu\t%5lu\n", hour, minute, compArr[i][1]);
    }
    fprintf(pOutputFile, "---------------------------------------------\n");

    fclose(pOutputFile);

    /* Вычислим воздушные массы и запишем промежуточные вычисления в файл. */
    pOutputFile = fopen(tableFileName, "w");
    if (!pOutputFile) {
        log_error("Неудается открыть файл: %s.", tableFileName);
        return EXIT_FAILURE;
    }

    /* Массив значений воздушных масс. */
    double airMassArr[ARRAY_BUFFER_SIZE];
    for (size_t i = 0; i < ARRAY_BUFFER_SIZE; ++i)
        airMassArr[i] = 0.0;

    fprintf(pOutputFile, "Время\tСр солн время\tЗв время\tЧас угол\tВоз масса\n");
    for (size_t i = 0; i < compArrRowCount; ++i) {
        uint64_t hour, minute, second;
        secondsToTime(compArr[i][0], &hour, &minute, &second);

        if (second >= 30) minute += 1;

        /* Рассчитаем среднее солнечное время. */
        uint64_t meanSolarSeconds = getMeanSolarTime(compArr[i][0]);
        uint64_t meanSolarHour, meanSolarMinute, meanSolarSecond;
        secondsToTime(meanSolarSeconds, &meanSolarHour, &meanSolarMinute, &meanSolarSecond);

        if (meanSolarSecond >= 30) meanSolarMinute += 1;

        /* Рассчитаем звездное время. */
        uint64_t siderialTimeSeconds = getSiderialTime(meanSolarSeconds);
        uint64_t siderialHour, siderialMinute, siderialSecond;
        secondsToTime(siderialTimeSeconds, &siderialHour, &siderialMinute, &siderialSecond);

        if (siderialSecond >= 30) siderialMinute += 1;

        /* Рассчитаем часовой угол. */
        double dHourAngleSeconds = (double) siderialTimeSeconds - rightAscension * 3600.0;
        if (dHourAngleSeconds < 0.0)
            dHourAngleSeconds += 86400.0;
        if (dHourAngleSeconds > 86400.0)
            dHourAngleSeconds -= 86400.0;

        uint64_t hourAngleSeconds = round(dHourAngleSeconds);
        uint64_t hourAngleHour, hourAngleMinute, hourAngleSecond;
        secondsToTime(hourAngleSeconds, &hourAngleHour, &hourAngleMinute, &hourAngleSecond);

        if (hourAngleSecond >= 30) hourAngleMinute += 1;

        /* Рассчитаем воздушную массу. */
        double latitudeRadians = degreesToRadians(latitude);
        double declinationRadians = degreesToRadians(declination);
        double hourAngleRadians = secondsToRadians(dHourAngleSeconds);

        /* Упрощенная модель плоско-параллельной атмосферы. */
        double airMass =
            1 / (sin(latitudeRadians) * sin(declinationRadians) +
             cos(latitudeRadians) * cos(declinationRadians) * cos(hourAngleRadians));

        airMassArr[i] = airMass;

        fprintf(pOutputFile, "%02lu:%02lu\t%02lu:%02lu\t\t%02luh %02lum\t\t%02luh %02lum\t\t%f\n",
                hour, minute,
                meanSolarHour, meanSolarMinute,
                siderialHour, siderialMinute,
                hourAngleHour, hourAngleMinute,
                airMass
        );
    }
    fclose(pOutputFile);

    /* Вычислим блеск звезды. */
    double starBrightnessArr[ARRAY_BUFFER_SIZE];
    for (size_t i = 0; i < ARRAY_BUFFER_SIZE; ++i)
        starBrightnessArr[i] = 0.0;

    for (size_t i = 0; i < compArrRowCount; ++i) {
        double starBrightness = -2.5 * log10(compArr[i][1]);
        starBrightnessArr[i] = starBrightness;
    }

    /* Запишем зависимость блеска звезды от воздушной массы в файл. */
    pOutputFile = fopen(brightnessToAirMassFileName, "w");
    if (!pOutputFile) {
        log_error("Неудается открыть файл: %s.", brightnessToAirMassFileName);
        return EXIT_FAILURE;
    }

    fprintf(pOutputFile, "# Зависимость блеска звезды от воздушной массы.\n");
    fprintf(pOutputFile, "# Воз масса\t\tБлеск\n");
    for (size_t i = 0; i < compArrRowCount; ++i) {
        fprintf(pOutputFile, "%7.3f\t\t\t%.3f\n", airMassArr[i], starBrightnessArr[i]);
    }
    fclose(pOutputFile);

    /* Рассчитаем коэффициенты экстинкции. */
    double extinctionCoeffArr[ARRAY_BUFFER_SIZE];
    for (size_t i = 0; i < ARRAY_BUFFER_SIZE; ++i)
        extinctionCoeffArr[i] = 0.0;

    for (size_t i = 0; i < compArrRowCount - 1; ++i) {
        double starBright1 = starBrightnessArr[i];
        double starBright2 = starBrightnessArr[i + 1];
        double airMass1 = airMassArr[i];
        double airMass2 = airMassArr[i + 1];

        double extinctionCoeff = (starBright1 - starBright2) / (airMass1 - airMass2);
        extinctionCoeffArr[i] = extinctionCoeff;
    }

    /* Запишем коэффициенты экстинкции в файл. */
    pOutputFile = fopen(extinctionCoeffFileName, "w");
    if (!pOutputFile) {
        log_error("Неудается открыть файл: %s.", extinctionCoeffFileName);
        return EXIT_FAILURE;
    }
    
    fprintf(pOutputFile, "# Зависимость коэффцициента экстинкции от времени.\n");
    fprintf(pOutputFile, "# Время\t\tКоэфф. экстинкции\n");
    for (size_t i = 0; i < compArrRowCount - 1; ++i) {
        uint64_t averageSeconds = round((double)(compArr[i][0] + compArr[i + 1][0]) / 2.0);

        uint64_t hour, minute, second;
        secondsToTime(averageSeconds, &hour, &minute, &second);

        if (second >= 30) minute += 1;

        fprintf(pOutputFile, "  %02lu:%02lu\t\t%.3f\n",
                hour, minute, extinctionCoeffArr[i]);
    }

    log_info("Завершение программы. Код возврата: %d.", EXIT_SUCCESS);
    return EXIT_SUCCESS;
}
