# 📋 Roadmap: ESP32 Smart Bridge (RTOS Edition)

## Фаза 1: Инфраструктура (Core & Foundation)
*Цель: Создать надежный фундамент для RTOS-приложения.*

- [x] **1.1. Адаптация протокола** (выполнено ранее, требует переноса в новую структуру)
    - [x] `lib/protocol` (остается как legacy или интегрируется в Services)
    - [x] Анализ и портирование логики v1.2 в `Protocol_Srv`.
- [x] **1.2. Настройка проекта PlatformIO**
    - [x] Обновить `platformio.ini` (build_flags, src_filter).
    - [x] Создать структуру папок `App/...` в корне проекта.
- [x] **1.3. Реализация App/Core (Vertical Slice Goal)** — *ЗАВЕРШЕНО*
    - [x] `SystemState.h/cpp`: Структура данных (HEALTH_DATA v1.2), Mutex, Singleton.
    - [x] Парсинг Big-Endian данных из пакетов UART (0x03, 0x04).
    - [x] `EventGroups.h`: Определение битовых флагов (SysEvent).
    - [x] `Config.h`: Глобальные константы и настройки WiFi.

## Фаза 2: Драйверы и Сервисы (Drivers & Logic)
*Цель: Реализовать работу с железом и бизнес-логику без привязки к задачам.*

- [x] **2.1. App/Drivers**
    - [x] `Uart_Drv`: Инициализация Serial2, чтение/запись.
    - [x] `Fifo_Drv`: ООП-реализация кольцевого буфера на базе `lib/protocol`.
    - [x] Интеграция `Fifo_Drv` в `Uart_Drv` для фонового приема данных.
    - [x] `WiFi_Drv`: Реализован **Pure AP Mode** (Точка доступа SmartHeater_XXXX).
- [ ] **2.2. App/Services**
    - [ ] `Logger_Srv`: Потокобезопасный логгер.
    - [x] `Cmd_Srv`: Очередь команд (xQueue wrapper) - Инициализирована.
    - [x] `Protocol_Srv`: Интеграция парсера v1.2 (из lib/protocol).
    - [x] **Api_Srv:** Реализован прототип маппинга JSON (ручная сборка для MVP).

## Фаза 3: Задачи RTOS (Execution) — *ЗАВЕРШЕНО*
*Цель: Оживить систему, запустив параллельные потоки.*

- [x] **3.1. Uart_Task (Core 1)**
    - [x] Цикл приема данных, обновление `SystemState`.
    - [x] Сквозной тест связи (PING/ACK) с STM32.
    - [x] Интеграция с `SystemState` для сохранения данных `0x03` и `0x04`.
    - [x] **Отправка команд из `Cmd_Srv`**: Интеграция очереди в цикл задачи.
- [x] **3.2. Network_Task (Core 0)**
    - [x] Запуск WiFi (в режиме Pure AP Mode).
    - [x] **WebServer (MVP):** Реализация на базе стандартной `WebServer.h`.
    - [x] Реализация эндпоинта GET `/api/status` через `Api_Srv`.
    - [x] **Эндпоинт управления `/api/set_temp`**: Передача команд в `Cmd_Srv`.
- [ ] **3.3. System_Task** (опционально)
    - [ ] Heartbeat LED, Watchdog, Heap Monitor.

## Фаза 4: Веб-интерфейс (Frontend) — *ЗАВЕРШЕНО*
*Цель: Пользовательский UI.*

- [x] **4.1. Разработка UI Дизайна:**
    - [x] Создание концепции `UI_DESIGN.md` (v1.1).
    - [x] Верстка `index.html`, `style.css`, `app.js` (State-driven animations).
- [x] **4.2. Инфраструктура хранения:**
    - [x] Настройка LittleFS в `platformio.ini`.
    - [x] Реализация `Storage_Drv` для монтирования файловой системы.
    - [x] Настройка `Network_Task` на раздачу статических файлов.


## Фаза 5: Интеграция и Реактивность (ТЕКУЩИЙ ЭТАП) 🕒
- [x] 5.1. Проектирование гибридной модели "Smart Watchdog Listener".
    - [x] Согласован контракт: STM32 - ведущий (Push), ESP32 - ведомый (Listen + Fallback Poll).
- [ ] 5.2. Реализация логики Watchdog в Uart_Task.cpp:
    - [ ] Контроль `millis() - lastUpdate`.
    - [ ] Автоматическая отправка `0x10 (PING)` при превышении порога 65с.
- [ ] 5.3. Оптимизация Uart_Task для минимального потребления CPU в ожидании данных.
- [ ] 5.4. Сквозной тест: "Живые" данные с ADC STM32 -> Отображение в UI.

## Фаза 6: Рефакторинг и Оптимизация (Advanced C++)
- [ ] **6.1. Сетевой уровень (Industrial Transition):**
    - [ ] Переход на **ArduinoJson** и **ESPAsyncWebServer**.
- [ ] **6.2. Абстракция и Реактивность (Interfaces & Event Bus).**
