#ifndef NETWORK_TASK_H_
#define NETWORK_TASK_H_

#include <Arduino.h>
#include <WebServer.h>

/**
 * @brief Инициализация сетевой задачи (WiFi + WebServer)
 */
void Network_Task_Init();

/**
 * @brief Основной цикл сетевой задачи
 * Выполняется на Core 0
 */
void Network_Task_Loop(void *pvParameters);

#endif // NETWORK_TASK_H_
