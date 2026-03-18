
#include "Uart_Drv.h"

// Инициализация статического члена класса
Fifo_Drv<256> Uart_Drv::_rxFifo;

void Uart_Drv::Init(uint32_t baud) {
    Serial2.begin(baud);
    _rxFifo.Flush();
}

void Uart_Drv::Update() {
    // Пока в аппаратном буфере ESP32 есть байты - перекладываем их в наш FIFO
    while (Serial2.available() > 0) {
        uint8_t byte = (uint8_t)Serial2.read();
        _rxFifo.Push(byte);
    }
}

bool Uart_Drv::ReadByte(uint8_t &byte) {
    // Теперь читаем данные не из Serial2 напрямую, а из нашего FIFO
    return _rxFifo.Pop(byte);
}

void Uart_Drv::SendBytes(const uint8_t *data, size_t len) {
    if (data != nullptr && len > 0) {
        Serial2.write(data, len);
    }
}

