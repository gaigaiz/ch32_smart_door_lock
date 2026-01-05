#include "debug.h"
/*
 * OUT PP
 * R4 PD11
 * R3 PD9
 * R2 PE15
 * R1 PE13
 * IPU
 * C1 PE11
 * C2 PE9
 * C3 PE7
 * C4 PC5
 * */

void key_init()
{
        //初始化GPIO
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);  // 使能GPIOD时钟
        GPIO_InitTypeDef GPIO_InitStructure;
        // 配置PA0为复用推挽输出（TIM2通道1）
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11|GPIO_Pin_9;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//推挽输出
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIOD, &GPIO_InitStructure);

        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);  // 使能GPIOD时钟

        // 配置PA0为复用推挽输出（TIM2通道1）
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15|GPIO_Pin_13;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIOE, &GPIO_InitStructure);

        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);  // 使能GPIOD时钟

        // 配置PA0为复用推挽输出（TIM2通道1）
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11|GPIO_Pin_9|GPIO_Pin_7;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//上拉输出
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIOE, &GPIO_InitStructure);

        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);  // 使能GPIOD时钟

        // 配置PA0为复用推挽输出（TIM2通道1）
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIOC, &GPIO_InitStructure);
}
u8 key_read()
{
    u8 temp=0;
    //R4
    GPIO_ResetBits(GPIOD, GPIO_Pin_11);GPIO_SetBits(GPIOD, GPIO_Pin_9);GPIO_SetBits(GPIOE, GPIO_Pin_15); GPIO_SetBits(GPIOE, GPIO_Pin_13);//设置首行为低电平然后列扫描(后续同上)
    if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_11)==0)temp=4;
    if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_9)==0)temp=3;
    if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_7)==0)temp=2;
    if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_5)==0)temp=1;//C4
    //R3
    GPIO_SetBits(GPIOD, GPIO_Pin_11);GPIO_ResetBits(GPIOD, GPIO_Pin_9);GPIO_SetBits(GPIOE, GPIO_Pin_15); GPIO_SetBits(GPIOE, GPIO_Pin_13);
    if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_11)==0)temp=8;//C1
    if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_9)==0)temp=7;//C2
    if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_7)==0)temp=6;//C3
    if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_5)==0)temp=5;//C4
    //R2
    GPIO_SetBits(GPIOD, GPIO_Pin_11);GPIO_SetBits(GPIOD, GPIO_Pin_9);GPIO_ResetBits(GPIOE, GPIO_Pin_15); GPIO_SetBits(GPIOE, GPIO_Pin_13);
    if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_11)==0)temp=12;
    if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_9)==0)temp=11;
    if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_7)==0)temp=10;
    if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_5)==0)temp=9;
    //R1
    GPIO_SetBits(GPIOD, GPIO_Pin_11);GPIO_SetBits(GPIOD, GPIO_Pin_9);GPIO_SetBits(GPIOE, GPIO_Pin_15); GPIO_ResetBits(GPIOE, GPIO_Pin_13);
    if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_11)==0)temp=16;
    if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_9)==0)temp=15;
    if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_7)==0)temp=14;
    if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_5)==0)temp=13;

    return temp;
}
