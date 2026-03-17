#ifndef UART_H_
#define UART_H_

#include <stdint.h>
#include <Arduino.h>

/**
 * @brief Адаптер UART для протокола.
 * На STM32 это был прямой вызов регистра, на ESP32 используем Serial2.
 */

// Для совместимости с существующим frame_sender.c
void UART1_SendChar(uint8_t ch);

#endif // UART_H_
