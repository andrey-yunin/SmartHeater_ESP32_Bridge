#ifndef CMD_SRV_H_
#define CMD_SRV_H_

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include "Protocol_Srv.h" // Для доступа к структуре Frame_t

/**
 * @brief Сервис очереди команд (Command Bus)
 * Реализован как статический сервис для глобального доступа из любой задачи.
 */
class Cmd_Srv {
    public:
        // Инициализация очереди команд
        static void Init();

        /**
         * @brief Добавить команду в очередь на отправку в STM32
         * @param cmd Код команды (например, 0x13 для SET_TARGET)
         * @param data Указатель на массив данных
         * @param len Длина данных (не более MAX_DATA_LEN)
         * @return true если команда успешно добавлена
         */
        static bool Enqueue(uint8_t cmd, const uint8_t* data = nullptr, uint8_t len = 0);
   
        /**
         * @brief Извлечь команду из очереди (вызывается из Uart_Task)
         * @param outFrame Ссылка на структуру, куда будет скопирована команда
         * @param waitTicks Время ожидания (по умолчанию 0)
         * @return true если команда получена
         */
        static bool Dequeue(Frame_t &outFrame, TickType_t waitTicks = 0);
   
    private:
        static QueueHandle_t _cmdQueue;
};

#endif // CMD_SRV_H_
