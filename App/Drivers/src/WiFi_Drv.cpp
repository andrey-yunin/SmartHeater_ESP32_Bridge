#include "WiFi_Drv.h"

void WiFi_Drv::Init() {
    // 1. Считываем MAC-адрес
    String mac = WiFi.macAddress();
    Serial.println("[WiFi] Hardware MAC: " + mac);
    
    // 2. Включаем ТОЛЬКО режим Точки Доступа (WIFI_AP)
    // Это отключит попытки поиска внешних роутеров и уберет шум в UART.
    WiFi.mode(WIFI_AP);
        
    // Имя сети: SmartHeater_B94C
    String apName = "SmartHeater_" + mac.substring(mac.length() - 5);
    apName.replace(":", "");
        
    // Запуск точки доступа
    WiFi.softAP(apName.c_str(), "12345678"); 
        
    Serial.println("[WiFi] Mode: Pure Access Point");
    Serial.println("[WiFi] SSID: " + apName);
    Serial.println("[WiFi] AP IP: " + WiFi.softAPIP().toString());
}

void WiFi_Drv::Update() {
    // В режиме чистой AP нам не нужно отслеживать статус подключения к роутеру,
    // так как мы сами являемся роутером. 
    // Но для будущего мы можем выставить бит готовности сети сразу.
    static bool ready = false;
    if (!ready && systemEvents != NULL) {
        xEventGroupSetBits(systemEvents, SysEvent::WIFI_CONNECTED);
        ready = true;
    }
}

String WiFi_Drv::GetIP() {
    return WiFi.softAPIP().toString();
}



