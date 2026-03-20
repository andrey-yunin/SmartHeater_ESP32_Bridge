#include "Network_Task.h"
#include "WiFi_Drv.h"
#include "Api_Srv.h"

// Создаем веб-сервер на стандартном 80 порту
static WebServer server(80);

/**
 * @brief Обработчик корневой страницы (/)
 * Отдает минимальный HTML с JS-скриптом для мониторинга
 */
void handleRoot() {
    String html = "<html><head><meta charset='UTF-8'><title>Smart Heater</title>";
        html += "<style>body{font-family:sans-serif;text-align:center;padding:20px;}";
        html += "pre{background:#f4f4f4;padding:15px;display:inline-block;text-align:left;}</style>";
        html += "</head><body>";
        html += "<h1>Мониторинг Обогревателя</h1>";
        html += "<pre id='data'>Получение данных...</pre>";
        html += "<script>setInterval(() => {";
        html += "  fetch('/api/status').then(r => r.json()).then(j => {";
        html += "    document.getElementById('data').innerHTML = JSON.stringify(j, null, 2);";
        html += "  }).catch(e => document.getElementById('data').innerHTML = 'Ошибка связи');";
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
    // Настраиваем маршруты
    server.on("/", handleRoot);
    server.on("/api/status", handleStatus);

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
