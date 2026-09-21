/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "stm32f4xx.h"
#include "stm32f4xx_hal_can.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "tim.h"
#include "can.h"
#include "usart.h"
#include <stdint.h>
void LED_Breath(uint32_t channel);
void BUZZER_Beep(uint32_t f, uint32_t T);
void BUZZER_Music1(void);
void BUZZER_Music2(void);
void SERVO_Angle(uint32_t angle);
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
/* USER CODE BEGIN Variables */
volatile uint16_t motor_A = 0;   
volatile int16_t  motor_V = 0;   
volatile int16_t  motor_I = 0;   
volatile uint8_t  motor_t = 0;
volatile int32_t  motor_count  = 0;
volatile float    output_angle = 0.0f; 
/* USER CODE END Variables */

/* USER CODE END Variables */
/* Definitions for ledTask */
osThreadId_t ledTaskHandle;
const osThreadAttr_t ledTask_attributes = {
  .name = "ledTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for buzzerTask */
osThreadId_t buzzerTaskHandle;
const osThreadAttr_t buzzerTask_attributes = {
  .name = "buzzerTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for servoTask */
osThreadId_t servoTaskHandle;
const osThreadAttr_t servoTask_attributes = {
  .name = "servoTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for motorTask */
osThreadId_t motorTaskHandle;
const osThreadAttr_t motorTask_attributes = {
  .name = "motorTask",
  .stack_size = 1024 * 4,
  .priority = (osPriority_t) osPriorityBelowNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartledTask(void *argument);
void StartbuzzerTask(void *argument);
void StartservoTask(void *argument);
void StartmotorTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of ledTask */
  ledTaskHandle = osThreadNew(StartledTask, NULL, &ledTask_attributes);

  /* creation of buzzerTask */
  buzzerTaskHandle = osThreadNew(StartbuzzerTask, NULL, &buzzerTask_attributes);

  /* creation of servoTask */
  servoTaskHandle = osThreadNew(StartservoTask, NULL, &servoTask_attributes);

  /* creation of motorTask */
  motorTaskHandle = osThreadNew(StartmotorTask, NULL, &motorTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartledTask */
/**
  * @brief  Function implementing the ledTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartledTask */
void StartledTask(void *argument)
{
  /* USER CODE BEGIN StartledTask */
  /* Infinite loop */    
  
  HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_3);//RED LED PWM START
  HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_2);//GREEN LED PWM START
  HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_1);//BLUE LED PWM START
  for(;;)
  {
    LED_Breath(TIM_CHANNEL_1);//B
    LED_Breath(TIM_CHANNEL_2);//G
    LED_Breath(TIM_CHANNEL_3);//R
  }
  /* USER CODE END StartledTask */
}

/* USER CODE BEGIN Header_StartbuzzerTask */
/**
* @brief Function implementing the buzzerTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartbuzzerTask */
void StartbuzzerTask(void *argument)
{
  /* USER CODE BEGIN StartbuzzerTask */
  /* Infinite loop */
  uint8_t state = 0;
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_3);//BUZZER
  BUZZER_Beep(4000, 200);
  for(;;)
  {
    if(HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin) == GPIO_PIN_RESET)
    {
      osDelay(20);
      if(HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin) == GPIO_PIN_RESET)
      {
        while (HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin) == GPIO_PIN_RESET) 
        {
          osDelay(10);
        }
        osDelay(20);
      }
      state++;
      if(state == 1)
      {
        BUZZER_Music1();
      }
      if(state == 2)
      {
        BUZZER_Music2();
      }
      if(state > 2)
      {
        state = 0;
      }
    }
    osDelay(10);
  }
  /* USER CODE END StartbuzzerTask */
}

/* USER CODE BEGIN Header_StartservoTask */
/**
* @brief Function implementing the servoTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartservoTask */
void StartservoTask(void *argument)
{
  /* USER CODE BEGIN StartservoTask */
  /* Infinite loop */
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
  uint8_t angle = 0;
  SERVO_Angle(0);
  osDelay(1000);
  for(;;)
  {
    if(angle < 180)
    {
      angle += 45;
      SERVO_Angle(angle);
      osDelay(1000);
    }
    else {
      angle = 0;
      SERVO_Angle(angle);
      osDelay(1000);
    }
  }
  /* USER CODE END StartservoTask */
}

/* USER CODE BEGIN Header_StartmotorTask */
/**
* @brief Function implementing the motorTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartmotorTask */
void StartmotorTask(void *argument)
{
  /* USER CODE BEGIN StartmotorTask */
  /* Infinite loop */
  CAN_FilterTypeDef can_filter;
  can_filter.FilterBank = 0;//0号过滤器
  can_filter.SlaveStartFilterBank = 14;//28组filter
  can_filter.FilterActivation = ENABLE; //激活这个filter
  can_filter.FilterFIFOAssignment = CAN_RX_FIFO0;
  can_filter.FilterMode = CAN_FILTERMODE_IDMASK;//掩码模式
  can_filter.FilterIdHigh = 0;
  can_filter.FilterIdLow = 0;
  can_filter.FilterMaskIdHigh = 0;
  can_filter.FilterMaskIdLow = 0;//全收
  can_filter.FilterScale = CAN_FILTERSCALE_32BIT;//32位宽
  HAL_CAN_ConfigFilter(&hcan1, &can_filter);//配置过滤器
  HAL_CAN_Start(&hcan1);//启动CAN1
  HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO1_MSG_PENDING);//配置接受中断
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartmotorTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void LED_Breath(uint32_t channel)
{
  uint16_t t;
  for(t = 0;t <= 999;t++)
  {
    __HAL_TIM_SET_COMPARE(&htim5, channel, t);
    osDelay(1);
  }
  for(t = 999;t > 0;t--)
  {
    __HAL_TIM_SET_COMPARE(&htim5, channel, t);
    osDelay(1);
  }
  __HAL_TIM_SET_COMPARE(&htim5, channel, 0);
}
void BUZZER_Beep(uint32_t f, uint32_t T)
{
  uint32_t arr;
  arr = 1000000/f - 1;
  __HAL_TIM_SET_AUTORELOAD(&htim4 , arr);//arr
  __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_3, (arr + 1)/2);//ccr
  osDelay(T);
  __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_3, 0);
}
void BUZZER_Music1(void)
{
  uint16_t star[] = {523,523,784,784,880,880,784, 698,698,659,659,587,587,523};
  uint16_t beat[] = {300,300,300,300,300,300,600, 300,300,300,300,300,300,600};
  for(uint8_t i = 0; i < 14; i++)
  {
    BUZZER_Beep(star[i], beat[i]);
    osDelay(50);
  }
}
void BUZZER_Music2(void)
{
  uint16_t sun[] = {784,784,523,523,587,659, 784,784,523,523,587,659,587,523,784};
  uint16_t beat[] = {300,300,300,300,300,450, 300,300,300,300,300,300,300,300,600};
  for(uint8_t i = 0; i < 15; i++)
  {
    BUZZER_Beep(sun[i], beat[i]);
    osDelay(50);
  }
}
void SERVO_Angle(uint32_t angle)
{
  uint32_t ccr;
  ccr = 500 + angle * 2000/180;
  __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, ccr);
}
void CAN_Send(int16_t i)
{
  CAN_TxHeaderTypeDef can_tx_header;
  uint32_t can_tx_box;
  uint8_t tx_Data[8] = {0};
  //Id的配置
  can_tx_header.DLC = 8;//8字节
  can_tx_header.StdId = 0x20;//帧ID
  can_tx_header.IDE = CAN_ID_STD;//标准帧
  can_tx_header.RTR = CAN_RTR_DATA;//数据帧

  tx_Data[0] = (i >> 8) & 0xFF;//高位
  tx_Data[1] = i & 0xFF;//低位
  HAL_CAN_AddTxMessage(&hcan1, &can_tx_header, tx_Data, &can_tx_box);//打包发邮箱
}
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
  CAN_RxHeaderTypeDef rx_header;
  uint8_t rx_data[8];
  
  static uint16_t last_angle;
  uint16_t original_angle;
  int16_t vary ;
  HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rx_header, rx_data);
  if(rx_header.StdId == 0x201)//ID1
  {
    motor_A = (rx_data[0] << 8) | rx_data[1];  
    motor_V = (int16_t)((rx_data[2] << 8) | rx_data[3]); 
    motor_I = (int16_t)((rx_data[4] << 8) | rx_data[5]); 
    motor_t = rx_data[6];                       
  }
}

/* USER CODE END Application */

