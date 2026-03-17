#ifndef PROTOCOL_SRV_H_
#define PROTOCOL_SRV_H_

#include <Arduino.h>

#define FRAME_START_MARKER '$'
#define MAX_DATA_LEN       16
#define PACKET_TIMEOUT_MS  50

/**
 * @brief Структура фрейма (совпадает с STM32 v1.2)
 */
typedef struct __attribute__((packed)) {
    uint8_t cmd;
    uint8_t len;
    uint8_t data[MAX_DATA_LEN];
    uint8_t crc;
    } Frame_t;

/**
 * @brief Статусы парсера
 */
enum class ProtocolStatus {
    IDLE,          // Ожидание данных
    COLLECTING,    // Сборка пакета в процессе
    READY,         // Пакет успешно собран и CRC верна
    CRC_ERROR      // Ошибка контрольной суммы
    };

class Protocol_Srv {
    public:
    Protocol_Srv();
    
    // Обработка одного входящего байта. Возвращает статус.
    ProtocolStatus ProcessByte(uint8_t byte);
   
    // Копирует последний успешно собранный фрейм
    void GetFrame(Frame_t &outFrame);

    // Сброс внутреннего состояния парсера
    void Reset();

    // Статический метод для упаковки данных в буфер перед отправкой
    // Возвращает размер упакованного фрейма
    static size_t PackFrame(uint8_t cmd, const uint8_t *data, uint8_t len, uint8_t *outBuffer);
   
    private:
    enum class State {
        WAIT_START,
        WAIT_CMD,
        WAIT_LEN,
        WAIT_DATA,
        WAIT_CRC
        };

    State _state;

    Frame_t _currentFrame;
    uint8_t _dataCounter;
    uint8_t _calculatedXor;
    uint32_t _lastByteTime;
    };

#endif // PROTOCOL_SRV_H_
