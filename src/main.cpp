#include <Arduino.h>
#include "Event_Groups.h" // Подключаем системные флаги
#include "Cmd_Srv.h"       // Подключаем сервис команд
#include "WiFi_Drv.h"      // Подключаем WiFi
#include "Uart_Task.h"
#include "Network_Task.h"

void setup() {
    // 1. Инициализация системного лога (через USB)
    // Это наш монитор, куда будут падать сообщения о приеме пакетов
    Serial.begin(115200);
    delay(1000); // Небольшая пауза, чтобы успел открыться монитор порта

    Serial.println("=================================");
    Serial.println("SmartHeater ESP32 Bridge Starting");
    Serial.println("=================================");

    // 2. Инициализация системных ресурсов (Фундамент)
    // Создаем группу событий ПЕРЕД запуском задач
    systemEvents = xEventGroupCreate();

    // Инициализируем очередь команд
    Cmd_Srv::Init();

    // 3. Инициализация драйверов
    WiFi_Drv::Init();

    // 4. Запуск задачи UART (внутри нее создастся поток FreeRTOS)
    // Она сама инициализирует драйвер и начнет слушать STM32
    Uart_Task_Init(); // Core 1 (UART Logic)
    Network_Task_Init(); // Core 0 (Network & Web)

    Serial.println("[Main] System initialized. RTOS Scheduler running...");
}

void loop() {
    // ОБЯЗАТЕЛЬНО: Пока у нас нет Network_Task, обновляем статус WiFi здесь.
    // Без этого мы не увидим IP-адрес и не выставим системные флаги.
    // WiFi_Drv::Update();
    
    // В RTOS-проектах на ESP32 loop() обычно пустой или 
    // используется для второстепенных задач с низким приоритетом.
    // Мы просто будем "спать", чтобы не мешать основным задачам.
    vTaskDelay(pdMS_TO_TICKS(1000));
}
