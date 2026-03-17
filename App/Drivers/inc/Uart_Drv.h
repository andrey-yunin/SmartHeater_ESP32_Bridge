#ifndef UART_DRV_H_
#define UART_DRV_H_

#include <Arduino.h>

/**
 * @brief Драйвер UART для связи с STM32.
 * Инкапсулирует работу с Hardware Serial.
 */
class Uart_Drv {
    public:
    // Используем Serial2 (пины по умолчанию для ESP32: RX=16, TX=17)
    static void Init(uint32_t baud = 115200);
   
    // Чтение одного байта (неблокирующее)
    static bool ReadByte(uint8_t &byte);
   
    // Отправка массива байтов
    static void SendBytes(const uint8_t *data, size_t len);
   
    // Проверка доступности данных
    static int Available();
   
    private:
    Uart_Drv() = delete; // Класс статический
    };
   
#endif // UART_DRV_H_
