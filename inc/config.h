#ifndef CONFIG_H
#define CONFIG_H

/* Координаты обрабатываемой звезды. 
 * Взято с сайта: https://simbad.cds.unistra.fr/simbad/sim-basic?Ident=v603aql&submit=SIMBAD+search
 * Эпоха - B1950.
 */
static const double rightAscension = 18.77262537;   /* Прямое восхождение в часах. */
static const double declination = 0.52680807;       /* Склонение в градусах. */

/* Координаты места наблюдения. */
static const double latitude = 43.81666667;     /* Широта в градусах. */
static const double longitude = 41.45;          /* Долгота в градусах. */

/* Звездное время в местную полночь.
 * Взято с сайта: https://babel.hathitrust.org/cgi/pt?id=mdp.39015026570146&seq=114
 */
static const double siderialTime = 0.961825; /* В часах. */

#endif /* CONFIG_H */
