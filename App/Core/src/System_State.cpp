#include "System_State.h"

System_State::System_State() {
    _mutex = xSemaphoreCreateMutex();
    memset(&_state, 0, sizeof(HeaterData_t));
}


void System_State::UpdateUserData(const uint8_t* data) {
    if (data == nullptr) return;
    if (xSemaphoreTake(_mutex, pdMS_TO_TICKS(100)) == pdTRUE) {
        int16_t raw_curr = (int16_t)((data[0] << 8) | data[1]);
        int16_t raw_targ = (int16_t)((data[2] << 8) | data[3]);

    _state.currentTemp = (float)raw_curr / 10.0f;
    _state.targetTemp = (float)raw_targ / 10.0f;
    _state.lastUpdate = millis();

    xSemaphoreGive(_mutex);
    }
}

void System_State::UpdateHealthData(const uint8_t* data) {
    if (data == nullptr) return;

    if (xSemaphoreTake(_mutex, pdMS_TO_TICKS(100)) == pdTRUE) {
        _state.sensorStatus = data[0];
        _state.relayStatus  = (data[1] > 0);
        _state.mcuTemp      = (int8_t)data[2];
        _state.uptime       = (uint32_t)((data[3] << 24) | (data[4] << 16) | (data[5] << 8) | data[6]);
        
        _state.lastUpdate = millis();
        xSemaphoreGive(_mutex);
    }
}

HeaterData_t System_State::GetData() {
    HeaterData_t copy;
    if (xSemaphoreTake(_mutex, portMAX_DELAY) == pdTRUE) {
        copy = _state;
        xSemaphoreGive(_mutex);
    }
    return copy;
}


bool System_State::isStm32Alive() {
    bool alive = false;
    if (xSemaphoreTake(_mutex, pdMS_TO_TICKS(50)) == pdTRUE) {
        if (_state.lastUpdate > 0) {
            alive = (millis() - _state.lastUpdate < 5000);
        }
    xSemaphoreGive(_mutex);
    }
    return alive;
}
