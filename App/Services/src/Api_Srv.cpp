#include "Api_Srv.h"

String Api_Srv::GetSystemStatusJson() {
// Получаем атомарный снимок данных из System_State (Источник правды)
HeaterData_t data = System_State::instance().GetData();

// Ручная сборка JSON (Прототипный уровень)
// Мы гарантируем соответствие типов данных для фронтенда
String json = "{";
    json += "\"temp_curr\":" + String(data.currentTemp, 1) + ",";
    json += "\"temp_target\":" + String(data.targetTemp, 1) + ",";
    json += "\"relay\":" + String(data.relayStatus ? "true" : "false") + ",";
    json += "\"sensor_ok\":" + String(data.sensorStatus == 0 ? "true" : "false") + ",";
    json += "\"uptime\":" + String(data.uptime) + ",";
    json += "\"mcu_temp\":" + String(data.mcuTemp) + ",";
    json += "\"alive\":" + String(System_State::instance().isStm32Alive() ? "true" : "false");
    json += "}";
return json;
}
