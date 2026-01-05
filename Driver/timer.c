/*
 * timer.c
 *
 *  Created on: 2025年9月13日
 *      Author: leader
 */
#include "debug.h"

//定时器3+led显示
unsigned int time_1000ms;
unsigned char led_flag;
void Tim3_Init(u16 arr,u16 psc)//arr-自动重装载的值 psc-分频值
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;//定义结构体
    NVIC_InitTypeDef NVIC_InitStructure;//定义结构体
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);//打开TIM3的时钟(注意该时钟挂载的总线是哪一个APB1/APB2)TIM3-APB1

    TIM_TimeBaseInitStruct.TIM_ClockDivision=0;//时钟分割
    TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;//设置为向上计数
    TIM_TimeBaseInitStruct.TIM_Period=arr;//自动重装载值，5000为500ms
    TIM_TimeBaseInitStruct.TIM_Prescaler=psc;//设置分频值

    TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStruct);
    TIM_ITConfig(TIM3, TIM_IT_Update|TIM_IT_Trigger, ENABLE);



    NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn;//需要记得使用拥有中断通道的时钟
    NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;

    NVIC_Init(&NVIC_InitStructure);

    TIM_Cmd(TIM3, ENABLE);
}
// 定义舵机的PWM周期和占空比
#define PWM_PERIOD 20000  // PWM周期为20ms (20000us)
#define PWM_MIN 500       // 最小脉宽为0.5ms (500us)
#define PWM_MAX 2500      // 最大脉宽为2.5ms (2500us)

// 初始化TIM2以生成PWM信号
void TIM2_PWM_Init(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);  // 使能TIM2时钟

    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;

    // 配置TIM2的基本参数
    TIM_TimeBaseStructure.TIM_Period = PWM_PERIOD - 1;  // 设置自动重装值
    TIM_TimeBaseStructure.TIM_Prescaler = 96 - 1;       // 设置预分频器，96MHz/96 = 1MHz (1us)
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    // 配置TIM2的PWM模式
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = PWM_MIN;  // 初始占空比为最小值
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

    // 配置TIM2通道1
    TIM_OC1Init(TIM2, &TIM_OCInitStructure);  // 使用TIM2的通道1
    TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);  // 使能TIM2通道1的预装载寄存器


    TIM_ARRPreloadConfig(TIM2, ENABLE);               // 使能TIM2的自动重装寄存器预装载

    TIM_Cmd(TIM2, ENABLE);  // 使能TIM2
    //初始化GPIO以输出PWM信号-这里我将定时器2的初始化与它GPIO的初始化放在一起整合为1个函数
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  // 使能GPIOA时钟

        GPIO_InitTypeDef GPIO_InitStructure;

        // 配置PA0为复用推挽输出（TIM2通道1）
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIOA, &GPIO_InitStructure);

}
void lock(unsigned char mode)//舵机的工作模式0-上锁 1-开锁
{

    if(mode==0)//0度
    {
       TIM_SetCompare1(TIM2, 500);//设置占空比
    }else if(mode==1)
    {
       TIM_SetCompare1(TIM2, 1500);//设置占空比

    }
}

void Led_Pwm_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};//定义了一个GPIO的结构体，用来传参

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);//打开了GPIO外设的时钟
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;//GPIO的三种设置(引脚)-由于我选择连接LED1的引脚为PC2
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//GPIO的三种设置(模式)-推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//GPIO的三种设置(速度)
    GPIO_Init(GPIOC, &GPIO_InitStructure);//将结构体的首地址传入

    //这个单片机的时钟是96兆-96 000 000 - 6个0代表兆 3个0代表K,然后我们对于该时钟进行分频，设置分频为96则该单片机1s中可以加1 000 000这么多次，然后我们想把这个做成1ms只需要/1000即可
    //Tim3_Init(1000,96-1);//首先进行分频然后计数到1000停止并归0

}
