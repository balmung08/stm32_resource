#include "stm32f10x.h"
#include <string.h>
#include <stdio.h>
#include "Main.h"
#include "UART1.h"
#include "UART2.h"
#include "delay.h"
#include "JY901.h"
#include "sys.h"
#include "oled.h"
#include "bmp.h"

struct STime		stcTime;
struct SAcc 		stcAcc;
struct SGyro 		stcGyro;
struct SAngle 	stcAngle;
struct SMag 		stcMag;
struct SDStatus stcDStatus;
struct SPress 	stcPress;
struct SLonLat 	stcLonLat;
struct SGPSV 		stcGPSV;
struct SQ       stcQ;

char ACCCALSW[5] = {0XFF,0XAA,0X01,0X01,0X00};//进入加速度校准模式
char SAVACALSW[5]= {0XFF,0XAA,0X00,0X00,0X00};//保存当前配置

//用串口2给JY模块发送指令
void sendcmd(char cmd[])
{
	char i;
	for(i=0;i<5;i++)
		UART2_Put_Char(cmd[i]);
}


//CopeSerialData为串口2中断调用函数，串口每收到一个数据，调用一次这个函数。
void CopeSerial2Data(unsigned char ucData)
{
	static unsigned char ucRxBuffer[250];
	static unsigned char ucRxCnt = 0;	
	
	
	
	ucRxBuffer[ucRxCnt++]=ucData;	//将收到的数据存入缓冲区中
	if (ucRxBuffer[0]!=0x55) //数据头不对，则重新开始寻找0x55数据头
	{
		ucRxCnt=0;
		return;
	}
	if (ucRxCnt<11) {return;}//数据不满11个，则返回
	else
	{
		switch(ucRxBuffer[1])//判断数据是哪种数据，然后将其拷贝到对应的结构体中，有些数据包需要通过上位机打开对应的输出后，才能接收到这个数据包的数据
		{
			case 0x50:	memcpy(&stcTime,&ucRxBuffer[2],8);break;//memcpy为编译器自带的内存拷贝函数，需引用"string.h"，将接收缓冲区的字符拷贝到数据结构体里面，从而实现数据的解析。
			case 0x51:	memcpy(&stcAcc,&ucRxBuffer[2],8);break;
			case 0x52:	memcpy(&stcGyro,&ucRxBuffer[2],8);break;
			case 0x53:	memcpy(&stcAngle,&ucRxBuffer[2],8);break;
			case 0x54:	memcpy(&stcMag,&ucRxBuffer[2],8);break;
			case 0x55:	memcpy(&stcDStatus,&ucRxBuffer[2],8);break;
			case 0x56:	memcpy(&stcPress,&ucRxBuffer[2],8);break;
			case 0x57:	memcpy(&stcLonLat,&ucRxBuffer[2],8);break;
			case 0x58:	memcpy(&stcGPSV,&ucRxBuffer[2],8);break;
			case 0x59:	memcpy(&stcQ,&ucRxBuffer[2],8);break;
		}
		ucRxCnt=0;//清空缓存区
	}
}

void CopeSerial1Data(unsigned char ucData)
{	
	UART2_Put_Char(ucData);//转发串口1收到的数据给串口2（JY模块）
}


int main(void)
{  		
	unsigned char i = 0;
	float roll,pitch,yaw;
	SysTick_init(72,10);//设置时钟频率
	Initial_UART1(115200);//接PC的串口
	Initial_UART2(9600);//接JY-901模块的串口	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	  	 //??NVIC????2:2??????,2?????? 	LED_Init();	
	OLED_Init();			//???OLED  
	OLED_Clear(); 
	delay_ms(1000);
	delay_ms(1000);//等菾Y-91初始化完成

	
	while(1)
	{			
		delay_ms(500);
		i++;
		if(i>20)
		{
			i = 0;
			printf("正在进行加速度校准\r\n");
			sendcmd(ACCCALSW);delay_ms(100);//等待模块内部自动校准好，模块内部会自动计算需要一定的时间
			sendcmd(SAVACALSW);delay_ms(100);//保存当前配置
			printf("加速度校准完成\r\n");
		}
		roll =  (float)stcAngle.Angle[0]/32768*180;
		pitch = (float)stcAngle.Angle[1]/32768*180;
		yaw = (float)stcAngle.Angle[2]/32768*180;

		printf("%f      ",roll);
		printf("Angle:%.3f %.3f %.3f\r\n",(float)stcAngle.Angle[0]/32768*180,(float)stcAngle.Angle[1]/32768*180,(float)stcAngle.Angle[2]/32768*180);
		OLED_Clear(); 
		OLED_ShowString(0,0,"pitch:",16);
		OLED_ShowNum(60,0,abs((int)pitch),4,16);
		OLED_ShowString(0,2,"roll:",16);
		OLED_ShowNum(60,2,abs((int)roll),4,16);
		OLED_ShowString(0,4,"yaw:",16);
		OLED_ShowNum(60,4,abs((int)yaw),4,16);	
    if((int)pitch<0)
					OLED_ShowChar(50,0,'-',16);
		if((int)roll<0)
					OLED_ShowChar(50,2,'-',16);			
		if((int)yaw<0)
					OLED_ShowChar(50,4,'-',16);				
		delay_ms(10);
		
	}
}











/*
//输出时间
		printf("Time:20%d-%d-%d %d:%d:%.3f\r\n",stcTime.ucYear,stcTime.ucMonth,stcTime.ucDay,stcTime.ucHour,stcTime.ucMinute,(float)stcTime.ucSecond+(float)stcTime.usMiliSecond/1000);
			delay_ms(10);
		//输出加速度
		//串口接受到的数据已经拷贝到对应的结构体的变量中了，根据说明书的协议，以加速度为例 stcAcc.a[0]/32768*16就是X轴的加速度，
		printf("Acc:%.3f %.3f %.3f\r\n",(float)stcAcc.a[0]/32768*16,(float)stcAcc.a[1]/32768*16,(float)stcAcc.a[2]/32768*16);
			delay_ms(10);
		//输出角速度
		printf("Gyro:%.3f %.3f %.3f\r\n",(float)stcGyro.w[0]/32768*2000,(float)stcGyro.w[1]/32768*2000,(float)stcGyro.w[2]/32768*2000);
			delay_ms(10);
		//输出角度
		printf("Angle:%.3f %.3f %.3f\r\n",(float)stcAngle.Angle[0]/32768*180,(float)stcAngle.Angle[1]/32768*180,(float)stcAngle.Angle[2]/32768*180);
			delay_ms(10);
		//输出磁场
		printf("Mag:%d %d %d\r\n",stcMag.h[0],stcMag.h[1],stcMag.h[2]);	
			delay_ms(10);
		//输出气压、高度
		printf("Pressure:%ld Height%.2f\r\n",stcPress.lPressure,(float)stcPress.lAltitude/100);
			delay_ms(10);
		//输出端口状态
		printf("DStatus:%d %d %d %d\r\n",stcDStatus.sDStatus[0],stcDStatus.sDStatus[1],stcDStatus.sDStatus[2],stcDStatus.sDStatus[3]);
			delay_ms(10);
		//输出经纬度
		printf("Longitude:%ldDeg%.5fm Lattitude:%ldDeg%.5fm\r\n",stcLonLat.lLon/10000000,(double)(stcLonLat.lLon % 10000000)/1e5,stcLonLat.lLat/10000000,(double)(stcLonLat.lLat % 10000000)/1e5);
			delay_ms(10);
		//输出地速
		printf("GPSHeight:%.1fm GPSYaw:%.1fDeg GPSV:%.3fkm/h\r\n",(float)stcGPSV.sGPSHeight/10,(float)stcGPSV.sGPSYaw/10,(float)stcGPSV.lGPSVelocity/1000);
			delay_ms(10);
		//输出四元素
		printf("Four elements:%.5f %.5f %.5f %.5f\r\n\r\n",(float)stcQ.q[0]/32768,(float)stcQ.q[1]/32768,(float)stcQ.q[2]/32768,(float)stcQ.q[3]/32768);
		    delay_ms(10);//等待传输完成
*/

