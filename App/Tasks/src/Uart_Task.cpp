#include "Uart_Task.h"
#include "Uart_Drv.h"
#include "Protocol_Srv.h"

// Создаем экземпляр парсера протокола для этой задачи
static Protocol_Srv protocol;

void Uart_Task_Init() {
    // Инициализируем драйвер UART (Hardware Serial2)
    Uart_Drv::Init(115200);

    // Создаем задачу FreeRTOS
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
    uint32_t lastPingTime = 0; // Таймер для отправки PING  
   
    Serial.println("[UartTask] Started on Core 1");

    while (1) {
        // --- ВАЖНО: Выкачиваем байты из железа в наш FIFO ---
        Uart_Drv::Update();

        uint32_t currentTime = millis();

        // 1. ПЕРИОДИЧЕСКАЯ ОТПРАВКА PING (раз в 5 секунд)
        if (currentTime - lastPingTime >= 5000) {
            lastPingTime = currentTime;

            uint8_t outBuf[10];
            // Упаковываем команду 0x10 (PING), данных нет (len=0)
            size_t packetSize = Protocol_Srv::PackFrame(0x10, nullptr, 0, outBuf);

            Serial.println("[UartTask] TX -> PING (0x10)");
            Uart_Drv::SendBytes(outBuf, packetSize);
        }

        // 2. ПРИЕМ ДАННЫХ ИЗ FIFO
        // Теперь ReadByte берет данные из нашего надежного буфера Fifo_Drv
        while (Uart_Drv::ReadByte(byte)) {
            ProtocolStatus status = protocol.ProcessByte(byte);

            if (status == ProtocolStatus::READY) {
                protocol.GetFrame(frame);

                // Если пришел ответ ACK (0x01)
                if (frame.cmd == 0x01) {
                    Serial.printf("[UartTask] RX <- ACK for CMD 0x%02X\n", frame.data[0]);
                }
                // Если пришел NACK (0x02)
                else if (frame.cmd == 0x02) {
                    Serial.printf("[UartTask] RX <- NACK! Error code: 0x%02X\n", frame.data[0]);
                }

                // Любой другой пакет (например, ваш статус 0x04)
                else {
                    Serial.printf("[UartTask] RX <- Frame: CMD=0x%02X, LEN=%d\n",
                        frame.cmd, frame.len);

                        // Вывод сырых данных для анализа
                        if (frame.len > 0) {
                            Serial.print("Data: ");
                            for(int i=0; i < frame.len; i++) {
                                Serial.printf("%02X ", frame.data[i]);
                            }
                            Serial.println();
                        }
                }
            }
            else if (status == ProtocolStatus::CRC_ERROR) {
                Serial.println("[UartTask] RX <- CRC Error!");
            }
        }
        
        // Небольшая задержка для планировщика FreeRTOS
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}
