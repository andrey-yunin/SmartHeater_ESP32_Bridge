#ifndef STORAGE_DRV_H_
#define STORAGE_DRV_H_

#include <LittleFS.h>

/**
 * @brief Драйвер файловой системы (LittleFS)
 */
class Storage_Drv {
public:
    // Инициализация и монтирование FS
    static bool Init();
    // Вывод информации о свободном месте
    static void CheckFS();

private:
    Storage_Drv() = delete; // Статический сервис
};

#endif
