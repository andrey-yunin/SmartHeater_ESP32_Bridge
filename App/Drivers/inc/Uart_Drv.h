#ifndef UART_DRV_H_
#define UART_DRV_H_

#include <Arduino.h>
#include "Fifo_Drv.h"

/**
 * @brief Драйвер UART для связи с STM32.
 * Использует внутренний FIFO для фонового накопления данных.
 */
class Uart_Drv {
    public:
        static void Init(uint32_t baud = 115200);
        
        /**
         * @brief Перекачка данных из аппаратного буфера в FIFO.
         * Должна вызываться регулярно в цикле задачи.
         */
        static void Update();
   
        /**
         * @brief Чтение байта из нашего FIFO (неблокирующее).
         */
        static bool ReadByte(uint8_t &byte);
   
        static void SendBytes(const uint8_t *data, size_t len);
   
    private:
        Uart_Drv() = delete;
        // Внутренний буфер на 256 байт
        static Fifo_Drv<256> _rxFifo;
};

#endif // UART_DRV_H_
