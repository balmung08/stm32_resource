#ifndef MATRIX_KEY_H
#define MATRIX_KEY_H	 

#include <stm32f10x.h>
#include "usart.h"
#include "sys.h"

#define uint unsigned int 
#define uchar unsigned char

//8个引脚 4个为行 4个为列
//行输出端口定义
#define X1_GPIO_PORT GPIOF          
#define X2_GPIO_PORT GPIOF  
#define X3_GPIO_PORT GPIOF           
#define X4_GPIO_PORT GPIOF 
//列输入端口定义
#define Y1_GPIO_PORT GPIOF         
#define Y2_GPIO_PORT GPIOF   
#define Y3_GPIO_PORT GPIOF           
#define Y4_GPIO_PORT GPIOF 

//行输出引脚定义
#define X1_GPIO_PIN GPIO_Pin_1
#define X2_GPIO_PIN GPIO_Pin_3
#define X3_GPIO_PIN GPIO_Pin_4
#define X4_GPIO_PIN GPIO_Pin_5

//列输入引脚定义
#define Y1_GPIO_PIN GPIO_Pin_12
#define Y2_GPIO_PIN GPIO_Pin_13
#define Y3_GPIO_PIN GPIO_Pin_14
#define Y4_GPIO_PIN GPIO_Pin_15

//行输出时钟定义
#define X1_RCC RCC_APB2Periph_GPIOF
#define X2_RCC RCC_APB2Periph_GPIOF
#define X3_RCC RCC_APB2Periph_GPIOF
#define X4_RCC RCC_APB2Periph_GPIOF

//列输入时钟定义
#define Y1_RCC RCC_APB2Periph_GPIOF
#define Y2_RCC RCC_APB2Periph_GPIOF
#define Y3_RCC RCC_APB2Periph_GPIOF
#define Y4_RCC RCC_APB2Periph_GPIOF
//移植代码只需要修改上面的端口和引脚和时钟即可，下面的代码不用修改。
//矩阵键盘所用的8个引脚可连续可不连续，看实际需要和个人爱好自己定义。

#define KEY_ERR 255


void Matrix_Key_Init(void);
int Matrix_Key_Scan(void);
void Matrix_Key_Test(void) ;
extern int16_t num;
extern int16_t N;
extern int16_t M;
extern int16_t FLAG;
extern int16_t ge;
extern int16_t shi;
extern int16_t bai;
extern int16_t ANGLE;
extern int key[3];
extern int16_t b;
#endif

