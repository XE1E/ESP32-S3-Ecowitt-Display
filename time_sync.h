/**
 * time_sync.h - Sincronizacion de hora con el servidor
 *
 * Usa el timestamp del servidor (received_at) para ajustar
 * el reloj del ESP32, eliminando la necesidad de configurar
 * timezone manualmente.
 */

#ifndef TIME_SYNC_H
#define TIME_SYNC_H

#include <Arduino.h>
#include <time.h>
#include <sys/time.h>

static bool timeSynced = false;
static unsigned long lastSyncMillis = 0;

/**
 * Parsea timestamp ISO 8601 y ajusta el reloj del sistema
 * Formato esperado: "2026-07-17T04:11:01" o "2026-07-17T04:11:01.123456"
 *
 * @param isoTimestamp String con el timestamp del servidor
 * @return true si se pudo sincronizar
 */
bool syncTimeFromServer(const char* isoTimestamp) {
    if (isoTimestamp == nullptr || strlen(isoTimestamp) < 19) {
        Serial.println("[TIME] Timestamp invalido");
        return false;
    }

    // Parsear: "2026-07-17T04:11:01"
    int year, month, day, hour, minute, second;
    int parsed = sscanf(isoTimestamp, "%d-%d-%dT%d:%d:%d",
                        &year, &month, &day, &hour, &minute, &second);

    if (parsed != 6) {
        Serial.printf("[TIME] Error parseando: %s\n", isoTimestamp);
        return false;
    }

    // Construir struct tm
    struct tm timeinfo = {0};
    timeinfo.tm_year = year - 1900;  // Años desde 1900
    timeinfo.tm_mon = month - 1;      // Meses 0-11
    timeinfo.tm_mday = day;
    timeinfo.tm_hour = hour;
    timeinfo.tm_min = minute;
    timeinfo.tm_sec = second;
    timeinfo.tm_isdst = 0;            // Sin horario de verano

    // Convertir a time_t (segundos desde epoch)
    time_t epochTime = mktime(&timeinfo);

    if (epochTime == -1) {
        Serial.println("[TIME] Error convirtiendo a epoch");
        return false;
    }

    // Ajustar el reloj del sistema
    struct timeval tv;
    tv.tv_sec = epochTime;
    tv.tv_usec = 0;

    if (settimeofday(&tv, NULL) != 0) {
        Serial.println("[TIME] Error ajustando reloj");
        return false;
    }

    timeSynced = true;
    lastSyncMillis = millis();

    // Verificar
    time_t now;
    time(&now);
    struct tm* local = localtime(&now);

    Serial.printf("[TIME] Sincronizado: %04d-%02d-%02d %02d:%02d:%02d\n",
                  local->tm_year + 1900, local->tm_mon + 1, local->tm_mday,
                  local->tm_hour, local->tm_min, local->tm_sec);

    return true;
}

/**
 * Verifica si la hora esta sincronizada
 */
bool isTimeSynced() {
    return timeSynced;
}

/**
 * Obtiene la hora actual como string formateado
 * @param buf Buffer para almacenar el resultado
 * @param bufSize Tamaño del buffer
 * @param format24h true para 24h, false para 12h
 */
void getCurrentTimeStr(char* buf, size_t bufSize, bool format24h = true) {
    time_t now;
    time(&now);
    struct tm* local = localtime(&now);

    if (format24h) {
        snprintf(buf, bufSize, "%02d:%02d", local->tm_hour, local->tm_min);
    } else {
        int hour12 = local->tm_hour % 12;
        if (hour12 == 0) hour12 = 12;
        const char* ampm = local->tm_hour < 12 ? "AM" : "PM";
        snprintf(buf, bufSize, "%d:%02d %s", hour12, local->tm_min, ampm);
    }
}

/**
 * Obtiene la fecha actual como string
 */
void getCurrentDateStr(char* buf, size_t bufSize) {
    time_t now;
    time(&now);
    struct tm* local = localtime(&now);

    const char* dias[] = {"Dom", "Lun", "Mar", "Mie", "Jue", "Vie", "Sab"};
    const char* meses[] = {"Ene", "Feb", "Mar", "Abr", "May", "Jun",
                           "Jul", "Ago", "Sep", "Oct", "Nov", "Dic"};

    snprintf(buf, bufSize, "%s %d %s %d",
             dias[local->tm_wday],
             local->tm_mday,
             meses[local->tm_mon],
             local->tm_year + 1900);
}

/**
 * Obtiene componentes de tiempo actuales
 */
void getCurrentTime(int* hour, int* minute, int* second) {
    time_t now;
    time(&now);
    struct tm* local = localtime(&now);

    if (hour) *hour = local->tm_hour;
    if (minute) *minute = local->tm_min;
    if (second) *second = local->tm_sec;
}

#endif // TIME_SYNC_H
