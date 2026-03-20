#ifndef EVENT_GROUPS_H_
#define EVENT_GROUPS_H_
 
#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>

/**
 * @brief Битовые маски системных событий.
 * Используем constexpr для безопасности типов (C++11/14/17).
 */
namespace SysEvent {
    static constexpr uint32_t WIFI_CONNECTED = (1 << 0); // Бит 0: WiFi OK
    static constexpr uint32_t STM32_ALIVE    = (1 << 1); // Бит 1: STM32 шлет данные
    static constexpr uint32_t SERVER_STARTED = (1 << 2); // Бит 2: Web-сервер запущен
}


/**
 * Глобальный дескриптор группы событий.
 * Позволяет задачам ждать условий (например, ожидать WiFi перед запуском API).
 */
extern EventGroupHandle_t systemEvents;

#endif // EVENT_GROUPS_H_
