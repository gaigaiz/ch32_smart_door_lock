#include "debug.h"
#include "iic.h"
//SCL PE6
//SDA PE5
//---------------------------------------------------------------
#define I2C_SCL_0   GPIO_ResetBits(GPIOE, GPIO_Pin_6)
#define I2C_SCL_1   GPIO_SetBits(GPIOE, GPIO_Pin_6)
#define I2C_SDA_0   GPIO_ResetBits(GPIOE, GPIO_Pin_5)
#define I2C_SDA_1   GPIO_SetBits(GPIOE, GPIO_Pin_5)
#define I2C_SDA     GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_5)
//--------------------管脚定义-----------------------------------

//---------------------------------------------------------------

#define TRUE        1
#define FALSE       0

#define DELAY_nT    2           // 延时6个机器周期(2+2*DELAY_nT),
                                // (延时时间大于5us,因此机器个数
                                // 根据晶振频率而定)
//***********************************************************************
// 函数名称：I2C_Start()
// 功    能：启动I2C总线
// 入口参数：无
// 出口参数：无
//***********************************************************************
void AT24C02_Init()
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_OD;
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_5|GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_Init(GPIOE,&GPIO_InitStructure);
}

void I2C_Start(void)
{

    I2C_SDA_1;
    I2C_SCL_1;
    Delay_Us(7);
    I2C_SDA_0;
    Delay_Us(7);
    I2C_SCL_0;
    Delay_Us(7);
}
//***********************************************************************
// 函数名称：I2C_Stop()
// 功    能：终止I2C总线
// 入口参数：无
// 出口参数：无
//***********************************************************************
void I2C_Stop(void)
{

    I2C_SDA_0;
    I2C_SCL_1;
    Delay_Us(7);
    I2C_SDA_1;
    Delay_Us(7);
    I2C_SCL_0;
    Delay_Us(7);
}


void I2CSendAck(unsigned char ackbit)
{
    I2C_SCL_0;

    if(ackbit)I2C_SDA_1;
    else I2C_SDA_0;
    Delay_Us(7);
    I2C_SCL_1;
    Delay_Us(7);
    I2C_SCL_0;
    I2C_SDA_1;
    Delay_Us(7);
}
//***********************************************************************
// 函数名称：I2C_Send_Bit_0()
// 功    能：发送比特0
// 入口参数：无
// 出口参数：无
//***********************************************************************
void I2C_Send_Bit_0(void)
{

    I2C_SDA_0;
    I2C_SCL_1;
    Delay_Us(7);
    I2C_SCL_0;
    Delay_Us(7);
}
//***********************************************************************
// 函数名称：I2C_Send_Bit_1()
// 功    能：发送比特1
// 入口参数：无
// 出口参数：无
//***********************************************************************
void I2C_Send_Bit_1(void)
{

    I2C_SDA_1;
    I2C_SCL_1;
    Delay_Us(7);
    I2C_SCL_0;
    Delay_Us(7);
}
//***********************************************************************
// 函数名称：I2C_Check_Ack()
// 功    能：发送完一个字节后检验设备的应答信号
// 入口参数：无
// 出口参数：返回值为True，成功
//           返回值为False，失败
//***********************************************************************
unsigned char I2C_Check_Ack(void)
{
    unsigned char F0;
    I2C_SDA_1;
    I2C_SCL_1;
    Delay_Us(7);
    F0 = I2C_SDA;
    Delay_Us(7);
    I2C_SCL_0;
    Delay_Us(7);
    if( F0 == 1 )  return FALSE;
    else                return TRUE;
}
//***********************************************************************
// 函数名称：I2C_Write8Bit()
// 功    能：向I2C总线写入8bit数据
// 入口参数：I2C_data     将要写入I2C总线的8bit数据
// 出口参数：无
//***********************************************************************
void I2C_Write8Bit(unsigned char I2C_data)
{
    unsigned char i;

    for( i=0;i<8;i++ )
    {
        if( (I2C_data<<i)&0x80 )
            I2C_Send_Bit_1();
        else
            I2C_Send_Bit_0();
    }
}
//***********************************************************************
// 函数名称：I2C_Read8Bit()
// 功    能：从I2C总线接收8bit数据
// 入口参数：无
// 出口参数：返回值为从I2C总线上接收到的8bit数据
//***********************************************************************
unsigned char I2C_Read8Bit(void)
{
    unsigned char F0;
    unsigned char I2C_data = 0,i;

    for( i = 0; i < 8; i++ )
    {
        I2C_data = I2C_data << 1;
        I2C_SDA_1;
        I2C_SCL_1;
        Delay_Us(7);
        F0 = I2C_SDA;
        Delay_Us(7);
        I2C_SCL_0;
        if( F0 == 1 )
            I2C_data = I2C_data | 0x01;
    }
    return I2C_data;
}
//unsigned char num=0;
#define slave   0xa6
void w_24cXX(unsigned char slave_add,unsigned char byte_add,unsigned char byte_add2,unsigned char dat)
{
    I2C_Start();
    I2C_Write8Bit(slave_add);
    I2C_Check_Ack();
    I2C_Write8Bit(byte_add);
    I2C_Check_Ack();
    I2C_Write8Bit(byte_add2);
    I2C_Check_Ack();
    I2C_Write8Bit(dat);
    I2C_Check_Ack();
    I2C_Stop();
}
unsigned char r_24cXX(unsigned char slave_add,unsigned char byte_add,unsigned char byte_add2)
{
    unsigned char temp=0;
    I2C_Start();
    I2C_Write8Bit(slave_add);
    I2C_Check_Ack();
    I2C_Write8Bit(byte_add);
    I2C_Check_Ack();
    I2C_Write8Bit(byte_add2);
    I2C_Check_Ack();
    I2C_Start();
    I2C_Write8Bit(slave_add+0x01);
    I2C_Check_Ack();
    temp=I2C_Read8Bit();
    return temp;
    I2C_Check_Ack();
    I2C_Stop();
}

void EEPROM_Write(u8* string,u8 addr,u8 len)
{
        I2C_Start();
        I2C_Write8Bit(0xa6);
        I2C_Check_Ack();

        I2C_Write8Bit(addr);
        I2C_Check_Ack();


        while(len--)
        {
        I2C_Write8Bit(*string++);
        I2C_Check_Ack();
        Delay_Us(200);
        }

        I2C_Stop();




}

void EEPROM_Read(u8* string,u8 addr,u8 len)
{
    I2C_Start();
    I2C_Write8Bit(0xa6);
    I2C_Check_Ack();
    I2C_Write8Bit(addr);
    I2C_Check_Ack();


    I2C_Start();
    I2C_Write8Bit(0xa6+0x01);
    I2C_Check_Ack();

    while(len--)
    {
     *string++= I2C_Read8Bit();
     if(len)
     {
     I2CSendAck(0);
     }
     else
     {
         I2CSendAck(1);
     }
    }
}




