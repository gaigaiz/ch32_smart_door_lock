
#include "debug.h"//头文件
#include "lcd.h"//屏幕头文件
#include "timer.h"//定时器头文件
#include "usart.h"//串口头文件
#include "uart.h"
#include "key.h"//矩阵按键头文件
#include "pic.h"//照片头文件
#include "audio.h"//语音模块头文件
#include "string.h"//数组函数相关头文件
#include "as608.h"
#include "esp8266.h"
#include "iic.h"
/*函数声明区*/
void USART3_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));//串口3快速中断
void USART2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));//串口2快速中断
void UART7_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));//串口7快速中断
void UART6_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));//串口8快速中断
void TIM3_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));//定时器3快速中断-需要添加这个快速中断的一行，不然有可能进不去这个中断
u8 string_chek(u8* string1,u8* string2,u8 len);//数组检测函数-比对两个数组内的数据是否相同(此处用来检验密码锁的输入确认功能)
void key_clear();
void string_copy(u8* string1,u8* string2,u8 len);//这个函数用于修改开锁密码(功能:复制数组);
u8 rfid_chek();//检验卡片是否为已经添加的卡
void Ps_Wait();//等待协议数据包接收并让应答协议数据包打印
void esp8266_proc();
/*变量声明区*/
unsigned long int uwTick;//总定时器
unsigned char Key_Val, Key_Old, Key_Down, Key_Up;

u8 num;
u8 password[6]={1,2,3,4,5,6};//密码
u8 password_cmd[6]={2,7,7,5,1,6};//管理员密码
u8 key_temp[7]={10,10,10,10,10,10};//按键暂存区-如果输入的数字超过第六位锁死在第7位
u8 key_index;//按键的索引
u8 key_index_old;//防止重复填充黄色背景造成屏幕反复刷新
u16 time5000ms;//5s计时变量，用来自动锁门
u8 lock_flag;//控制是否锁住的标志位
u8 password_error;//输入错误密码次数
u16 time15s=15;//密码错误锁定倒计时15s[使用倒计时的方案]
u16 time1000ms;//密码错误倒计时1s
u8 show_flag;//汉字显示标志位
u8 show_flag_old;//汉字显示标志位
u8 mode;//0-主页(开锁/关锁)   1-无管理员权限并修改密码   2-有管理员权限并修改密码   3-录入卡片，没有解锁管理员权限    4-录入卡片，解锁管理员权限      5-录入指纹，没有解锁管理员权限    6-录入指纹，解锁管理员权限
u8 rfid_index;//刷卡模块接收索引
u8 rfid_temp[4]={0};//卡号暂存区
u8 rfid[4][4]={0};//卡片存储
u8 rfid_password_index;//卡片存储索引
u8 uart7_rec_string[20]={0};//串口7接收数组
u8 uart_rec_index;//串口7指引
u8 uart_rec_tick;//由于这个数据包是不定长的所以需要这个值
u8 ps_wait_flag;//用于等待指纹数据接收完毕
u8 as608_proc_flag;//获取手指是否按在模块上
u8 as608_proc_flag_old;//检验上个标志位是否发生改变
u8 as608_store_index=1;//指纹存储索引
/*使用超时解析UART6*/
u8 uart6_rec_string[256]={0};//
u8 uart6_rec_tick;
u8 uart6_rec_index;
/*执行函数声明区*/
//LCD
void Lcd_Proc()
{

    /*黄色密码输入区*/
    if(key_index!=key_index_old)
    {
        LCD_Fill(0, 45, 128, 66, YELLOW);//密码输入的背景颜色(每次进入后都会重新填充)这个需要放入判断内不然会出现bug
        u8 i=key_index;
        if(key_index==7)key_index=6;//锁死在第六位防止卡死
        while(i--)
        {
            if(i<6)//当数据输出超过时不进行后续的显示
            LCD_ShowChar(16 * i, 45, '*', RED, YELLOW, 16, 0);//进行加密显示
        }
        key_index_old=key_index;//记录上一次状态
    }
    /*屏幕顶部汉字显示区*/
    if(show_flag!=show_flag_old)
    {
        LCD_Fill(0, 0, 128, 32, WHITE);
        show_flag_old= show_flag;//注意刷新完屏幕后将对应数据传输给另一个标志位方便屏幕刷新(如果没加此句会导致屏幕频繁刷新)
    }

    switch(show_flag)
    {
        case 0:
        LCD_ShowChinese(0, 0, "门锁状态上锁", RED, WHITE, 16, 0);
        break;
        case 1:
        LCD_ShowChinese(0, 0, "门锁状态开锁", RED, WHITE, 16, 0);
        break;
        case 2://门锁倒计时
        LCD_ShowIntNum(0, 0, time15s, 2, RED, WHITE, 16);
        break;
    }
}

//key
void Key_Proc()
{
    Key_Val = key_read();
    Key_Down = Key_Val & (Key_Val ^ Key_Old);
    Key_Up = ~Key_Val & (Key_Val ^ Key_Old);
    Key_Old = Key_Val;
    /*输入按键的对应数字以及其他按键对应的操作*/
    if(password_error==3)return;//锁住键盘输入
    if(Key_Down)
    {
        audio_play(1);//按键音效

    }
    switch(Key_Down)
    {
        case 1:
        key_temp[key_index]=1;
        key_index++;
        break;
        case 2:
        key_temp[key_index]=2;
        key_index++;
        break;
        case 3:
        key_temp[key_index]=3;
        key_index++;
        break;
        case 4://密码修改界面
        if(mode==0)
        {
            mode=1;
            audio_play(6);
            password_error=0;//将错误次数归0
            key_clear();
        }
        break;
        case 5:
        key_temp[key_index]=4;
        key_index++;
        break;
        case 6:
        key_temp[key_index]=5;
        key_index++;
        break;
        case 7:
        key_temp[key_index]=6;
        key_index++;
        break;
        case 8:
        if(mode==0)
        {
           mode=5;//录入指纹
           audio_play(18);//语音-录入指纹，请输入管理员密码
           password_error=0;//将错误次数归0
           key_clear();
        }
        break;
        case 9:
        key_temp[key_index]=7;
        key_index++;
        break;
        case 10:
        key_temp[key_index]=8;
        key_index++;
        break;
        case 11:
        key_temp[key_index]=9;
        key_index++;
        break;
        case 12://卡片录入
        if(mode==0)
        {
            mode=3;
            audio_play(13);//语音-录入卡片，请输入管理员密码
            key_clear();
        }
        break;
        case 13:
        key_index=0;//清空
        if(mode==6)
        {
            PS_Empty();//清空所有已录入指纹
            mode=0;
        }
        break;
        case 14:
        key_temp[key_index]=0;
        key_index++;
        break;
        case 15://按键回退(注意需要对这个索引值进行限幅防止执行--后溢出)
        if(key_index)
        {
            key_index--;
            key_temp[key_index]=10;
        }
        break;
        case 16:
        switch(mode)
        {
            case 0://主页
                if(string_chek(key_temp, password, 6))
                {
                     lock_flag =1;//开锁
                     show_flag =1;//开锁状态
                     audio_play(3);//语音欢迎回家
                     key_clear();
                     password_error=0;//将错误次数归0
                }
                else
                {
                    audio_play(4);//语音:密码错误
                    key_clear();//键盘清空
                    if(++password_error==3)
                    {
                        show_flag=2;//键盘锁定
                        audio_play(5);//语音键盘锁定
                    }
                }
            break;
            case 1://无管理员权限并解锁管理员
            if(string_chek(key_temp, password_cmd, 6))
            {
                mode=2;//修改密码解锁管理员权限
                audio_play(7);//语音
                key_clear();
            }
            else
            {
                audio_play(9);
                if(++password_error==3)//如果密码错误3次(锁键盘)
                {
                    audio_play(10);
                    show_flag=2;
                }
            }
            break;
            case 2://解锁管理员后修改密码
            string_copy(key_temp, password, 6);//将当前输入的密码变为新的开锁密码
            audio_play(8);//语音播报修改新的开锁密码
            mode=0;//返回原界面
            key_clear();//注意清空后当前输入的数据
            break;
            case 3://解锁管理员权限并将手中卡片进行录入
            if(string_chek(key_temp, password_cmd, 6))
            {
                mode=4;//录入卡片，并使用密码解锁管理员权限
                audio_play(14);//语音-管理员密码验证成功，请将卡片平放在传感器上
                key_clear();
            }
            else
            {
                audio_play(9);
                if(++password_error==3)//如果密码错误3次(锁键盘)
                {
                    audio_play(10);
                    show_flag=2;
                }
            }
            break;
            case 5://解锁管理员权限并将指纹进行录入
            if(string_chek(key_temp, password_cmd, 6))
            {
                mode=6;//录入卡片，并使用密码解锁管理员权限
                audio_play(19);//语音-管理员密码验证成功，请将手指平放在传感器上
                key_clear();
            }
            else
            {
                audio_play(9);
                if(++password_error==3)//如果密码错误3次(锁键盘)
                {
                    audio_play(10);
                    show_flag=2;
                }
            }
            break;
        }
        break;

    }

}
/*检查数组是否相同(检查密码是否相同)*/
u8 string_chek(u8* string1,u8* string2,u8 len)
{
    while(len--)
    {
        if(string1[len]==string2[len]);
        else return 0;//不同则返回假
    }
    return 1;//如果二者数组相同返回真
}
void lock_proc()//门锁处理函数
{
    lock(lock_flag);
}
void as608_proc()
{
    as608_proc_flag=GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1);//通过读取PA1引脚的高低电平来知晓是否有手指按在指纹模块上，高-有，低-无
    if(as608_proc_flag==as608_proc_flag_old)return;
    as608_proc_flag_old=as608_proc_flag;
    if(as608_proc_flag==0)return;//抬起指纹时不进行后续的操作

    if(mode==6)//检验是否处于模式6状态
    {
        PS_GetImage();//指纹传感器获取图像到暂存区
        Ps_Wait();//等待
        PS_GenCha(1);//将暂存区数据放入buffer1
        Ps_Wait();//等待
        PS_GenCha(2);//将暂存区数据放入buffer2
        Ps_Wait();//等待
        PS_RegModel();//合并buffer1和buffer2
        Ps_Wait();//等待
        PS_StoreChar(as608_store_index);//将数据进行储存
        as608_store_index++;
        Ps_Wait();//等待
        audio_play(21);//语音-指纹录入成功
        mode=0;//返回主页模式
    }
    if(mode==0)//处于主页模式时
    {
        PS_GetImage();//指纹传感器获取图像到暂存区
        Ps_Wait();//等待
        PS_GenCha(2);//将暂存区数据放入buffer1
        Ps_Wait();//等待
        PS_Search();
        Ps_Wait();//等待
        if(uart7_rec_string[13]>50)//这个是指纹模块返回的分数，如果该分数大于50代表查找成功并指纹皮胚
        {
            audio_play(16);
            lock_flag =1;//开锁
            show_flag =1;//文字显示开锁状态
            audio_play(3);//语音欢迎回家
        }
        else
        {
            audio_play(17);
        }
    }
}
void Ps_Wait()
{
    ps_wait_flag=1;//注意在串口7中断接收时需要置0
    do
    {
        Delay_Ms(200);//由于接收到的数据包长度很长如果想要将所有应答的协议数据包打印出来需要足量的时间进行打印
    }
    while(ps_wait_flag);//当处于接收指纹数据时一直进行等待直到接收完毕则结束等待
}
void key_clear()//键盘暂存区清空函数
{
    memset(key_temp,10,6);//将当前存储的数组进行清除防止开锁后输入密码进行保留
    key_index=0;
}
void string_copy(u8* string1,u8* string2,u8 len)//这个函数用于修改开锁密码(功能:复制数组)
{
    u8 i;
    for(i=0;i<len;i++)
    {
        string2[i]=string1[i];
    }
}
u8 rfid_chek()//RC卡检测-用于检验RC卡开门
{
    u8 i;
    for(i=0;i<rfid_password_index;i++)
    {
        if(string_chek(rfid_temp, rfid[i], 4)) return 1;
    }
    return 0;
}
void esp8266_proc()
{
    if(uart6_rec_index==0)return;//未接收到数据
    if(uart6_rec_tick>10)//超时解析
    {
        usart1_send_string(uart6_rec_string, uart6_rec_index);
        char* add1 = strstr(uart6_rec_string,"appkey");//从这个uart6_rec_string完整数组中寻找到appkey开头的数组，并将这个a的首地址赋值给add1的地址
        int appkey=0;
        sscanf(add1,"appkey\":%d",&appkey);
        char* add2 = strstr(uart6_rec_string,"appstring");
        int appstring =0;
        sscanf(add2,"appstring\":\"%d",&appstring);//注意双引号要进行转义
//        LCD_ShowIntNum(0, 80, appkey, 3, BLUE, GREEN, 16);
//        LCD_ShowIntNum(0, 100, appstring, 6, BLUE, GREEN, 16);
        u8 temp2_string[]="AT+MQTTPUB=0,\"$sys/qZOR7qPPJH/ch32/thing/property/post\",\"{\\\"id\\\":\\\"123\\\"\\,\\\"params\\\":{\\\"appkey\\\":{\\\"value\\\":0}}}\",0,0\r\n";//这是清空数组，修改密码后将真实value值进行清空
        u8 temp3_string[]="AT+MQTTPUB=0,\"$sys/qZOR7qPPJH/ch32/thing/property/post\",\"{\\\"id\\\":\\\"123\\\"\\,\\\"params\\\":{\\\"appkey\\\":{\\\"value\\\":8}}}\",0,0\r\n";
        switch(appkey)
          {
              case 1:
              lock_flag=1;//开锁
              break;
              case 2:
              lock_flag=0;//关锁
              break;
              case 3://将开锁密码进行重置
              password[0]=appstring/100000%10;
              password[1]=appstring/10000%10;
              password[2]=appstring/1000%10;
              password[3]=appstring/100%10;
              password[4]=appstring/10%10;
              password[5]=appstring%10;
              EEPROM_Write(password, 8, 6);//将密码写入EEPROM中
              u8 temp_string[]="AT+MQTTPUB=0,\"$sys/qZOR7qPPJH/ch32/thing/property/post\",\"{\\\"id\\\":\\\"123\\\"\\,\\\"params\\\":{\\\"appkey\\\":{\\\"value\\\":6}}}\",0,0\r\n";
              uart6_send_string(temp_string, sizeof(temp_string)-1);//修改好密码后会有弹窗提示(弹窗提示的appkey==6),即将新的appkey值发送给手机
              break;
              case 4://控制音量
              audio_yinliang(appstring);
              uart6_send_string(temp3_string, sizeof(temp3_string)-1);//修改好密码后会有弹窗提示(弹窗提示的appkey==6),即将新的appkey值发送给手机
              break;
              case 7:
              uart6_send_string(temp2_string, sizeof(temp2_string)-1);//清空真实value值
              break;
          }
        uart6_rec_index=0;
    }
}
//中断服务函数(系统计时器3)
void TIM3_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM3, TIM_IT_Update)!=RESET)
    {
        uwTick++;
        uart_rec_tick++;
        uart6_rec_tick++;
        if(lock_flag==1)//当处于开锁状态时
        {
           if(++time5000ms==5000)//当门锁已经打开5s时进行自动锁门
           {
               time5000ms=0;
               lock_flag=0;
               audio_play(2);
               show_flag=0;//自动关锁汉字显示
           }
        }
        if(password_error==3)//如果密码错误3次
        {
            if(++time1000ms==1000)//1s计时
            {
                time1000ms=0;
                if(--time15s==0)//15s倒计时
                 {
                    time15s=15;
                    show_flag=0;//键盘解锁完毕后返回原界面
                    password_error=0;//密码错误次数归0
                 }
            }
        }
    }
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
}
//串口3中断(发送和接收中断)
void USART3_IRQHandler(void)
{
    u8 temp;
    if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)//获取当前串口中断状态
    {
        temp=USART_ReceiveData(USART3);
    }
    USART_ClearITPendingBit(USART3, USART_IT_RXNE);
}
//串口3中断(发送和接收中断)
void USART2_IRQHandler(void)
{
    u8 temp;
    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)//获取当前串口中断状态
    {
        temp=USART_ReceiveData(USART2);
        switch(rfid_index)
        {
            case 0:
            if(temp==0x04)rfid_index++;
            else rfid_index=0;//如果检测到不是对应的数据则直接索引复位为0
            break;
            case 1:
            if(temp==0x0c)rfid_index++;
            else rfid_index=0;//如果检测到不是对应的数据则直接索引复位为0
            break;
            case 2:
            if(temp==0x02)rfid_index++;
            else rfid_index=0;//如果检测到不是对应的数据则直接索引复位为0
            break;
            case 3:
            if(temp==0x30)rfid_index++;
            else rfid_index=0;//如果检测到不是对应的数据则直接索引复位为0
            break;
            case 4:
            if(temp==0x00)rfid_index++;
            else rfid_index=0;//如果检测到不是对应的数据则直接索引复位为0
            break;
            case 5:
            if(temp==0x04)rfid_index++;
            else rfid_index=0;//如果检测到不是对应的数据则直接索引复位为0
            break;
            case 6:
            if(temp==0x00)rfid_index++;
            else rfid_index=0;//如果检测到不是对应的数据则直接索引复位为0
            break;
            case 7:
            rfid_temp[0]=temp;
            rfid_index++;
            break;
            case 8:
            rfid_temp[1]=temp;
            rfid_index++;
            break;
            case 9:
            rfid_temp[2]=temp;
            rfid_index++;
            break;
            case 10:
            rfid_temp[3]=temp;
            rfid_index=0;
            if(mode==0)//处于主页时进行刷卡
            {
                if(rfid_chek())
                {
                    audio_play(11);//语音-管理员密码验证成功，请将卡片平放在传感器上
                    lock_flag=1;
                }
                else {
                    audio_play(12);//语音-刷卡失败，请重试
                }
            }
            if(mode==4)//现已经获得管理员权限，接下来录入卡片
            {
                string_copy(rfid_temp, rfid[rfid_password_index], 4);//将卡片中后四位数据进行录入
                audio_play(15);//语音-卡片添加成功
                mode=0;
                rfid_password_index++;//目前已添加1张卡片
            }
            break;
        }
    }
    USART_ClearITPendingBit(USART2, USART_IT_RXNE);
}

/*串口屏接收函数*/
void UART7_IRQHandler(void)
{
    u8 temp=0;
    if(USART_GetITStatus(UART7, USART_IT_RXNE) != RESET)
       {
        ps_wait_flag=0;//处于接收状态时归0
        if(uart_rec_tick>10)uart_rec_index=0;//如果超过10ms没有接收到新数据代表数据包已经发送完毕，将对应的索引清0
        temp=USART_ReceiveData(UART7);//从串口7接收数据
        uart7_rec_string[uart_rec_index]=temp;//将数据存储在数组中
        uart_rec_index++;//索引自加
        uart_rec_tick=0;//将计时值归0-用于超时解析
       }
    USART_ClearITPendingBit(UART7, USART_IT_RXNE);
}
void UART6_IRQHandler(void)
{
    u8 temp=0;
    if(USART_GetITStatus(UART6, USART_IT_RXNE) != RESET)
    {
       uart6_rec_tick=0;//进入串口6中断并成功接收到数据时将该值归0
       temp=USART_ReceiveData(UART6);//从串口7接收数据
       uart6_rec_string[uart6_rec_index]=temp;
       uart6_rec_index++;
    }
    USART_ClearITPendingBit(UART6, USART_IT_RXNE);
}
/* 任务调度器 */
typedef struct
{
    void (*task_func)(void);   // 任务函数
    unsigned long int rate_ms; // 任务周期
    unsigned long int last_ms; // 任务最后运行的时间
} task_t;

task_t Scheduler_Task[] =
    {
            {Lcd_Proc,100,0},//屏幕
            {Key_Proc,10,0},//按键
            {lock_proc,30,0},//舵机
            {as608_proc,20,0},//指纹模块
            {esp8266_proc,2,0}
    };

unsigned char task_num;

void Scheduler_Init()
{
    task_num = sizeof(Scheduler_Task) / sizeof(task_t);
}

void Scheduler_Run()
{
    unsigned char i;
    for (i = 0; i < task_num; i++)
    {
        unsigned long int now_time = uwTick;
        if (now_time >= Scheduler_Task[i].rate_ms + Scheduler_Task[i].last_ms)
        {
            Scheduler_Task[i].last_ms = now_time;
            Scheduler_Task[i].task_func();
        }
    }
}
/*主函数*/
int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//中断优先级分组
	SystemCoreClockUpdate();//时钟配置
	USART_Printf_Init(115200);//串口1初始化(一般用于检验某一个模块是否起作用)
	Delay_Init();//延时函数初始化
	TIM2_PWM_Init();//定时器2的pwm初始化(舵机初始化)
	Usart3_Init();
	Usart2_Init();
	as608_init();
    esp8266_init();
    Delay_Ms(10);
    onenet_init();
	LCD_Init();//LCD初始化

//	/*进度条*/
	LCD_Fill(0, 0, 127, 127, WHITE);
	LCD_ShowPicture(0, 0, 128, 128, gImage_1);
	unsigned char i=0;
	while(i<128)
	{
	        LCD_DrawLine(i, 0, i, 10, RED);//绘制一条红色的随时间增长的红线(可以制作为进度条)
	        i++;
	        Delay_Ms(20);
	}
	/*进度条走完后，首页显示*/


	LCD_ShowPicture(0, 0, 128, 128, gImage_2);
	LCD_Show_Chinese(0, 0, "门锁状态:上锁", RED, WHITE, 16, 0);
	LCD_Show_Chinese(0,30,"输入密码",RED,WHITE,16,0);
	LCD_Fill(0, 45, 128, 66, YELLOW);//密码输入的背景颜色(每次进入后都会重新填充)


	key_init();
	Tim3_Init(1000, 96-1);//系统计时器初始化
	Scheduler_Init();
	audio_init();
	audio_yinliang(5);
	Delay_Ms(10);//在调节音量时需要使用延时函数，不然会让后续的语音播放这个代码跳过
	audio_play(2);

//	AT24C02_Init();//首先对EEPROM模块进行初始化
//	u8 eep_string1[]={1,2,3};
//	u8 eep_string2[]={0,0,0};
//	EEPROM_Write(eep_string1, 1, 3);
//	Delay_Ms(10);
//	EEPROM_Read(eep_string2, 1, 3);
//	LCD_ShowIntNum(0, 0, eep_string2[0], 3, BLUE, GREEN, 16);
//	LCD_ShowIntNum(0, 0, eep_string2[1], 3, BLUE, GREEN, 16);
//	LCD_ShowIntNum(0, 0, eep_string2[2], 3, BLUE, GREEN, 16);
	while(1)
    {

	    Scheduler_Run();

	}
}


