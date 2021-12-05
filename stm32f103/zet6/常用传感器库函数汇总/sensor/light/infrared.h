#define INFRARED_STATE() GPIO_ReadInputDataBit(infrared_PORT,infrared_PIN)

#define infrared_PORT GPIOG
#define infrared_PIN GPIO_Pin_15
#define infrared_RCC RCC_APB2Periph_GPIOG

#define INFRARED_STATE() GPIO_ReadInputDataBit(infrared_PORT,infrared_PIN)