/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "hal.h"
#include "wolfboot/wolfboot.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef struct
{
  uint8_t start;
  uint8_t length;
  uint8_t data[64];
  uint8_t end;
  uint8_t index;
}ota_data_frame_t;

typedef enum
{
  OTA_ERR_FRAME = 0,
  OTA_BEGIN_FRAME,
  OTA_DATA_FRAME,
}ota_frame_t;
typedef enum 
{
  NOT_RECEIVING =0,
  STATE_RECEIVING_DATA_FRAME,
  STATE_RECEIVING_BEGIN_OTA,
  
}status;
typedef struct
{
  uint8_t begin_start;
  uint16_t file_size;
  uint8_t begin_end;
}ota_begin_frame_t;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define OTA_RX_BUFFER_SIZE          128

/* Frame bytes */
#define OTA_FRAME_ACK_BYTE          0x01

#define OTA_FRAME_ERR_BYTE          0x02

#define OTA_FRAME_DATA_START_BYTE   0x03
#define OTA_FRAME_DATA_END_BYTE     0x04

#define OTA_FRAME_BEGIN_START_BYTE  0x05
#define OTA_FRAME_BEGIN_END_BYTE    0x06

#define OTA_FRAME_FINISHED_BYTE     0x07
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef hlpuart1;

osThreadId otaTaskHandle;
osMessageQId otaQueueHandle;
/* USER CODE BEGIN PV */
uint8_t otaRxBuffer[OTA_RX_BUFFER_SIZE];
uint8_t otaRxByte;
volatile uint8_t otaRxIndex;
volatile uint8_t dataIndex;
ota_data_frame_t dataFrame;
ota_begin_frame_t beginFrame;
volatile uint32_t dst=WOLFBOOT_PARTITION_UPDATE_ADDRESS;
uint8_t writtenLength;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_LPUART1_UART_Init(void);
void StartOtaTask(void const * argument);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int fputc(int ch, FILE* stream)
{
  HAL_UART_Transmit(&hlpuart1, (uint8_t *)&ch, 1, 0xFFFF);
  return ch;
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */
  

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_LPUART1_UART_Init();
  /* USER CODE BEGIN 2 */
  HAL_UART_Receive_IT(&hlpuart1, &otaRxByte, 1);
//__HAL_UART_ENABLE_IT(&hlpuart1, UART_IT_RXNE);

  /* USER CODE END 2 */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* definition and creation of otaQueue */
  osMessageQDef(otaQueue, 8, uint8_t);
  otaQueueHandle = osMessageCreate(osMessageQ(otaQueue), NULL);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of otaTask */
  osThreadDef(otaTask, StartOtaTask, osPriorityNormal, 0, 128);
  otaTaskHandle = osThreadCreate(osThread(otaTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* Start scheduler */
  osKernelStart();
  
  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure the main internal regulator output voltage 
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 20;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_LPUART1;
  PeriphClkInit.Lpuart1ClockSelection = RCC_LPUART1CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief LPUART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_LPUART1_UART_Init(void)
{

  /* USER CODE BEGIN LPUART1_Init 0 */

  /* USER CODE END LPUART1_Init 0 */

  /* USER CODE BEGIN LPUART1_Init 1 */

  /* USER CODE END LPUART1_Init 1 */
  hlpuart1.Instance = LPUART1;
  hlpuart1.Init.BaudRate = 115200;
  hlpuart1.Init.WordLength = UART_WORDLENGTH_8B;
  hlpuart1.Init.StopBits = UART_STOPBITS_1;
  hlpuart1.Init.Parity = UART_PARITY_NONE;
  hlpuart1.Init.Mode = UART_MODE_TX_RX;
  hlpuart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  hlpuart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  hlpuart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  hlpuart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  hlpuart1.FifoMode = UART_FIFOMODE_DISABLE;
  if (HAL_UART_Init(&hlpuart1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&hlpuart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&hlpuart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN LPUART1_Init 2 */

  /* USER CODE END LPUART1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  HAL_PWREx_EnableVddIO2();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14|GPIO_PIN_7, GPIO_PIN_RESET);

  /*Configure GPIO pins : PB14 PB7 */
  GPIO_InitStruct.Pin = GPIO_PIN_14|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
ota_frame_t otaFrameDetect(void)
{
  ota_frame_t result;
  status state=NOT_RECEIVING;
  result = OTA_ERR_FRAME;
  /* Reset buffer when full */
  if(otaRxIndex == OTA_RX_BUFFER_SIZE)
  {
    otaRxIndex = 0;
  }
  /* Store received byte in buffer */
  otaRxBuffer[otaRxIndex] = otaRxByte;
  otaRxIndex++;
  /* Detect state */
  if(otaRxBuffer[0] == OTA_FRAME_DATA_START_BYTE)
  {
    state = STATE_RECEIVING_DATA_FRAME;
  }
   if(otaRxBuffer[0] == OTA_FRAME_BEGIN_START_BYTE)
  {
    state = STATE_RECEIVING_BEGIN_OTA;
  }
  switch(state)
  {
  /* Fill data frame */
  case STATE_RECEIVING_DATA_FRAME:
    if(otaRxIndex == 1)
    {
      dataFrame.start = otaRxByte;
    }
    else if(otaRxIndex == 2)
    {
      dataFrame.length = otaRxByte;
    }
    else if ((otaRxIndex > 2) && (otaRxIndex <= dataFrame.length + 2))
    {
      dataFrame.data[dataIndex] = otaRxByte;
      dataIndex++;
    }
    else if(otaRxIndex == sizeof(dataFrame.start) + sizeof(dataFrame.length) + dataFrame.length+1)
    {
      dataFrame.end = otaRxByte;
      dataFrame.index=dataIndex;
      otaRxIndex = 0;
      dataIndex = 0;
      memset(otaRxBuffer, 0, OTA_RX_BUFFER_SIZE);

      result = OTA_DATA_FRAME;
    }
    break;
   /* Fill begin_ota frame */
   case STATE_RECEIVING_BEGIN_OTA:
     /* TODO */
      if(otaRxIndex == 1)
    {
      beginFrame.begin_start = otaRxByte;
    }
    else if (otaRxIndex ==2)
    {
      beginFrame.file_size = ((uint16_t)otaRxBuffer[1] << 8);
    }
    else if(otaRxIndex ==3)
    {
      beginFrame.file_size |= otaRxBuffer[2];
    }
    else if(otaRxIndex == 4)
    {
      beginFrame.begin_end = otaRxByte;
      otaRxIndex = 0;
      memset(otaRxBuffer, 0, OTA_RX_BUFFER_SIZE);
      result = OTA_BEGIN_FRAME;
    }
     break;
   default:
     break;
  }
  return result;
}


/**
  * @brief  Process frame
  * @param  None
  * @retval None
  */
void otaProcessFrame(uint32_t message)
{
  /* TODO: 1. Validate frame according to its type
           2. if valid --> write data to flash --> send ack back to host
           3. if not valid --> send err to host
           4. if failed to write data to flash --> send err to host
   */
 
  uint8_t Tx=0;
  uint8_t writtenLength;
  switch(message)
  {
  case OTA_BEGIN_FRAME:
  
    if (beginFrame.begin_end ==OTA_FRAME_BEGIN_END_BYTE)
    {
    /*Send ACK to host */ 
     Tx=OTA_FRAME_ACK_BYTE;
     
     HAL_UART_Transmit(&hlpuart1,(uint8_t *)&Tx ,1,HAL_MAX_DELAY);
    }
    else 
    {
     Tx=OTA_FRAME_ERR_BYTE;
     
     HAL_UART_Transmit(&hlpuart1,(uint8_t *)&Tx ,1,HAL_MAX_DELAY);
     
    beginFrame.begin_start=0x00;
     beginFrame.file_size= 0x0000;
     beginFrame.begin_end = 0x00;

    }
    break;
    case OTA_DATA_FRAME:
      
      if ((dataFrame.end ==OTA_FRAME_DATA_END_BYTE)&& (dataFrame.length==dataFrame.index))
        {
         
          
            if ((dst % WOLFBOOT_SECTOR_SIZE) == 0) 
            {
            hal_flash_erase(dst, WOLFBOOT_SECTOR_SIZE);
            }
            /* when an address of the flash contains a value different from '0xFFFF FFFF*/
            
            
           // if (dataFrame.length
          /* frame is valid */ 
            if (hal_flash_write(dst, (uint8_t*)&dataFrame.data, (int)dataFrame.length)==0)
              {
//                if (dataFrame.length % 8!=0)
//                  dst+=8;
//                else
            dst+=dataFrame.length;
            writtenLength=dst-WOLFBOOT_PARTITION_UPDATE_ADDRESS;
            Tx=OTA_FRAME_ACK_BYTE;
     
            HAL_UART_Transmit(&hlpuart1,(uint8_t *)&Tx ,1,HAL_MAX_DELAY);
            dataFrame.start=0;
            memset(dataFrame.data, 0, dataFrame.length);
            dataFrame.end=0;
            if (writtenLength==beginFrame.file_size)
            {
              Tx=OTA_FRAME_FINISHED_BYTE;
     
            HAL_UART_Transmit(&hlpuart1,(uint8_t *)&Tx ,1,HAL_MAX_DELAY);
            }
              }
            
            
            else 
              {
            /* couldn't write to flash*/
            Tx=OTA_FRAME_ERR_BYTE;
     
            HAL_UART_Transmit(&hlpuart1,(uint8_t *)&Tx ,1,HAL_MAX_DELAY);
              }
          }
       else if (dataFrame.length>0x40 ) /* data length is bigger than 64 bytes*/ 
        {
          Tx=OTA_FRAME_ERR_BYTE;
     
          HAL_UART_Transmit(&hlpuart1,(uint8_t *)&Tx ,1,HAL_MAX_DELAY);
        }
      else 
      { /* invalid frame*/ 
        Tx=OTA_FRAME_ERR_BYTE;
     
        HAL_UART_Transmit(&hlpuart1,(uint8_t *)&Tx ,1,HAL_MAX_DELAY);
      }
          break;
    default:
    break;
 }
}


/**
  * @brief  Rx Transfer completed callback.
  * @param  huart UART handle.
  * @retval None
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  ota_frame_t frameType;

  if(huart->Instance == LPUART1)
  {
     
    frameType = otaFrameDetect();
    if(frameType != OTA_ERR_FRAME)
    {
      osMessagePut(otaQueueHandle, (uint32_t)frameType, osWaitForever);
    }
    HAL_UART_Receive_IT(&hlpuart1, &otaRxByte, 1);
  }
}
/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartOtaTask */
/**
  * @brief  Function implementing the otaTask thread.
  * @param  argument: Not used 
  * @retval None
  */
/* USER CODE END Header_StartOtaTask */
void StartOtaTask(void const * argument)
{
  /* USER CODE BEGIN 5 */
  osEvent event;
  uint32_t message;
  /* Infinite loop */
  for(;;)
  {
    event = osMessageGet(otaQueueHandle, osWaitForever);
    if(event.status == osEventMessage)
    {
      message = event.value.v;
      switch(message)
      {
      case OTA_BEGIN_FRAME:
        //printf("New begin frame detected!\r\n");
        //printf("Processing frame...\r\n");
        otaProcessFrame(OTA_BEGIN_FRAME);
        break;
      case OTA_DATA_FRAME:
        //printf("New data frame detected!\r\n");
        //printf("Processing frame...\r\n");
        otaProcessFrame(OTA_DATA_FRAME);
        break;
      default:
        printf("Thread received an unknown message\r\n");
        break;
      }
    }
  }
  /* USER CODE END 5 */ 
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM2 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM2) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(char *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
