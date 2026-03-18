# 📋 Roadmap: ESP32 Smart Bridge (RTOS Edition)

## Фаза 1: Инфраструктура (Core & Foundation)
*Цель: Создать надежный фундамент для RTOS-приложения.*

- [x] **1.1. Адаптация протокола**
    - [x] Анализ `lib/protocol` и портирование логики v1.2 в `Protocol_Srv`.
- [x] **1.2. Настройка проекта PlatformIO**
    - [x] Обновить `platformio.ini` (build_flags, src_filter).
    - [x] Создать структуру папок `App/...` в корне проекта.
- [ ] **1.3. Реализация App/Core (Vertical Slice Goal)**
    - [ ] `SystemState.h/cpp`: Структура данных (HEALTH_DATA v1.2), Mutex, Singleton.
    - [ ] Парсинг Big-Endian данных из пакетов UART (0x03, 0x04).
    - [ ] `EventGroups.h`: Определение битовых флагов.
    - [ ] `Config.h`: Глобальные константы.

## Фаза 2: Драйверы и Сервисы (Drivers & Logic)
*Цель: Реализовать работу с железом и бизнес-логику без привязки к задачам.*

- [ ] **2.1. App/Drivers**
    - [x] `Uart_Drv`: Базовая инициализация Serial2.
    - [x] `Fifo_Drv`: ООП-реализация кольцевого буфера на базе `lib/protocol`.
    - [x] Интеграция `Fifo_Drv` в `Uart_Drv` для фонового приема данных.
    - [ ] `WiFi_Drv`: Базовая логика подключения (без веб-сервера).
- [ ] **2.2. App/Services**
    - [x] `Protocol_Srv`: Реализация парсера и упаковщика фреймов v1.2.
    - [ ] `Logger_Srv`: Потокобезопасный логгер.
    - [ ] `Cmd_Srv`: Очередь команд (xQueue wrapper).

## Фаза 3: Задачи RTOS (Execution)
*Цель: Оживить систему, запустив параллельные потоки.*

- [ ] **3.1. Uart_Task (Core 1)**
    - [x] Создание задачи FreeRTOS на Core 1.
    - [x] Сквозной тест связи (PING/ACK) с STM32.
    - [ ] Интеграция with `SystemState` для сохранения данных `0x04`.
- [ ] **3.2. Network_Task (Core 0)**
    - [ ] Запуск WiFi (через Driver) и WebServer.
    - [ ] Реализация API эндпоинтов (через `Api_Srv`).
- [ ] **3.3. System_Task** (опционально)
    - [ ] Heartbeat LED, Watchdog.

## Фаза 4: Веб-интерфейс (Frontend)
*Цель: Пользовательский UI.*

- [ ] **4.1. API Layer**
    - [ ] `Api_Srv`: Генерация JSON status.
    - [ ] Обработка POST запросов управления.
- [ ] **4.2. SPA Client**
    - [ ] HTML/CSS/JS файлы.
    - [ ] Сборка и прошивка LittleFS.

## Фаза 5: Интеграция и Тесты
*Цель: Проверка стабильности.*

- [ ] **5.1. Stress Test**
    - [ ] Проверка на утечки памяти (Heap check).
    - [ ] Тест на восстановление связи (WiFi/UART).
