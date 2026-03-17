 #include "Uart_Drv.h"

 void Uart_Drv::Init(uint32_t baud) {
    // Инициализация Serial2. 
    // Пины можно переопределить через Serial2.begin(baud, SERIAL_8N1, RX_PIN, TX_PIN);
    Serial2.begin(baud);
    while(!Serial2); // Ожидание инициализации (для ESP32 обычно мгновенно)
    }

    bool Uart_Drv::ReadByte(uint8_t &byte) {
    if (Serial2.available() > 0) {
        byte = (uint8_t)Serial2.read();
        return true;
        }
        return false;
    }

    void Uart_Drv::SendBytes(const uint8_t *data, size_t len) {
        if (data != nullptr && len > 0) {
            Serial2.write(data, len);
            }
        }

    int Uart_Drv::Available() {
        return Serial2.available();
        }
