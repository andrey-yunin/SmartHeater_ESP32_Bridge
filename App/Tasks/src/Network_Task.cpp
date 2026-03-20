#include "Network_Task.h"
#include "WiFi_Drv.h"
#include "Api_Srv.h"
#include "Cmd_Srv.h" // Добавили для управления


// Создаем веб-сервер на стандартном 80 порту
static WebServer server(80);

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
            server.send(200, "application/json", "{\"status\":\"ok\"}");
            Serial.printf("[NetTask] Web Command -> SET_TEMP: %.1f\n", val);
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
 */
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
 
/**
 * @brief Обработчик запроса статуса (/api/status)
 */
void handleStatus() {
    server.send(200, "application/json", Api_Srv::GetSystemStatusJson());
}

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

    // РЕГИСТРАЦИЯ МАРШРУТОВ (ЭНДПОИНТОВ)
    // ---------------------------------
    // 1. Корень - отдает HTML страницу   
    server.on("/", handleRoot);

    // 2. Статус - отдает JSON данные
    server.on("/api/status", handleStatus);

    // 3. Управление - ВАЖНО: без нее выдает ошибку 404
    server.on("/api/set_temp", handleSetTemp); 


    server.begin();
    Serial.println("[NetTask] Web Server started");

    while (1) {
        // WiFi_Drv::Update() теперь живет в этом потоке
        WiFi_Drv::Update();

        // Обработка HTTP-запросов
        server.handleClient();

        // Пауза для планировщика FreeRTOS
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
