/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#include "usb_device.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "edge-impulse-sdk/classifier/ei_run_classifier.h"

using namespace ei;

// paste the raw features here
static const float features[] = {
		162.2424, -27.8252, 454.5918, 270.9091, -55.2039, 392.2041, 535.6364, -72.6214, -29.3265, 509.0909, -8.4272, -298.5918, 522.2424, -22.6408, -169.1020, 396.5454, -51.4757, 269.1429, 100.2424, 1.8252, 471.5510, 8.4242, 34.5631, 473.7551, 381.7575, -81.6505, 270.7959, 551.9393, -37.2039, -110.9388, 556.0605, 33.2816, -198.4286, 508.7272, -25.4369, 93.3674, 132.3030, -9.0680, 469.1633, -100.0000, 52.0388, 458.0816, 11.1515, 16.1553, 442.8979
};

int get_feature_data(size_t offset, size_t length, float *out_ptr) {
    memcpy(out_ptr, features + offset, length * sizeof(float));
    return 0;
}
#include "usbd_cdc_if.h"
#include "string.h"
#include "MY_LIS3DSH.h"
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
LIS3DSH_InitTypeDef myAccConfigDef;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
CRC_HandleTypeDef hcrc;

SPI_HandleTypeDef hspi1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
static void MX_CRC_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#include <stdarg.h>
#include "edge-impulse-sdk/classifier/ei_run_classifier.h"

void vprint(const char *fmt, va_list argp)
{
    char string[200];
    if(0 < vsprintf(string, fmt, argp)) // build string
    {
    	CDC_Transmit_FS((uint8_t*)string, strlen(string));

    }
}

void ei_printf(const char *format, ...) {
    va_list myargs;
    va_start(myargs, format);
    vprint(format, myargs);
    va_end(myargs);
}
    char *comma = ",";
	char *end = " \n";
	char *message[500] = {};
    char * recivebuffer[64];
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
  signal_t signal;
  signal.total_length = sizeof(features) / sizeof(features[0]);
  signal.get_data = &get_feature_data;

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USB_DEVICE_Init();
  MX_SPI1_Init();
  MX_CRC_Init();
  /* USER CODE BEGIN 2 */
  myAccConfigDef.dataRate = LIS3DSH_DATARATE_1600;
  	myAccConfigDef.fullScale = LIS3DSH_FULLSCALE_2;
  	myAccConfigDef.antiAliasingBW = LIS3DSH_FILTER_BW_50;//update rate
  	myAccConfigDef.enableAxes = LIS3DSH_XYZ_ENABLE;
  	myAccConfigDef.interruptEnable = false;
  	LIS3DSH_Init(&hspi1, &myAccConfigDef);
  	LIS3DSH_DataScaled myscaledata;
  	LIS3DSH_X_calibrate(-1000.0, 980.0);
  	    LIS3DSH_Y_calibrate(-1020.0, 1040.0);
  	    LIS3DSH_Z_calibrate(-920.0, 1040.0);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  	  while (1)
  	  {
  	        ei_impulse_result_t result = { 0 };
  	        EI_IMPULSE_ERROR res = run_classifier(&signal, &result, true);
  	        ei_printf("run_classifier returned: %d\n", res);

  	        ei_printf("Predictions (DSP: %d ms., Classification: %d ms., Anomaly: %d ms.): \n",
  	            result.timing.dsp, result.timing.classification, result.timing.anomaly);

  	        // print the predictions
  	        ei_printf("[");
  	        for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) {
  	      	  ei_printf_float(result.classification[ix].value);
  	    #if EI_CLASSIFIER_HAS_ANOMALY == 1
  	            ei_printf(", ");
  	    #else
  	            if (ix != EI_CLASSIFIER_LABEL_COUNT - 1) {
  	                ei_printf(", ");
  	            }
  	    #endif
  	        }
  	    #if EI_CLASSIFIER_HAS_ANOMALY == 1
  	        ei_printf_float(result.anomaly);
  	    #endif
  	        ei_printf("]\n\n\n");

  	      HAL_Delay(5000);
  	  }
	 // CDC_Transmit_FS(sendbuffer, strlen(sendbuffer));
	//  HAL_Delay(100);
	  	 char *ret;
	 			  	    if (!ret)
	 			  	    {
	  if (LIS3DSH_PollDRDY(100)==true)
	  {
		myscaledata=LIS3DSH_GetDataScaled();
		char buf[50];
		sprintf(buf,"%f,%f,%f\n",myscaledata.x,myscaledata.y,myscaledata.z);
		/* char *bufx[20];
			  	char *bufy[20];
			  	char *bufz[20];
				memset( bufx, 0, sizeof( bufx));
				memset( bufy, 0, sizeof( bufy));
				memset( bufz, 0, sizeof( bufz));
			  	gcvt(myscaledata.x, 9, bufx);
			  	gcvt(myscaledata.y, 9, bufy);
			  	gcvt(myscaledata.z, 9, bufz);
			  	strcat(message,bufx);
			  	strcat(message,comma);
			  	strcat(message,bufy);
			  	strcat(message,comma);
			  	strcat(message,bufz);
			  	strcat(message,end); */




			  	CDC_Transmit_FS((uint8_t *) buf, strlen(buf));
			  	  HAL_Delay(300)
			  	;



	  }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	 			  	    }
  }
  /* USER CODE END 3 */


/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief CRC Initialization Function
  * @param None
  * @retval None
  */
static void MX_CRC_Init(void)
{

  /* USER CODE BEGIN CRC_Init 0 */

  /* USER CODE END CRC_Init 0 */

  /* USER CODE BEGIN CRC_Init 1 */

  /* USER CODE END CRC_Init 1 */
  hcrc.Instance = CRC;
  if (HAL_CRC_Init(&hcrc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CRC_Init 2 */

  /* USER CODE END CRC_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

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
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3|GPIO_PIN_0, GPIO_PIN_RESET);

  /*Configure GPIO pins : PE3 PE0 */
  GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

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
