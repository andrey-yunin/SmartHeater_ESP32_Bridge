#include "fifo_handler.h"

static uint8_t fifo_buffer[FIFO_SIZE];
static volatile uint16_t head = 0;
static volatile uint16_t tail = 0;

bool FIFO_Push(uint8_t byte) {
    uint16_t next = (head + 1) & (FIFO_SIZE - 1);
    if (next != tail) {
        fifo_buffer[head] = byte;
        head = next;
        return true;
    }
    return false; // Буфер полон
}

bool FIFO_Pop(uint8_t *byte) {
    if (head != tail) {
        *byte = fifo_buffer[tail];
        tail = (tail + 1) & (FIFO_SIZE - 1);
        return true;
    }
    return false; // Буфер пуст
}

void FIFO_Flush(void) {
    head = tail = 0;
}
