#include "Uart_Task.h"
#include "Uart_Drv.h"
#include "Protocol_Srv.h"

// Создаем экземпляр парсера протокола для этой задачи
static Protocol_Srv protocol;

void Uart_Task_Init() {
    // Инициализируем драйвер UART (Hardware Serial2)
    Uart_Drv::Init(115200);

    // Создаем задачу FreeRTOS
    // "UartTask" - имя задачи
    // 4096 - размер стека в байтах
    // NULL - параметры
    // 10 - приоритет (выше, чем у системы, но ниже, чем у критических задач)
    // NULL - дескриптор
    // 1 - номер ядра (Core 1)
   
    xTaskCreatePinnedToCore(
        Uart_Task_Loop,
        "UartTask",
        4096,
        NULL,
        10,
        NULL,
        1
    );
}

void Uart_Task_Loop(void *pvParameters) {
    uint8_t byte;
    Frame_t frame;
    Serial.println("[UartTask] Started on Core 1");

    while (1) {
        // Читаем все доступные байты из драйвера
        while (Uart_Drv::ReadByte(byte)) {
            // Скармливаем байт парсеру
            ProtocolStatus status = protocol.ProcessByte(byte);
            if (status == ProtocolStatus::READY) {
                // Пакет собран! Извлекаем его
                protocol.GetFrame(frame);

                // ЛОГИКА ДЛЯ ТЕСТА: Выводим в консоль (USB)
                Serial.printf("[UartTask] RX Frame: CMD=0x%02X, LEN=%d, CRC=0x%02X\n",
                    frame.cmd, frame.len, frame.crc);

                    if (frame.len > 0) {
                        Serial.print("Data: ");
                        for(int i=0; i < frame.len; i++) {
                            Serial.printf("%02X ", frame.data[i]);
                            }
                            Serial.println();
                    }
            }
            else if (status == ProtocolStatus::CRC_ERROR) {
                Serial.println("[UartTask] CRC Error or Timeout!");
            }
        }

        // Небольшая задержка, чтобы не "душить" процессор (1 мс)
        // Позволяет планировщику FreeRTOS переключаться на другие задачи
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}