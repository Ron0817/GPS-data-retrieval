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
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/*
	*	FILE			: main.c
	*	PROJECT			: Assignment5_Group31.c
	*	PROGRAMMER		: Rongxuan Du, Mingyang Wang, Group31
	*	FIRST VERSION	: 2021-03-15
	*	DESCRIPTION		:
	*		The project implements GPS controller that read gps data from putty and applied
	*		the finite state machine to display different command on both putty and lcd
	*/
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "HD44780_F3.h"
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

/* USER CODE BEGIN PV */
int state = 1;
char stringData[100];
char* delim = ",";
char *token;
char *ID;
char *utcTime;
char *latitude1;
char *latitude2;
char latitude[20];
float latitude3;
char *longtitude1;
char *longtitude2;
char longtitude[20];
float longtitude3;
char *altitude1;
char *altitude2;
char altitude[20];
float altitude3;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */

//FUNCTION			: dataToken
//DESCRIPTION		: applied strtok to gps data, extract important information
//PARAMETERS		: (char*)
//RETURNS			: void
void dataToken(char* );
//FUNCTION			: readButton
//DESCRIPTION		: applied debounce read and not read the buffer until it was unpressed to avoid multiple detection
//PARAMETERS		: (int)
//RETURNS			: int
int readButton(int );
//FUNCTION			: lcdDisplay
//DESCRIPTION		: control lcd display
//PARAMETERS		: (char*, unsigned char)
//RETURNS			: void
void lcdDisplay(char*, unsigned char);
//FUNCTION			: timeDisplay
//DESCRIPTION		: display time as required based on control lcd display
//PARAMETERS		: ()
//RETURNS			: void
void timeDisplay();
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
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  HD44780_Init();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

	  switch(state)
	  {
		  case 1:
			  printf("\n\rVCP starts working state1...Enter GPS data: \n\r");
			  //fgets(stringData, 100, stty);
			  lcdDisplay("Retriving Data..", 0);
			  scanf("%s", stringData);
			  HAL_Delay(500);
			  state = 2;
			  break;
		  case 2:
			  lcdDisplay("Processing Data..", 0);

			  dataToken(stringData); //have been delayed 2500ms
			  HAL_Delay(2500);
			  state = 3;
			  break;
		  case 3:							//state3 - task selection
			  lcdDisplay("1)Time,2)Latitu", 0);
			  lcdDisplay("3)Longi,4)Altitu", 1);
			  if(readButton(GPIO_PIN_3)){			//button1 pressed
				  printf("button1 pressed\r\n");

				 state = 4;
			  }
			  if(readButton(GPIO_PIN_4)){			//button2 pressed
				  printf("button2 pressed\r\n");

				  state = 5;
			  }
			  if(readButton(GPIO_PIN_7)){
				  printf("button3 pressed\r\n");

				  state = 6;
			  }
			  if(readButton(GPIO_PIN_8)){
				  printf("button4 pressed\r\n");
				  state = 7;
			  }
			  break;
		  case 4:					//state 4 - time display
			  timeDisplay();
			  HAL_Delay(5000);
			  state = 3;
			  break;

		  case 5:					//state 5 - altitude display
			  lcdDisplay("Latitude:       ", 0);
			  printf("Latitude is %s\r\n", latitude);
			  int temp1 = latitude3;
			  char msg1[16];
			  sprintf(msg1, "0%dd %02dm %ddd   ", temp1/100, temp1%100, (int)(10000 * (latitude3-temp1)));

			  lcdDisplay(msg1, 1);
			  HAL_Delay(5000);
			  state = 3;
			  break;
		  case 6:					//state 6 - longitude display
			  lcdDisplay("Longitude:      ", 0);
			  printf("longitude is %s\r\n", longtitude);
			  int temp2 = longtitude3;
			  char msg2[16];
			  sprintf(msg2, "0%dd %02dm 0%ddd   ", temp2/100, temp2%100, (int)(100 * (longtitude3-temp2)));
			  lcdDisplay(msg2, 1);
			  HAL_Delay(5000);

			  state = 3;
			  break;

		  case 7:					//state 7 - altitude display
			  lcdDisplay("Altitude:       ", 0);
			  printf("altitude is %s\r\n", altitude);
			  char msg3[16];
			  sprintf(msg3, "%.3f meter   ", altitude3);
			  lcdDisplay(msg3, 1);
			  HAL_Delay(5000);

			  state = 3;
			  break;
		  default:
			  break;

	  }

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

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE|RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 16;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Enable MSI Auto calibration
  */
  HAL_RCCEx_EnableMSIPLLMode();
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
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : PA3 PA4 PA7 PA8 */
  GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_7|GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : LD3_Pin */
  GPIO_InitStruct.Pin = LD3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD3_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

void lcdDisplay(char* temp, unsigned char y)
{
	char str[32];

	sprintf(str, temp);
	//HAL_Delay(250);
	HD44780_GotoXY(0,y);
	HD44780_PutStr(str);

}

void dataToken(char* stringData)
{
	ID = strtok(stringData, ",");
	printf("Sentence ID is %s\r\n", ID);

	utcTime = strtok(NULL, ",");
	int utc;
	utc = atoi(utcTime);
	printf("UTC time is %s => %dh %dm %ds\r\n", utcTime, utc/10000, (utc%10000)/100, (utc%10000)%100);

	latitude1 = strtok(NULL, ",");				//get first latitude pointer
	char * latitudeBuffer = (char *)malloc(20);	//temp buffer
	strcpy(latitudeBuffer, latitude1);			//store first latitude string
	latitude3 = atof(latitudeBuffer);
	latitude2 = strtok(NULL, ",");				//get second latitude pointer
	strcat(latitudeBuffer, latitude2);			//concatenate second latitude string
	strcpy(latitude, latitudeBuffer);			//copy buffer string to latitude string
	printf("Latitude is %s\r\n", latitude);
	free(latitudeBuffer);

	longtitude1 = strtok(NULL, ",");
	char * longtitudeBuffer = (char *)malloc(20);
	strcpy(longtitudeBuffer, longtitude1);
	longtitude3 = atof(longtitudeBuffer);
	longtitude2 = strtok(NULL, ",");
	strcat(longtitudeBuffer, longtitude2);
	strcpy(longtitude, longtitudeBuffer);
	printf("longitude is %s\r\n", longtitude);
	free(longtitudeBuffer);

	token = strtok(NULL, ",");	// position fix 1
	token = strtok(NULL, ",");	//satellites used
	token = strtok(NULL, ",");	//hdop 38466

	altitude1 = strtok(NULL, ",");
	char * altitudeBuffer = (char *)malloc(20);
	strcpy(altitudeBuffer, altitude1);
	altitude3 = atof(altitudeBuffer);
	altitude2 = strtok(NULL, ",");
	strcat(altitudeBuffer, altitude2);
	strcpy(altitude, altitudeBuffer);
	printf("altitude is %s\r\n", altitude);
	free(altitudeBuffer);

//	while (token != NULL){
//		printf("token is %s\n", token);
//		token = strtok(NULL, ",");
//	}
}

int readButton(int pin)
{
	if(HAL_GPIO_ReadPin(GPIOA, pin)){		//debounce read
		HAL_Delay(50);
		if(HAL_GPIO_ReadPin(GPIOA, pin)){
			while(HAL_GPIO_ReadPin(GPIOA, pin))
				{
					while(HAL_GPIO_ReadPin(GPIOA, pin)){
						HAL_GPIO_ReadPin(GPIOA, pin);
					}
					return 1;
				}
		}
	}

}

void timeDisplay()
{
	char str[32];
	int utc;
	utc = atoi(utcTime);
	  printf("UTC time is  => %dh %dm %ds\r\n", utc/10000, (utc%10000)/100, (utc%10000)%100);

	sprintf(str, "%dh%dm%.1ds         ", utc/10000, (utc%10000)/100, (utc%10000)%100);
	HD44780_GotoXY(0,0);
	HD44780_PutStr(str);
}
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
