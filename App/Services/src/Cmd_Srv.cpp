#include "Cmd_Srv.h"

// Определение статического члена
QueueHandle_t Cmd_Srv::_cmdQueue = NULL;

void Cmd_Srv::Init() {
    if (_cmdQueue == NULL) {
        // Создаем очередь на 10 команд. Этого достаточно, чтобы Web не "зависал"
        // при интенсивном управлении.
        _cmdQueue = xQueueCreate(10, sizeof(Frame_t));
    }
}

bool Cmd_Srv::Enqueue(uint8_t cmd, const uint8_t* data, uint8_t len) {
    if (_cmdQueue == NULL) return false;
    Frame_t frame;
    frame.cmd = cmd;
    frame.len = (len > MAX_DATA_LEN) ? MAX_DATA_LEN : len;
    if (data != nullptr && len > 0) {
        memcpy(frame.data, data, frame.len);
    }

    // Отправляем в очередь без ожидания (non-blocking для Web-сервера)
    return xQueueSend(_cmdQueue, &frame, 0) == pdTRUE;
}

bool Cmd_Srv::Dequeue(Frame_t &outFrame, TickType_t waitTicks) {
    if (_cmdQueue == NULL) return false;
    // Ожидаем появления данных в очереди заданное время
    return xQueueReceive(_cmdQueue, &outFrame, waitTicks) == pdTRUE;
}
