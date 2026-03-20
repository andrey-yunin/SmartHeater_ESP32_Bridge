#ifndef WIFI_DRV_H_
#define WIFI_DRV_H_

#include <WiFi.h>
#include "Event_Groups.h"
#include "Config.h" // Здесь должны быть SSID и PASS

class WiFi_Drv {
    public:
        /**
         * @brief Инициализация и запуск процесса подключения
         */
        static void Init();
   
        /**
         * @brief Проверка статуса (вызывается периодически или по событиям)
         */
        static void Update();
   
        /**
         * @brief Возвращает локальный IP в виде строки
         */
        static String GetIP();    

    private:
        // Запрещаем создание объектов (удаляем конструктор по умолчанию)
        WiFi_Drv() = delete; 
    };

   
#endif // WIFI_DRV_H_
