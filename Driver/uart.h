/*
 * uart.h
 *
 *  Created on: 2025Äê10ÔÂ18ÈÕ
 *      Author: leader
 */

#ifndef DRIVER_UART_H_
#define DRIVER_UART_H_

void Uart7_Init();
void uart7_send_string(u8*string,u8 len);
void usart1_send_string(u8*string,u8 len);
void Uart6_Init();
void uart6_send_string(u8*string,u8 len);
#endif /* DRIVER_UART_H_ */
