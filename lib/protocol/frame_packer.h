/*
 * frame_packer.h
 *
 *  Created on: Mar 16, 2026
 *      Author: andrey
 */

#ifndef INC_FRAME_PACKER_H_
#define INC_FRAME_PACKER_H_


#include <stdint.h>
#include <stdbool.h>

#define FRAME_START_MARKER '$'
#define MAX_DATA_LEN       16
#define PACKET_TIMEOUT_MS  50

/**
 * @brief Структура фрейма протокола v1.2
 */
typedef struct __attribute__((packed)) {
uint8_t cmd;
uint8_t len;
uint8_t data[MAX_DATA_LEN];
uint8_t crc;
} Frame_t;

/**
 * @brief Состояние сборщика
 */
typedef enum {
	PACKER_IDLE,       // Ожидание старта
    PACKER_COLLECTING, // Сборка в процессе
    PACKER_READY,      // Фрейм готов и CRC верна
    PACKER_ERROR       // Ошибка CRC или таймаут
	} PackerStatus_t;

/**
 * @brief Задача сборщика фреймов. Вызывается в главном цикле.
 * @param frame Указатель на структуру, куда будет записан готовый фрейм.
 * @return Статус сборки.
 */
PackerStatus_t FramePacker_Task(Frame_t *frame);


#endif /* INC_FRAME_PACKER_H_ */
