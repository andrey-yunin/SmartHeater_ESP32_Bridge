# 🏗️ Архитектура проекта: ESP32 Smart Bridge (RTOS Edition)

## 1. Концепция
Система строится на базе операционной системы реального времени **FreeRTOS** с использованием **Task-Oriented Architecture**. Программный код разделен на функциональные слои для обеспечения модульности, тестируемости и отказоустойчивости.

Ключевые принципы:
*   **Изоляция ресурсов:** Каждая задача владеет своими ресурсами (UART, WiFi) и общается с другими через потокобезопасные примитивы.
*   **Data Consistency:** Использование паттерна "Snapshot" и мьютексов для атомарного чтения/записи состояния.
*   **Separation of Concerns:** Четкое разделение драйверов железа, бизнес-логики и потоков исполнения.

---

## 2. Файловая структура (Layered Layout)

```text
/
├── App/                         # Основная логика приложения (Вне src)
│   ├── Core/                    # Слой 1: Фундамент и Общие данные (Passive)
│   │   ├── inc/                 # SystemState.h, Config.h, EventGroups.h
│   │   └── src/                 # SystemState.cpp
│   ├── Drivers/                 # Слой 0: Низкоуровневые адаптеры (HAL)
│   │   ├── inc/                 # Uart_Drv.h, WiFi_Drv.h, Storage_Drv.h
│   │   └── src/                 # Uart_Drv.cpp, WiFi_Drv.cpp, Storage_Drv.cpp
│   ├── Services/                # Слой 2: Бизнес-логика и Обработка (Logic)
│   │   ├── inc/                 # Protocol_Srv.h, Api_Srv.h, Cmd_Srv.h, Logger_Srv.h
│   │   └── src/                 # Protocol_Srv.cpp, Api_Srv.cpp, Cmd_Srv.cpp, Logger_Srv.cpp
│   └── Tasks/                   # Слой 3: Потоки исполнения (Active Tasks)
│       ├── inc/                 # Uart_Task.h, Network_Task.h, System_Task.h
│       └── src/                 # Uart_Task.cpp, Network_Task.cpp, System_Task.cpp
├── src/
│   └── main.cpp                 # Точка входа: инит слоев и запуск планировщика
├── platformio.ini               # Конфигурация проекта
└── ...
```

---

## 3. Описание слоев (Layer Responsibilities)

### 3.1. App/Core (Passive Foundation)
*   **SystemState:** "Единственный источник правды". Хранит текущую температуру, уставку, статус реле и время последнего обновления. Все поля защищены мьютексом. Предоставляет метод `GetSnapshot()` для получения согласованной копии данных.
*   **EventGroups:** Определяет битовые флаги событий (например, `BIT_WIFI_CONNECTED`, `BIT_DATA_RECEIVED`), которые позволяют задачам синхронизироваться без активного опроса.

### 3.2. App/Drivers (Hardware Abstraction)
*   **Uart_Drv:** Инициализирует `Serial2`. Предоставляет методы `ReadByte()` и `SendBytes()`. Скрывает детали реализации Arduino API.
*   **WiFi_Drv:** Отвечает за подключение к точке доступа, обработку событий `SYSTEM_EVENT_STA_GOT_IP` и переподключение при обрыве.
*   **Storage_Drv:** Инициализирует LittleFS, читает/пишет конфигурационные файлы JSON.

### 3.3. App/Services (Business Logic)
*   **Protocol_Srv:** Знает структуру пакета v1.2. Парсит поток байт из UART, проверяет CRC. Формирует байтовые массивы команд. **Не имеет своего цикла**, вызывается из задач.
*   **Api_Srv:** Формирует JSON-ответы для фронтенда. Разбирает входящие HTTP-запросы и валидирует параметры.
*   **Cmd_Srv:** Обертка над FreeRTOS Queue. Позволяет любой задаче (например, Web) безопасно положить команду в очередь на отправку.

### 3.4. App/Tasks (Active Execution)
*   **Uart_Task (Core 1, High Priority):**
    *   Цикл `while(1)`:
        1.  Читает байты из `Uart_Drv`.
        2.  Скармливает их `Protocol_Srv`.
        3.  Если пакет готов -> обновляет `SystemState`.
        4.  Проверяет `Cmd_Srv` (очередь команд). Если есть команда -> отправляет через `Uart_Drv`.
*   **Network_Task (Core 0, Medium Priority):**
    *   Поддерживает WiFi соединение.
    *   Запускает `ESPAsyncWebServer`.
    *   Обрабатывает HTTP-запросы в контексте своих коллбеков.
*   **System_Task (Core 0, Low Priority):**
    *   Мигает светодиодом (Heartbeat).
    *   Следит за свободной памятью (Heap Monitor).
    *   Проверяет флаг `STM32_ALIVE` (Watchdog связи).

---

## 4. Потоки данных (Data Flow)

### 4.1. Обновление состояния (STM32 -> Web)
1.  **STM32** шлет пакет.
2.  **Uart_Task** принимает байты -> `Protocol_Srv` собирает фрейм.
3.  `Protocol_Srv` валидирует CRC -> вызывает `SystemState::Update()`.
4.  `SystemState` захватывает Mutex, обновляет поля, освобождает Mutex.
5.  **Browser** шлет GET запрос `/api/status`.
6.  **Network_Task** вызывает `Api_Srv::GetStatusJson()`.
7.  `Api_Srv` вызывает `SystemState::GetSnapshot()` -> получает атомарную копию данных.
8.  `Api_Srv` формирует JSON -> WebServer отдает его браузеру.

### 4.2. Управление (Web -> STM32)
1.  **Browser** шлет POST запрос `/api/control`.
2.  **Network_Task** парсит параметры -> вызывает `Cmd_Srv::Enqueue(CMD_SET_TEMP, 25.0)`.
3.  `Cmd_Srv` кладет команду в `xQueue`.
4.  **Uart_Task** в своем цикле видит данные в очереди.
5.  Вызывает `Protocol_Srv::PackCommand()` -> получает байты.
6.  Вызывает `Uart_Drv::Send()` -> байты уходят на STM32.

---

## 5. Конфигурация сборки (PlatformIO)
Для корректной компиляции папки `App` в корне необходимо добавить следующие настройки в `platformio.ini`:
```ini
build_flags = 
    -I App/Core/inc
    -I App/Drivers/inc
    -I App/Services/inc
    -I App/Tasks/inc

# Включаем файлы из папки App в процесс сборки
build_src_filter = +<*> +<../App/>
```
