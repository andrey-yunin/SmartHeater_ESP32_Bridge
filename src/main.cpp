#include <Arduino.h>
#include "Uart_Task.h"

void setup() {
    // 1. Инициализация системного лога (через USB)
    // Это наш монитор, куда будут падать сообщения о приеме пакетов
    Serial.begin(115200);
    delay(1000); // Небольшая пауза, чтобы успел открыться монитор порта

    Serial.println("=================================");
    Serial.println("SmartHeater ESP32 Bridge Starting");
    Serial.println("=================================");
   
    // 2. Запуск задачи UART (внутри нее создастся поток FreeRTOS)
    // Она сама инициализирует драйвер и начнет слушать STM32
    Uart_Task_Init();

    Serial.println("[Main] System initialized. RTOS Scheduler running...");
}

void loop() {
    // В RTOS-проектах на ESP32 loop() обычно пустой или 
    // используется для второстепенных задач с низким приоритетом.
    // Мы просто будем "спать", чтобы не мешать основным задачам.
    vTaskDelay(pdMS_TO_TICKS(1000));
}
