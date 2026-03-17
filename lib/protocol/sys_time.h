#ifndef SYS_TIME_H_
#define SYS_TIME_H_

#include <Arduino.h>

/**
 * @brief Адаптер системного времени для ESP32/Arduino
 * Возвращает количество миллисекунд с момента старта.
 */
static inline uint32_t get_uptime_ms(void) {
    return millis();
}

#endif // SYS_TIME_H_
