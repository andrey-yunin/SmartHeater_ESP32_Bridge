#ifndef UART_TASK_H_
#define UART_TASK_H_
 
#include <Arduino.h>

/**
 * @brief Инициализация и запуск задачи UART.
 * Создает поток FreeRTOS на Core 1 (прикладное ядро).
 */
void Uart_Task_Init();

/**
 * @brief Основная функция задачи (Internal use).
 */
void Uart_Task_Loop(void *pvParameters);

#endif // UART_TASK_H_
