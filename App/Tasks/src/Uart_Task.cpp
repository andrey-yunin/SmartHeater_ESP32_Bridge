#include "Uart_Task.h"
#include "Uart_Drv.h"
#include "Protocol_Srv.h"
#include "System_State.h" // Подключаем хранилище данных
#include "Cmd_Srv.h"       // Подключаем сервис команд



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

    // Инициализируем временем старта, чтобы не пинговать сразу
    uint32_t lastRxTime = millis();   
   
    Serial.println("[UartTask] Started on Core 1");

    while (1) {
        // --- 1. ПРОВЕРКА ОЧЕРЕДИ КОМАНД (Web -> STM32) ---
        // Если в очереди есть команда от пользователя, отправляем её немедленно
        Frame_t cmdFrame;

        if (Cmd_Srv::Dequeue(cmdFrame)) {
            uint8_t outBuf[20];
            // Упаковываем команду (например, 0x13 - SET_TEMP)
            size_t packetSize = Protocol_Srv::PackFrame(cmdFrame.cmd, cmdFrame.data, cmdFrame.len, outBuf);
            // Отправляем в UART
            Uart_Drv::SendBytes(outBuf, packetSize);

            Serial.printf("[UartTask] TX -> Sent CMD 0x%02X to STM32 (Data Len: %d)\n", cmdFrame.cmd, cmdFrame.len);
        }

        // --- 2. ПРИЕМ ДАННЫХ ИЗ ЖЕЛЕЗА ---
        Uart_Drv::Update();

        uint32_t currentTime = millis();


        // 3. WATCHDOG СВЯЗИ (ARCHITECTURE.md: PING 0x10 если нет данных > 65с)
        if (currentTime - lastRxTime >= 65000) {
            // Сбрасываем таймер, чтобы не спамить пингами, если ответа нет
            lastRxTime = currentTime; 

            uint8_t outBuf[10];
            size_t packetSize = Protocol_Srv::PackFrame(0x10, nullptr, 0, outBuf);
            Uart_Drv::SendBytes(outBuf, packetSize);

            Serial.println("[UartTask] TX -> Watchdog: Sent PING (0x10) due to RX timeout");
        }

        // 4. ПРИЕМ И ОБРАБОТКА ДАННЫХ ИЗ FIFO
        // Теперь ReadByte берет данные из нашего надежного буфера Fifo_Drv
        while (Uart_Drv::ReadByte(byte)) {
            ProtocolStatus status = protocol.ProcessByte(byte);

            if (status == ProtocolStatus::READY) {
                // Обновляем время последнего приема — канал живой
                lastRxTime = millis(); 
                protocol.GetFrame(frame);

                // --- РАСПРЕДЕЛЕНИЕ КОМАНД  ---

                // Пакет 0x03: Данные пользователя (Температуры)
                if (frame.cmd == 0x03) {
                    System_State::instance().UpdateUserData(frame.data);
                    Serial.println("[UartTask] RX <- User Data (0x03) Updated");
                    
                }

                // Пакет 0x04: Данные диагностики (Health)
                else if (frame.cmd == 0x04) {
                    System_State::instance().UpdateHealthData(frame.data);
                    // ПОЛУЧАЕМ КОПИЮ ДАННЫХ ДЛЯ ПЕЧАТИ (добавьте эту строку):
                    HeaterData_t data = System_State::instance().GetData();
                    Serial.printf("[UartTask] RX <- HEALTH (0x04). Relay: %s, Sensor: %d, Uptime: %d\n", 
                        data.relayStatus ? "ON (ГРЕЕТ)" : "OFF", 
                        data.sensorStatus,
                        data.uptime);
                }

                // Если пришел ответ ACK (0x01)
                else if (frame.cmd == 0x01) {
                    Serial.printf("[UartTask] RX <- ACK for CMD 0x%02X\n", frame.data[0]);  
                }
                // Если пришел NACK (0x02)
                else if (frame.cmd == 0x02) {
                    Serial.printf("[UartTask] RX <- NACK! Error: 0x%02X\n", frame.data[0]);
                }

                // Любой другой пакет (например, статус 0x04)
                else {
                    Serial.printf("[UartTask] RX <- Unknown Frame: CMD=0x%02X\n", frame.cmd);
                    }
            }
            else if (status == ProtocolStatus::CRC_ERROR) {
                Serial.println("[UartTask] RX <- CRC Error or Timeout!");
            }
        }
        
        // Небольшая задержка для планировщика FreeRTOS
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
