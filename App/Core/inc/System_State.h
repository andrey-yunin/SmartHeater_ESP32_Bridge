#ifndef SYSTEM_STATE_H_
#define SYSTEM_STATE_H_

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

/**
 * @brief Структура данных состояния (согласно UART_PROTOCOL.md)
 */
struct HeaterData_t {
    // USER_DATA (0x03)
    float currentTemp;    // T_curr (Значение / 10)
    float targetTemp;     // T_target (Значение / 10)
   
    // HEALTH_DATA (0x04)
    uint8_t sensorStatus; // 0-OK, 1-No Signal, 2-Short
    bool relayStatus;     // 0-Off, 1-On
    int8_t mcuTemp;       // Температура STM32 (°C)
    uint32_t uptime;      // Аптайм (секунды)
   
    // Служебное
    uint32_t lastUpdate;  // Время последнего получения пакета
};

class System_State {
    
    public:
        static System_State& instance() {
            static System_State inst;
            return inst;
        }

        // Обновление из пакета 0x03 (USER_DATA) - 4 байта
        void UpdateUserData(const uint8_t* data);

        // Обновление из пакета 0x04 (HEALTH_DATA) - 7 байт
        void UpdateHealthData(const uint8_t* data);

        // Получение копии данных (потокобезопасно)
        HeaterData_t GetData();

        // Проверка, активна ли связь с STM32
        bool isStm32Alive();

    private:
        System_State();
        HeaterData_t _state;
        SemaphoreHandle_t _mutex;
};

#endif
