#ifndef CONFIG_H_
#define CONFIG_H_

// Параметры Wi-Fi (замените на свои при прошивке)
#define WIFI_SSID       "YOUR_SSID"
#define WIFI_PASS       "YOUR_PASSWORD"

// Таймауты и интервалы
#define WIFI_RECONNECT_MS 5000    // Интервал попыток переподключения
#define STM32_ALIVE_MS    5000    // Порог "живучести" связи с STM32

// Настройки UART
#define UART_BAUD         115200
#define UART_RX_PIN       16      // GPIO16 (RX2)
#define UART_TX_PIN       17      // GPIO17 (TX2)

#endif // CONFIG_H_

