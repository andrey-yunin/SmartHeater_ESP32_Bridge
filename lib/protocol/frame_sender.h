/*
 * frame_sender.h
 *
 *  Created on: Mar 16, 2026
 *      Author: andrey
 */

#ifndef INC_FRAME_SENDER_H_
#define INC_FRAME_SENDER_H_


#include <stdint.h>

/**
 * @brief Отправка ACK (Подтверждение выполнения)
 * @param cmd_id ID команды, которую подтверждаем
 */
void FrameSender_SendACK(uint8_t cmd_id);

/**
 * @brief Отправка NACK (Ошибка)
 * @param err_code Код ошибки (0x01-CRC, 0x02-ID и т.д.)
 */
void FrameSender_SendNACK(uint8_t err_code);

/**
 * @brief Универсальная функция отправки любого фрейма
 */
void FrameSender_SendFrame(uint8_t cmd, uint8_t *data, uint8_t len);




#endif /* INC_FRAME_SENDER_H_ */
