/*
 * as608.h
 *
 *  Created on: 2025Äê10ÔÂ18ÈÕ
 *      Author: leader
 */

#ifndef DRIVER_AS608_H_
#define DRIVER_AS608_H_

void as608_init();
void As608_GPIO_Init();
void PS_GetImage();
void PS_GenCha(u8 BufferID);
void PS_RegModel();
void PS_StoreChar(u8 addr);
void PS_Search();
void PS_Empty();
#endif /* DRIVER_AS608_H_ */
