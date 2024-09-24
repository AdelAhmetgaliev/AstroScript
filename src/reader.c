#include "reader.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "logger.h"

/* Разделители, используемые в функции readFileAndFillArrays(). */
static const char delims[] = {' ', '\n', '\0'};

static const uint64_t minuteSeconds = 60;
static const uint64_t hourSeconds = 3600;

/* Переводит строку формата времени в секунды. */
static uint64_t strTimeToSeconds(const char* const strTime);

bool readFileAndFillArrays(const char* const fileName, const char* const filter,
        uint64_t (*pTokArr)[2], uint64_t (*pFonArr)[2],
        uint64_t (*pCompArr)[2], uint64_t (*pExpTimeArr)[2]) {
    log_info("Начало работы функции readFileAndFillArrays().");

    FILE* pInputFile = fopen(fileName, "r");

    if (!pInputFile) {
        log_error("Не удается открыть файл: %s", fileName);
        return false;
    }

    char* pLineBuf = NULL;
    size_t lineBufSize = 0;
    uint32_t lineCount = 0;

    ssize_t lineSize;
    lineSize = getline(&pLineBuf, &lineBufSize, pInputFile);

    uint8_t filterColumn = 0;   /* Номер столбца нужного нам фильтра. */
    uint8_t timeColumn = 0;     /* Номер столбца времени. */
    uint8_t expTimeColumn = 0;  /* Номер столбца времени экспозиции. */

    /* Выделяем 5 байтов, потому что тип объекта не длинее 4-х символов + нуль символ. */
    char objectType[5] = "\0";
    
    size_t currentTokArrIndex = 0;
    size_t currentFonArrIndex = 0;
    size_t currentCompArrIndex = 0;
    size_t currentExpTimeArrIndex = 0;

    while (lineSize > 0) {
        /* Первые три строки нам не нужны, поэтому их пропускаем. */
        if (2 >= lineCount) {
            ++lineCount;
            lineSize = getline(&pLineBuf, &lineBufSize, pInputFile);
            continue;
        }
        
        char* token = strtok(pLineBuf, delims);
        uint8_t columnCount = 0;

        /* Найдем в каком столбце лежат время и нужный нам фильтр. */
        if (3 == lineCount) {
            while (NULL != token) {
                if (0 == strcasecmp(token, filter))
                    filterColumn = columnCount;

                if (0 == strcasecmp(token, "Time"))
                    timeColumn = columnCount;

                if (0 == strcasecmp(token, "Texp"))
                    expTimeColumn = columnCount;

                ++columnCount;
                token = strtok(NULL, delims);
            }

            ++lineCount;
            lineSize = getline(&pLineBuf, &lineBufSize, pInputFile);
            continue;
        }

        /* Заполним массивы данными. */
        uint64_t value = 0;         /* Отсчет нужного фильтра. */
        uint64_t seconds = 0;       /* Время съемки в секундах. */
        uint64_t expTime = 0;       /* Время экспозиции. */
        while (NULL != token) {
            if (0 == columnCount) {
                if (0 != strcasecmp(token, "-")) {
                    strncpy(objectType, "\0", sizeof(objectType));
                    strncat(objectType, token, sizeof(objectType) - 1);
                }
            }

            if (columnCount == filterColumn) {
                value = (uint64_t) strtoull(token, 0, 10);
            }

            if (columnCount == timeColumn) {
                seconds = strTimeToSeconds(token);
            }

            if (columnCount == expTimeColumn) {
                expTime = (uint64_t) strtoull(token, 0, 10);
            }

            ++columnCount;
            token = strtok(NULL, delims);
        }

        if (0 == strcasecmp(objectType, "tok")) {
            pTokArr[currentTokArrIndex][0] = seconds;
            pTokArr[currentTokArrIndex][1] = value;
            ++currentTokArrIndex;
        }
        else if (0 == strcasecmp(objectType, "fon")) {
            pFonArr[currentFonArrIndex][0] = seconds;
            pFonArr[currentFonArrIndex][1] = value;
            ++currentFonArrIndex;
        }
        else if (0 == strcasecmp(objectType, "comp")) {
            pCompArr[currentCompArrIndex][0] = seconds;
            pCompArr[currentCompArrIndex][1] = value;
            ++currentCompArrIndex;
        }

        pExpTimeArr[currentExpTimeArrIndex][0] = seconds;
        pExpTimeArr[currentExpTimeArrIndex][1] = expTime;
        ++currentExpTimeArrIndex;

        ++lineCount;
        lineSize = getline(&pLineBuf, &lineBufSize, pInputFile);
    }
    
    if (!pLineBuf) {
        free(pLineBuf);
        pLineBuf = NULL;
    }

    fclose(pInputFile);
    pInputFile = NULL;
    return true;
}

static uint64_t strTimeToSeconds(const char* const strTime) {
    uint64_t hours, minutes, seconds;
    sscanf(strTime, "%02lu:%02lu:%02lu", &hours, &minutes, &seconds);

    return hours * hourSeconds + minutes * minuteSeconds + seconds;
}
