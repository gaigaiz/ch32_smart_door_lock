/*
 * audio.c
 *
 *  Created on: 2025年9月28日
 *      Author: leader
 */

#include "debug.h"
#include "usart.h"
void audio_init()
{
    Usart3_Init();
}
void audio_play(u8 num)
{
    u8 string[]={0x7E,0x05,0x41,0x00,num,0x05^0x41^0x00^num,0xEF};//根据产品使用手册来书写串口协议

    u8 i;
    for(i=0;i<7;i++)
    {
      USART_SendData(USART3, string[i]);
      while(    !USART_GetFlagStatus(USART3, USART_FLAG_TC) );//其中USART_GetFlagStatus-查看串口3发送状态的函数    USART_FLAG_TC-这个是串口3发送完成的标志位(然后该标志位发送完成是会变为1相反1为0)
      //这里在该函数前进行取反是为了当串口3发送完成时，该标志位变为1，通过取反变为0，从而跳出循环，相反则一直持续循环直到发送完成
    }
}
void audio_yinliang(u8 yinliang)//设置语音的音量
{
    u8 string[]={0x7E,0x04,0x31,yinliang,0x04^0x31^yinliang,0xEF};//根据产品使用手册来书写串口协议(音量设置)

    u8 i;
    for(i=0;i<6;i++)//根据数据包协议的大小来进行设置
    {
      USART_SendData(USART3, string[i]);
      while(    !USART_GetFlagStatus(USART3, USART_FLAG_TC) );//其中USART_GetFlagStatus-查看串口3发送状态的函数    USART_FLAG_TC-这个是串口3发送完成的标志位(然后该标志位发送完成是会变为1相反1为0)
      //这里在该函数前进行取反是为了当串口3发送完成时，该标志位变为1，通过取反变为0，从而跳出循环，相反则一直持续循环直到发送完成
    }
}
