#include "debug.h"
#include "uart.h"
u8 string1[] = "AT+RST\r\n";
u8 string2[] = "AT+CWMODE=1\r\n";
u8 string3[] = "AT+CWDHCP=1,1\r\n";
u8 string4[] = "AT+CWJAP=\"gaiz\",\"wcl981206\"\r\n";
u8 string5[] = "AT+MQTTUSERCFG=0,1,\"ch32\",\"qZOR7qPPJH\",\"version=2018-10-31&res=products%2FqZOR7qPPJH%2Fdevices%2Fch32&et=1763716840&method=md5&sign=hcqvkPb5ZA1eeorw6u1Qow%3D%3D\",0,0,\"\"\r\n";
u8 string6[] = "AT+MQTTCONN=0,\"mqtts.heclouds.com\",1883,1\r\n";
u8 string7[] = "AT+MQTTSUB=0,\"$sys/qZOR7qPPJH/ch32/thing/property/set\",1\r\n";
u8 string8[]="AT+MQTTPUB=0,\"$sys/qZOR7qPPJH/ch32/thing/property/post\",\"{\\\"id\\\":\\\"123\\\"\\,\\\"params\\\":{\\\"appkey\\\":{\\\"value\\\":123}}}\",0,0\r\n";
void esp8266_init()
{
    Uart6_Init();
}

void onenet_init()
{
    uart6_send_string(string1, sizeof(string1)-1);
    Delay_Ms(1000);
    uart6_send_string(string2, sizeof(string2)-1);
    Delay_Ms(1000);
    uart6_send_string(string3, sizeof(string3)-1);
    Delay_Ms(1000);
    uart6_send_string(string4, sizeof(string4)-1);
    Delay_Ms(1000);
    uart6_send_string(string5, sizeof(string5)-1);//为了防止处于busy状态可以适当增加延时时间
    Delay_Ms(1000);
    Delay_Ms(1000);
    uart6_send_string(string6, sizeof(string6)-1);
    Delay_Ms(1000);
    Delay_Ms(1000);
    uart6_send_string(string7, sizeof(string7)-1);//下载后使用API调试测试是否成功订阅主题
    Delay_Ms(1000);
    Delay_Ms(1000);
    uart6_send_string(string8, sizeof(string8)-1);//下载后使用API调试测试是否成功订阅主题
    Delay_Ms(1000);
    Delay_Ms(1000);

}
