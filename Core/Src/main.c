/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "main.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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
UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart2_rx;
DMA_HandleTypeDef hdma_usart2_tx;

/* USER CODE BEGIN PV */
char AtCommand[80];
uint8_t Reply[80];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */

int checkNetwork()
{
	//AT COMMAND TEST
	memset(AtCommand, 0, strlen(AtCommand));
	memset(Reply, 0, strlen((char*)Reply));
	sprintf(AtCommand, "AT\r\n");
	HAL_UART_Transmit(&huart2, (uint8_t *)AtCommand, strlen(AtCommand), 1000);
	HAL_UART_Receive(&huart2, Reply, 80, 1000);
	if(strstr((char *)Reply,"OK"))
	  {
		//ATisOK = 1;
		//HAL_UART_Transmit(&huart3,(uint8_t *)rx_buffer,strlen((char *)rx_buffer),1000);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, 1);
		HAL_Delay(3000);


	  }
	else
		{
			return 0;
		}

	// SIMCARD PIN
	memset(AtCommand, 0, strlen(AtCommand));
	memset(Reply, 0, strlen((char*)Reply));
	sprintf(AtCommand, "AT+CPIN=?\r\n");
	HAL_UART_Transmit(&huart2, (uint8_t *)AtCommand, strlen(AtCommand), 1000);
	HAL_UART_Receive(&huart2, Reply, 80, 1000);
	if(strstr((char *)Reply,"OK"))
		  {
			//ATisOK = 1;
			//HAL_UART_Transmit(&huart3,(uint8_t *)rx_buffer,strlen((char *)rx_buffer),1000);
			//HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, 1);
			HAL_Delay(1000);


		  }
		else if(strstr((char *)Reply,"ERROR"))
			{
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, 0);
			HAL_Delay(1000);
				return 0;
			}

	//SIM CARD PIN
	memset(AtCommand, 0, strlen(AtCommand));
	memset(Reply, 0, strlen((char*)Reply));
	sprintf(AtCommand, "AT+CPIN?\r\n");
	HAL_UART_Transmit(&huart2, (uint8_t *)AtCommand, strlen(AtCommand), 1000);
	HAL_UART_Receive(&huart2, Reply, 80, 1000);

	memset(AtCommand, 0, strlen(AtCommand));
	memset(Reply, 0, strlen((char*)Reply));
	sprintf(AtCommand, "AT+CIMI=?\r\n");
	HAL_UART_Transmit(&huart2, (uint8_t *)AtCommand, strlen(AtCommand), 1000);
	HAL_UART_Receive(&huart2, Reply, 80, 1000);

	//International mobile subscriber
	memset(AtCommand, 0, strlen(AtCommand));
	memset(Reply, 0, strlen((char*)Reply));
	sprintf(AtCommand, "AT+CIMI\r\n");
	HAL_UART_Transmit(&huart2, (uint8_t *)AtCommand, strlen(AtCommand), 1000);
	HAL_UART_Receive(&huart2, Reply, 80, 1000);

	// Network registration
	memset(AtCommand, 0, strlen(AtCommand));
	memset(Reply, 0, strlen((char*)Reply));
	sprintf(AtCommand, "AT+CREG?\r\n");
	HAL_UART_Transmit(&huart2, (uint8_t *)AtCommand, strlen(AtCommand), 1000);
	HAL_UART_Receive(&huart2, Reply, 80, 1000);

	// signal quality
	memset(AtCommand, 0, strlen(AtCommand));
	memset(Reply, 0, strlen((char*)Reply));
	sprintf(AtCommand, "AT+CSQ\r\n");
	HAL_UART_Transmit(&huart2, (uint8_t *)AtCommand, strlen(AtCommand), 1000);
	HAL_UART_Receive(&huart2, Reply, 80, 1000);
	return 1;
}

int sendSMS()
{
	 memset(AtCommand, 0, sizeof(AtCommand));
	memset(Reply, 0, sizeof(Reply));

	// Set SMS text mode
	sprintf(AtCommand, "AT+CMGF=1\r\n");
	HAL_UART_Transmit(&huart2, (uint8_t *)AtCommand, strlen(AtCommand), 1000);
	HAL_UART_Receive(&huart2, Reply, sizeof(Reply), 1000);

	if (strstr((char *)Reply, "OK") == NULL) {
		return 0; // Error setting text mode
	}

	// Set recipient phone number
	memset(AtCommand, 0, sizeof(AtCommand));
	sprintf(AtCommand, "AT+CMGS=\"+254750558407\"\r\n");
	HAL_UART_Transmit(&huart2, (uint8_t *)AtCommand, strlen(AtCommand), 1000);
	HAL_UART_Receive(&huart2, Reply, sizeof(Reply), 1000);

	if (strstr((char *)Reply, "> ") == NULL) { // Check for prompt
		return 0; // Error setting phone number
	}

	// Send SMS message
	memset(AtCommand, 0, sizeof(AtCommand));
	sprintf(AtCommand, "Hello\r\n");
	HAL_UART_Transmit(&huart2, (uint8_t *)AtCommand, strlen(AtCommand), 1000);
	AtCommand[0] = 26; // Ctrl+Z
	AtCommand[1] = '\0';
	HAL_UART_Transmit(&huart2, (uint8_t *)"\x1A", 1, 1000);
	 memset(Reply, 0, sizeof(Reply));
	HAL_UART_Receive(&huart2, Reply, sizeof(Reply), 1000);

	return 1;
}

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_DMA_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, 0);
  HAL_Delay(5000);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, 1);
  HAL_Delay(5000);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, 0);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (!checkNetwork())
  {
	  checkNetwork();
	  HAL_Delay(1000);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  sendSMS();
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel6_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel6_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel6_IRQn);
  /* DMA1_Channel7_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel7_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel7_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET);

  /*Configure GPIO pin : PB15 */
  GPIO_InitStruct.Pin = GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PA15 */
  GPIO_InitStruct.Pin = GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
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
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
