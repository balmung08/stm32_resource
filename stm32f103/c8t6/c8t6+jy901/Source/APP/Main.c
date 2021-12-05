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

char ACCCALSW[5] = {0XFF,0XAA,0X01,0X01,0X00};//������ٶ�У׼ģʽ
char SAVACALSW[5]= {0XFF,0XAA,0X00,0X00,0X00};//���浱ǰ����

//�ô���2��JYģ�鷢��ָ��
void sendcmd(char cmd[])
{
	char i;
	for(i=0;i<5;i++)
		UART2_Put_Char(cmd[i]);
}


//CopeSerialDataΪ����2�жϵ��ú���������ÿ�յ�һ�����ݣ�����һ�����������
void CopeSerial2Data(unsigned char ucData)
{
	static unsigned char ucRxBuffer[250];
	static unsigned char ucRxCnt = 0;	
	
	
	
	ucRxBuffer[ucRxCnt++]=ucData;	//���յ������ݴ��뻺������
	if (ucRxBuffer[0]!=0x55) //����ͷ���ԣ������¿�ʼѰ��0x55����ͷ
	{
		ucRxCnt=0;
		return;
	}
	if (ucRxCnt<11) {return;}//���ݲ���11�����򷵻�
	else
	{
		switch(ucRxBuffer[1])//�ж��������������ݣ�Ȼ���俽������Ӧ�Ľṹ���У���Щ���ݰ���Ҫͨ����λ���򿪶�Ӧ������󣬲��ܽ��յ�������ݰ�������
		{
			case 0x50:	memcpy(&stcTime,&ucRxBuffer[2],8);break;//memcpyΪ�������Դ����ڴ濽��������������"string.h"�������ջ��������ַ����������ݽṹ�����棬�Ӷ�ʵ�����ݵĽ�����
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
		ucRxCnt=0;//��ջ�����
	}
}

void CopeSerial1Data(unsigned char ucData)
{	
	UART2_Put_Char(ucData);//ת������1�յ������ݸ�����2��JYģ�飩
}


int main(void)
{  		
	unsigned char i = 0;
	float roll,pitch,yaw;
	SysTick_init(72,10);//����ʱ��Ƶ��
	Initial_UART1(115200);//��PC�Ĵ���
	Initial_UART2(9600);//��JY-901ģ��Ĵ���	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	  	 //??NVIC????2:2??????,2?????? 	LED_Init();	
	OLED_Init();			//???OLED  
	OLED_Clear(); 
	delay_ms(1000);
	delay_ms(1000);//���JY-91��ʼ�����

	
	while(1)
	{			
		delay_ms(500);
		i++;
		if(i>20)
		{
			i = 0;
			printf("���ڽ��м��ٶ�У׼\r\n");
			sendcmd(ACCCALSW);delay_ms(100);//�ȴ�ģ���ڲ��Զ�У׼�ã�ģ���ڲ����Զ�������Ҫһ����ʱ��
			sendcmd(SAVACALSW);delay_ms(100);//���浱ǰ����
			printf("���ٶ�У׼���\r\n");
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
//���ʱ��
		printf("Time:20%d-%d-%d %d:%d:%.3f\r\n",stcTime.ucYear,stcTime.ucMonth,stcTime.ucDay,stcTime.ucHour,stcTime.ucMinute,(float)stcTime.ucSecond+(float)stcTime.usMiliSecond/1000);
			delay_ms(10);
		//������ٶ�
		//���ڽ��ܵ��������Ѿ���������Ӧ�Ľṹ��ı������ˣ�����˵�����Э�飬�Լ��ٶ�Ϊ�� stcAcc.a[0]/32768*16����X��ļ��ٶȣ�
		printf("Acc:%.3f %.3f %.3f\r\n",(float)stcAcc.a[0]/32768*16,(float)stcAcc.a[1]/32768*16,(float)stcAcc.a[2]/32768*16);
			delay_ms(10);
		//������ٶ�
		printf("Gyro:%.3f %.3f %.3f\r\n",(float)stcGyro.w[0]/32768*2000,(float)stcGyro.w[1]/32768*2000,(float)stcGyro.w[2]/32768*2000);
			delay_ms(10);
		//����Ƕ�
		printf("Angle:%.3f %.3f %.3f\r\n",(float)stcAngle.Angle[0]/32768*180,(float)stcAngle.Angle[1]/32768*180,(float)stcAngle.Angle[2]/32768*180);
			delay_ms(10);
		//����ų�
		printf("Mag:%d %d %d\r\n",stcMag.h[0],stcMag.h[1],stcMag.h[2]);	
			delay_ms(10);
		//�����ѹ���߶�
		printf("Pressure:%ld Height%.2f\r\n",stcPress.lPressure,(float)stcPress.lAltitude/100);
			delay_ms(10);
		//����˿�״̬
		printf("DStatus:%d %d %d %d\r\n",stcDStatus.sDStatus[0],stcDStatus.sDStatus[1],stcDStatus.sDStatus[2],stcDStatus.sDStatus[3]);
			delay_ms(10);
		//�����γ��
		printf("Longitude:%ldDeg%.5fm Lattitude:%ldDeg%.5fm\r\n",stcLonLat.lLon/10000000,(double)(stcLonLat.lLon % 10000000)/1e5,stcLonLat.lLat/10000000,(double)(stcLonLat.lLat % 10000000)/1e5);
			delay_ms(10);
		//�������
		printf("GPSHeight:%.1fm GPSYaw:%.1fDeg GPSV:%.3fkm/h\r\n",(float)stcGPSV.sGPSHeight/10,(float)stcGPSV.sGPSYaw/10,(float)stcGPSV.lGPSVelocity/1000);
			delay_ms(10);
		//�����Ԫ��
		printf("Four elements:%.5f %.5f %.5f %.5f\r\n\r\n",(float)stcQ.q[0]/32768,(float)stcQ.q[1]/32768,(float)stcQ.q[2]/32768,(float)stcQ.q[3]/32768);
		    delay_ms(10);//�ȴ��������
*/
