#ifndef API_SRV_H_
#define API_SRV_H_

#include <Arduino.h>
#include "System_State.h"

/**
 * @brief Сервис подготовки данных для внешних API (Web/App)
 * Изолирует формат передачи (JSON) от внутренней структуры данных.
 */
class Api_Srv {
    public:
        /**
         * @brief Формирует JSON-строку с текущим состоянием системы
         * @return String JSON-объект
         */
        static String GetSystemStatusJson();

    private:
        // Запрещаем создание экземпляров (статический сервис)
        Api_Srv() = delete;
    };
#endif // API_SRV_H_
