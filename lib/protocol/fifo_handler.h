#ifndef FIFO_HANDLER_H_
#define FIFO_HANDLER_H_

#include <stdint.h>
#include <stdbool.h>

#define FIFO_SIZE 128 // Должно быть степенью двойки (2^N)

/**
 * @brief Добавление байта в очередь (используется в прерывании или задаче чтения)
 */
bool FIFO_Push(uint8_t byte);

/**
 * @brief Извлечение байта из очереди
 */
bool FIFO_Pop(uint8_t *byte);

/**
 * @brief Очистка очереди
 */
void FIFO_Flush(void);

#endif // FIFO_HANDLER_H_
