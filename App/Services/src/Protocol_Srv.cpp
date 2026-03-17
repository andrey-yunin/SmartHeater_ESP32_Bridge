#include "Protocol_Srv.h"

void Protocol_Srv::Reset() {
    _state = State::WAIT_START;
    _dataCounter = 0;
    _calculatedXor = 0;
    _lastByteTime = 0;
}

Protocol_Srv::Protocol_Srv() {
    Reset();
    }

ProtocolStatus Protocol_Srv::ProcessByte(uint8_t byte) {
    // 1. Проверка таймаута (если между байтами прошло слишком много времени)
    if (_state != State::WAIT_START && (millis() - _lastByteTime > PACKET_TIMEOUT_MS)) {
        Reset();
    }
    _lastByteTime = millis();

    // 2. Стейт-машина парсера
    switch (_state) {
        case State::WAIT_START:
            if (byte == FRAME_START_MARKER) {
                _state = State::WAIT_CMD;
                _calculatedXor = 0;
                return ProtocolStatus::COLLECTING;
                }
        break;

        case State::WAIT_CMD:
            _currentFrame.cmd = byte;
            _calculatedXor = byte;
            _state = State::WAIT_LEN;
            return ProtocolStatus::COLLECTING;

        case State::WAIT_LEN:
            if (byte > MAX_DATA_LEN) {
                Reset();
            return ProtocolStatus::CRC_ERROR;
            }
            _currentFrame.len = byte;
            _calculatedXor ^= byte;
            _dataCounter = 0;
            _state = (_currentFrame.len == 0) ? State::WAIT_CRC : State::WAIT_DATA;
            return ProtocolStatus::COLLECTING;

        case State::WAIT_DATA:
            _currentFrame.data[_dataCounter++] = byte;
            _calculatedXor ^= byte;
            if (_dataCounter >= _currentFrame.len) {
                _state = State::WAIT_CRC;
                }
            return ProtocolStatus::COLLECTING;

        case State::WAIT_CRC:
            _currentFrame.crc = byte;
            _state = State::WAIT_START; // Готовимся к следующему пакету
            if (_currentFrame.crc == _calculatedXor) {
                return ProtocolStatus::READY;
                }
            else {
                return ProtocolStatus::CRC_ERROR;
                }
    }
    return ProtocolStatus::IDLE;
}

void Protocol_Srv::GetFrame(Frame_t &outFrame) {
    memcpy(&outFrame, &_currentFrame, sizeof(Frame_t));
    }

size_t Protocol_Srv::PackFrame(uint8_t cmd, const uint8_t *data, uint8_t len, uint8_t *outBuffer) {
    if (len > MAX_DATA_LEN || outBuffer == nullptr) return 0;
    
    uint8_t xorSum = 0;
    outBuffer[0] = FRAME_START_MARKER;
    outBuffer[1] = cmd;
    xorSum ^= cmd;
    outBuffer[2] = len;
    xorSum ^= len;

    for (uint8_t i = 0; i < len; i++) {
        outBuffer[3 + i] = data[i];
        xorSum ^= data[i];
    }

    outBuffer[3 + len] = xorSum;
    return 4 + len; // Стартовый маркер(1) + CMD(1) + LEN(1) + DATA(N) + CRC(1)
    }
