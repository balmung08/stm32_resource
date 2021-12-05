#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "led.h"
#define PRINT(title, fmt, ...) printf("{"#title":"fmt"}\n", __VA_ARGS__);
#include "delay.h"
#include "usart.h"
#include "mpu6050.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h" 
#include "usmart.h"
#include "dht22.h"
#include "ds18b20.h"
#include "matrix_key.h"
#include "adc.h"
#include "hcsr04.h"
#include "infrared.h"

/*����1����1���ַ� 
//c:Ҫ���͵��ַ�
void usart1_send_char(u8 c)
{   	
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET); //ѭ������,ֱ���������   
	USART_SendData(USART1,c);  
} 
//�������ݸ�����������λ�����
//fun:������. 0XA0~0XAF
//data:���ݻ�����,���28�ֽ�!!
//len:data����Ч���ݸ���
void usart1_niming_report(u8 fun,u8*data,u8 len)
{
	u8 send_buf[32];
	u8 i;
	if(len>28)return;	//���28�ֽ����� 
	send_buf[len+3]=0;	//У��������
	send_buf[0]=0X88;	//֡ͷ
	send_buf[1]=fun;	//������
	send_buf[2]=len;	//���ݳ���
	for(i=0;i<len;i++)send_buf[3+i]=data[i];			//��������
	for(i=0;i<len+3;i++)send_buf[len+3]+=send_buf[i];	//����У���	
	for(i=0;i<len+4;i++)usart1_send_char(send_buf[i]);	//�������ݵ�����1 
}
//���ͼ��ٶȴ��������ݺ�����������
//aacx,aacy,aacz:x,y,z������������ļ��ٶ�ֵ
//gyrox,gyroy,gyroz:x,y,z�������������������ֵ
void mpu6050_send_data(short aacx,short aacy,short aacz,short gyrox,short gyroy,short gyroz)
{
	u8 tbuf[12]; 
	tbuf[0]=(aacx>>8)&0XFF;
	tbuf[1]=aacx&0XFF;
	tbuf[2]=(aacy>>8)&0XFF;
	tbuf[3]=aacy&0XFF;
	tbuf[4]=(aacz>>8)&0XFF;
	tbuf[5]=aacz&0XFF; 
	tbuf[6]=(gyrox>>8)&0XFF;
	tbuf[7]=gyrox&0XFF;
	tbuf[8]=(gyroy>>8)&0XFF;
	tbuf[9]=gyroy&0XFF;
	tbuf[10]=(gyroz>>8)&0XFF;
	tbuf[11]=gyroz&0XFF;
	usart1_niming_report(0XA1,tbuf,12);//�Զ���֡,0XA1
}	
//ͨ������1�ϱ���������̬���ݸ�����
//aacx,aacy,aacz:x,y,z������������ļ��ٶ�ֵ
//gyrox,gyroy,gyroz:x,y,z�������������������ֵ
//roll:�����.��λ0.01�ȡ� -18000 -> 18000 ��Ӧ -180.00  ->  180.00��
//pitch:������.��λ 0.01�ȡ�-9000 - 9000 ��Ӧ -90.00 -> 90.00 ��
//yaw:�����.��λΪ0.1�� 0 -> 3600  ��Ӧ 0 -> 360.0��
void usart1_report_imu(short aacx,short aacy,short aacz,short gyrox,short gyroy,short gyroz,short roll,short pitch,short yaw)
{
	u8 tbuf[28]; 
	u8 i;
	for(i=0;i<28;i++)tbuf[i]=0;//��0
	tbuf[0]=(aacx>>8)&0XFF;
	tbuf[1]=aacx&0XFF;
	tbuf[2]=(aacy>>8)&0XFF;
	tbuf[3]=aacy&0XFF;
	tbuf[4]=(aacz>>8)&0XFF;
	tbuf[5]=aacz&0XFF; 
	tbuf[6]=(gyrox>>8)&0XFF;
	tbuf[7]=gyrox&0XFF;
	tbuf[8]=(gyroy>>8)&0XFF;
	tbuf[9]=gyroy&0XFF;
	tbuf[10]=(gyroz>>8)&0XFF;
	tbuf[11]=gyroz&0XFF;	
	tbuf[18]=(roll>>8)&0XFF;
	tbuf[19]=roll&0XFF;
	tbuf[20]=(pitch>>8)&0XFF;
	tbuf[21]=pitch&0XFF;
	tbuf[22]=(yaw>>8)&0XFF;
	tbuf[23]=yaw&0XFF;
	usart1_niming_report(0XAF,tbuf,28);//�ɿ���ʾ֡,0XAF
}  */
 	
 int main(void)
 {	 
	u8 t=0,report=1;			
	u8 key;
	float pitch,roll,yaw; 		
	short aacx,aacy,aacz;		
	short gyrox,gyroy,gyroz;	
	short temp;		              //mpu6050���¶ȴ�����				
	float temperature = 0; 
	float tempds;	 
  float humidity = 0; 
	float length;
  int adcx;	 
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	 
	uart_init(115200);	 
	delay_init();	 
	usmart_dev.init(72);	
	MPU_Init();		
	Adc_Init();	
	Hcsr04Init();
	Matrix_Key_Init();
	infrared_INIT();	 
	while(DHT22_Init())    
	{
		printf("DHT22_Check Error!\r\n");
	}
	while(mpu_dmp_init())
 	{
		printf("mpu-dmp error!\r\n");
	}  
	while(DS18B20_Init())	//DS18B20???	
	{
		printf("ds18b20 error!\r\n");
	}	
	
	
	
 	while(1)
	{
		mpu_dmp_get_data(&pitch,&roll,&yaw);
		printf("%f,%f,%f",pitch,roll,yaw);
    
		DHT22_Read_Data(&temperature,&humidity);
		printf("%f,%f",temperature,humidity);
		
		tempds=DS18B20_Get_Temp();
		printf("%f",tempds);
		
		adcx=Get_Adc_Average(ADC_Channel_0,10);
		printf("%d",adcx);
		
		length = Hcsr04GetLength();//����ʱ����㳤�ȣ���˲����ϳ������������Ῠ������
		printf("%f",length);
		
		if(INFRARED_STATE())
			printf("yes");
		
		Matrix_Key_Test();
		printf("%d",N);   //NΪȫ�ֱ���������Ҫ�ٶ���
	}	
}
 

