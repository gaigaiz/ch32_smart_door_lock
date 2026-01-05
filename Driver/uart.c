/*
 * uart.c
 *
 *  Created on: 2025年10月18日
 *      Author: leader
 */
#include "debug.h"
#include "uart.h"

void Uart6_Init()
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef  NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART6 , ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC , ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;

    GPIO_Init(GPIOC, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx|USART_Mode_Rx;

    USART_Init(UART6, &USART_InitStructure);

    USART_ITConfig(UART6, USART_IT_RXNE,ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = UART6_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    USART_Cmd(UART6, ENABLE);
    USART_ClearFlag(UART6, USART_FLAG_TC);//清空串口7的发送标志位-防止第一个标志位丢失
}
void Uart7_Init()
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef  NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART7 , ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC , ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3;//2TX 3RX
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;

    GPIO_Init(GPIOC, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx|USART_Mode_Rx;

    USART_Init(UART7, &USART_InitStructure);

    USART_ITConfig(UART7, USART_IT_RXNE,ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = UART7_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    USART_Cmd(UART7, ENABLE);
    USART_ClearFlag(UART7, USART_FLAG_TC);//清空串口7的发送标志位-防止第一个标志位丢失
}

void uart6_send_string(u8*string,u8 len)
{
    u8 i;
    for(i=0;i<len;i++)
    {
        USART_SendData(UART6, string[i]);
        while(!USART_GetFlagStatus(UART6, USART_FLAG_TC) );//其中USART_GetFlagStatus-查看串口7发送状态的函数    USART_FLAG_TC-这个是串口7发送完成的标志位(然后该标志位发送完成是会变为1相反1为0)
    }
}

void uart7_send_string(u8*string,u8 len)
{
    u8 i;
    for(i=0;i<len;i++)
    {
        USART_SendData(UART7, string[i]);
        while(!USART_GetFlagStatus(UART7, USART_FLAG_TC) );//其中USART_GetFlagStatus-查看串口7发送状态的函数    USART_FLAG_TC-这个是串口7发送完成的标志位(然后该标志位发送完成是会变为1相反1为0)
    }
}

void usart1_send_string(u8*string,u8 len)
{
    u8 i;
    for(i=0;i<len;i++)
    {
        USART_SendData(USART1, string[i]);
        while(!USART_GetFlagStatus(USART1, USART_FLAG_TC) );//其中USART_GetFlagStatus-查看串口1发送状态的函数    USART_FLAG_TC-这个是串口1发送完成的标志位(然后该标志位发送完成是会变为1相反1为0)
    }
}
