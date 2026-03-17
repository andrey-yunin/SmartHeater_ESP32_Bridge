/*
 * frame_packer.c
 *
 *  Created on: Mar 16, 2026
 *      Author: andrey
 */


#include "frame_packer.h"
#include "fifo_handler.h"
#include "sys_time.h"

typedef enum {
	STATE_WAIT_START,
	STATE_WAIT_CMD,
    STATE_WAIT_LEN,
    STATE_WAIT_DATA,
    STATE_WAIT_CRC
	} PackerState_t;

static PackerState_t current_state = STATE_WAIT_START;
static uint8_t data_counter = 0;
static uint8_t calculated_xor = 0;
static uint32_t last_byte_time = 0;

PackerStatus_t FramePacker_Task(Frame_t *frame) {
	uint8_t byte;

	// 1. Проверка тайм-аута сборки (защита от "зависших" полупакетов)
	if (current_state != STATE_WAIT_START) {
		if ((get_uptime_ms() - last_byte_time) > PACKET_TIMEOUT_MS) {
			current_state = STATE_WAIT_START;
			return PACKER_ERROR;
			}
		}

	// 2. Обработка всех доступных байт из FIFO
    while (FIFO_Pop(&byte)) {
    	last_byte_time = get_uptime_ms(); // Обновляем время последнего байта
    	switch (current_state) {
    		case STATE_WAIT_START:
    			if (byte == FRAME_START_MARKER) {
    				current_state = STATE_WAIT_CMD;
    				calculated_xor = 0;
    				}
    			break;

    		case STATE_WAIT_CMD:
    			frame->cmd = byte;
    			calculated_xor = byte;
    			current_state = STATE_WAIT_LEN;
    			break;

    		case STATE_WAIT_LEN:
    			if (byte > MAX_DATA_LEN) {
    				current_state = STATE_WAIT_START;
    				return PACKER_ERROR;
    				}
    			frame->len = byte;
    			calculated_xor ^= byte;
    			data_counter = 0;
    			current_state = (frame->len == 0) ? STATE_WAIT_CRC : STATE_WAIT_DATA;
    			break;

    		case STATE_WAIT_DATA:
    			frame->data[data_counter++] = byte;
    			calculated_xor ^= byte;
    			if (data_counter >= frame->len) {
    				current_state = STATE_WAIT_CRC;
    				}
    			break;

    		case STATE_WAIT_CRC:
    			frame->crc = byte;
    			current_state = STATE_WAIT_START;
    			if (frame->crc == calculated_xor) {
    				return PACKER_READY; // Успех!
    				}
    			else {
    				return PACKER_ERROR; // Ошибка чексуммы
    				}

    			default:
    				current_state = STATE_WAIT_START;
    				break;
    		}


    	// Если фрейм готов или произошла ошибка в процессе цикла, выходим сразу
    	// (но обычно мы возвращаем READY только в конце кейса CRC)
    	}
	return (current_state == STATE_WAIT_START) ? PACKER_IDLE : PACKER_COLLECTING;
}



