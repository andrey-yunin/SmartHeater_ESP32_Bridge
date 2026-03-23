#include "Network_Task.h"
#include "WiFi_Drv.h"
#include "Api_Srv.h"
#include "Cmd_Srv.h" // Добавили для управления
#include <LittleFS.h> // Добавили для работы с файлами



// Создаем веб-сервер на стандартном 80 порту
static WebServer server(80);


/**
 * @brief Автоматическая раздача файлов из LittleFS
 */
bool handleFileRead(String path) {
    // Если путь корень, ищем index.html

    if (path.endsWith("/")) path += "index.html";

    // Определяем тип контента (MIME)
    String contentType = "text/plain";
    if (path.endsWith(".html")) contentType = "text/html";
    else if (path.endsWith(".css"))  contentType = "text/css";
    else if (path.endsWith(".js"))   contentType = "application/javascript";
    else if (path.endsWith(".json")) contentType = "application/json";

    // Если файл существует - отдаем его
    if (LittleFS.exists(path)) {
        File file = LittleFS.open(path, "r");
        server.streamFile(file, contentType);
        file.close();
        return true;
    }
    return false;
    }

/**
 * @brief Обработчик запроса статуса (/api/status)
 */
void handleStatus() {
    server.send(200, "application/json", Api_Srv::GetSystemStatusJson());
}

/**
 * @brief Обработчик установки температуры (/api/set_temp?val=25.5)
 */
void handleSetTemp() {
    if (server.hasArg("val")) {
        float val = server.arg("val").toFloat();
        // Протокол v1.2 требует int16_t (Значение * 10)
        int16_t raw = (int16_t)(val * 10.0f);
        uint8_t data[2];
        data[0] = (uint8_t)(raw >> 8);   // High byte (Big-Endian)
        data[1] = (uint8_t)(raw & 0xFF); // Low byte

        // Отправляем в очередь команду 0x13 (SET_TARGET)
        if (Cmd_Srv::Enqueue(0x13, data, 2)) {
            // ОПТИМИСТИЧНОЕ ОБНОВЛЕНИЕ: Записываем в состояние сразу, не дожидаясь ответа STM32
            System_State::instance().SetTargetTemp(val);
            Serial.printf("[NetTask] Command -> SET_TEMP: %.1f (0x%04X)\n", val, raw);
            server.send(200, "application/json", "{\"status\":\"ok\"}");            
        }
        else {
            server.send(500, "application/json", "{\"status\":\"error\",\"msg\":\"Queue full\"}");
        }
    }
    else {
        server.send(400, "application/json", "{\"status\":\"error\",\"msg\":\"Missing val\"}");
    }
}



/**
 * @brief Обработчик корневой страницы (/)
 * Отдает минимальный HTML с JS-скриптом для мониторинга
 *
void handleRoot() {
    String html = "<html><head><meta charset='UTF-8'><title>Smart Heater</title>";
        html += "<style>body{font-family:sans-serif;text-align:center;padding:20px;}";
        html += "pre{background:#f4f4f4;padding:15px;display:inline-block;text-align:left;}";
        html += "button{padding:10px 20px;margin:5px;cursor:pointer;}</style>";
        html += "</head><body>";
        html += "<h1>Управление Обогревателем</h1>";
        
        // Кнопки управления
        html += "<div><button onclick='setTemp(25)'>Установить 25°C</button>";
        html += "<button onclick='setTemp(18)'>Установить 18°C</button></div>";
        
        html += "<pre id='data'>Загрузка данных...</pre>";
        
        html += "<script>";
        html += "function setTemp(v){ fetch('/api/set_temp?val='+v).then(r=>r.json()).then(j=>console.log(j)); }";
        html += "setInterval(() => {";
        html += "  fetch('/api/status').then(r => r.json()).then(j => {";
        html += "    document.getElementById('data').innerHTML = JSON.stringify(j, null, 2);";
        html += "  });";
        html += "}, 1000);</script></body></html>";

        
        server.send(200, "text/html", html);
}
 
*/

void Network_Task_Init() {
    // Создаем задачу на Core 0 (ядро WiFi/BT стека)
    xTaskCreatePinnedToCore(
        Network_Task_Loop,
        "NetTask",
        8192,  // Стек с запасом для обработки HTTP
        NULL,
        5,     // Средний приоритет
        NULL,
        0      // Core 0
    );
}

void Network_Task_Loop(void *pvParameters) {
    Serial.println("[NetTask] Started on Core 0");
   
    // РЕГИСТРАЦИЯ МАРШРУТОВ
    server.on("/api/status", handleStatus);
    server.on("/api/set_temp", handleSetTemp);
   
    // ВАЖНО: Если запрос не к API, ищем файл в LittleFS
    server.onNotFound([]() {
        if (!handleFileRead(server.uri())) {
            server.send(404, "text/plain", "File Not Found");
        }
    });

    server.begin();
    Serial.println("[NetTask] Web Server ready for static files from LittleFS");
   
    while (1) {
        WiFi_Drv::Update();
        server.handleClient();
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

