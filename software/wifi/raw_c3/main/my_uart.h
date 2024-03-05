/*
 * my_uart.h
 *
 *  Created on: Oct 25, 2021
 *      Author: gideon
 */

#ifndef MY_UART_H_
#define MY_UART_H_

#include "esp_err.h"
#include "cmd_buffer.h"

esp_err_t  my_uart_init(command_buf_context_t *buffers, uint8_t uart_num);
BaseType_t my_uart_get_buffer(uint8_t uart_num, command_buf_t **buf, TickType_t ticks);
BaseType_t my_uart_transmit_packet(uint8_t uart_num, command_buf_t *buf);
BaseType_t my_uart_receive_packet(uint8_t uart_num, command_buf_t **buf, TickType_t ticks);
BaseType_t my_uart_free_buffer(uint8_t uart_num, command_buf_t *buf);

#endif /* MY_UART_H_ */
