#include "Storage_Drv.h"
#include <Arduino.h>

bool Storage_Drv::Init() {
    if (!LittleFS.begin(true)) {
        Serial.println("[Storage] Error mounting LittleFS!");
        return false;
    }

    Serial.println("[Storage] LittleFS mounted successfully.");
    CheckFS();
    return true;
    }

void Storage_Drv::CheckFS() {
    Serial.printf("[Storage] Total space: %d KB, Used: %d KB\n", 
        LittleFS.totalBytes()/1024, LittleFS.usedBytes()/1024);
}
