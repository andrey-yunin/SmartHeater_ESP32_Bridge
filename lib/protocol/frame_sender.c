/*
 * frame_sender.c
 *
 *  Created on: Mar 16, 2026
 *      Author: andrey
 */

#include "frame_sender.h"
#include "frame_packer.h" // Ради FRAME_START_MARKER
#include "uart.h"

void FrameSender_SendFrame(uint8_t cmd, uint8_t *data, uint8_t len) {
	uint8_t xor_sum = 0;

	// 1. Стартовый маркер
	UART1_SendChar(FRAME_START_MARKER);

	// 2. Команда
	UART1_SendChar(cmd);
    xor_sum ^= cmd;

     // 3. Длина
     UART1_SendChar(len);
     xor_sum ^= len;

     // 4. Данные + расчет XOR
     for (uint8_t i = 0; i < len; i++) {
    	 UART1_SendChar(data[i]);
         xor_sum ^= data[i];
         }

     // 5. Контрольная сумма
     UART1_SendChar(xor_sum);
     }

void FrameSender_SendACK(uint8_t cmd_id) {
	FrameSender_SendFrame(0x01, &cmd_id, 1);
	}

void FrameSender_SendNACK(uint8_t err_code) {
	FrameSender_SendFrame(0x02, &err_code, 1);
	}

