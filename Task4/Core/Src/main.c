/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
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
#include "main.h"
#include "sensor.h"
#include "uart_log.h"
#include <stdint.h>

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define BTN_DEBOUNCE_MS 50U    /* Buton debounce bekleme süresi  */
#define LED_BOOT_BLINK 3U      /* Boot testindeki yanıp sönme sayısı */
#define LED_BLINK_FAST_MS 100U /* UART hata bildirimi yanıp sönme   */
#define LED_BLINK_ERR_N 5U     /* UART hata bildirimi tekrar sayısı  */

/* Ölçüm sayacı (her basışta artar, Serial Monitor'de görünür) */
#define MEAS_COUNTER_MAX 9999U
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */
static SensorData g_sensor = {0};
static uint32_t g_meas_cnt = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_USART1_UART_Init(void);

// Custom functions
static void boot_led_test(void);
static void led_error_blink(void);
static void do_measurement(void);
static uint8_t btn_is_pressed(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick.
   */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ADC1_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */

  // Boot test
  boot_led_test();
  UART_Print(&huart1, "[SISTEM] TUFAN Sensor Dashboard baslatiliyor...\r\n");
  UART_Print(&huart1, "[SISTEM] LED yanip sonuyor (3x boot testi)...\r\n");
  UART_Print(&huart1, "[SISTEM] Hazir. Olcum icin butona basiniz.\r\n\r\n");

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1) {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    if (btn_is_pressed()) {
      do_measurement();

      // Tek basış garantisi: buton bırakılana kadar bekle
      while (HAL_GPIO_ReadPin(BTN_INPUT_GPIO_Port, BTN_INPUT_Pin) ==
             GPIO_PIN_SET) {
        HAL_Delay(10);
      }
    }
  }
  /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
    Error_Handler();
  }
}

/**
 * @brief ADC1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_ADC1_Init(void) {

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Common config
   */
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  if (HAL_ADC_Init(&hadc1) != HAL_OK) {
    Error_Handler();
  }

  /** Configure Regular Channel
   */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */
}

/**
 * @brief USART1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART1_UART_Init(void) {

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK) {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */
}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_PIN_GPIO_Port, LED_PIN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : LED_PIN_Pin */
  GPIO_InitStruct.Pin = LED_PIN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_PIN_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : BTN_INPUT_Pin */
  GPIO_InitStruct.Pin = BTN_INPUT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(BTN_INPUT_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/**
 * @brief  Boot başarı testi: LED 3 kez yanıp söner, sonra söner.
 */
static void boot_led_test(void) {
  for (uint8_t i = 0; i < LED_BOOT_BLINK; i++) {
    HAL_GPIO_WritePin(LED_PIN_GPIO_Port, LED_PIN_Pin,
                      GPIO_PIN_RESET); /* LED YAK (active-low) */
    HAL_Delay(300);
    HAL_GPIO_WritePin(LED_PIN_GPIO_Port, LED_PIN_Pin,
                      GPIO_PIN_SET); /* LED SÖNDÜR           */
    HAL_Delay(300);
  }
  /* Boot sonrası LED söndürülmüş kalır */
  HAL_GPIO_WritePin(LED_PIN_GPIO_Port, LED_PIN_Pin, GPIO_PIN_SET);
}

/**
 * @brief  UART hatası görsel bildirimi: LED 5 kez hızlı yanıp söner.
 */
static void led_error_blink(void) {
  for (uint8_t i = 0; i < LED_BLINK_ERR_N; i++) {
    HAL_GPIO_WritePin(LED_PIN_GPIO_Port, LED_PIN_Pin, GPIO_PIN_RESET);
    HAL_Delay(LED_BLINK_FAST_MS);
    HAL_GPIO_WritePin(LED_PIN_GPIO_Port, LED_PIN_Pin, GPIO_PIN_SET);
    HAL_Delay(LED_BLINK_FAST_MS);
  }
}

/**
 * @brief  Buton aktif mi? (debounce dahil)
 *         PA0 PULL-DOWN ile yapılandırıldığında:
 *           Basılı → GPIO_PIN_SET (HIGH)
 *           Bırakılmış → GPIO_PIN_RESET (LOW)
 * @retval 1: basılı ve kararlı, 0: basılı değil
 */
static uint8_t btn_is_pressed(void) {
  if (HAL_GPIO_ReadPin(BTN_INPUT_GPIO_Port, BTN_INPUT_Pin) ==
      GPIO_PIN_SET) {
    HAL_Delay(BTN_DEBOUNCE_MS); /* Debounce: 50 ms bekle */
    if (HAL_GPIO_ReadPin(BTN_INPUT_GPIO_Port, BTN_INPUT_Pin) ==
        GPIO_PIN_SET) {
      return 1;
    }
  }
  return 0;
}

/**
 * @brief  Tam ölçüm döngüsü:
 *         LED yak → ADC oku → UART'a gönder → LED söndür
 */
static void do_measurement(void) {
  HAL_StatusTypeDef status;
  char buf[64];

  /* LED YAK — ölçüm devam ediyor */
  HAL_GPIO_WritePin(LED_PIN_GPIO_Port, LED_PIN_Pin, GPIO_PIN_RESET);

  /* ADC OKU */
  status = sensor_read(&hadc1, &g_sensor);

  if (status == HAL_TIMEOUT) {
    UART_Print(&huart1, "[HATA] ADC zaman asimi! Olcum atlandi.\r\n");
    HAL_GPIO_WritePin(LED_PIN_GPIO_Port, LED_PIN_Pin, GPIO_PIN_SET);
    return;
  }

  if (status != HAL_OK || g_sensor.valid == 0) {
    UART_Print(&huart1,
               "[UYARI] ADC degeri aralik disi! Donanim kontrol edin.\r\n");
    HAL_GPIO_WritePin(LED_PIN_GPIO_Port, LED_PIN_Pin, GPIO_PIN_SET);
    return;
  }

  /* ÖLÇÜM SAYACINI ARTTIR */
  g_meas_cnt++;
  if (g_meas_cnt > MEAS_COUNTER_MAX)
    g_meas_cnt = 1;

  /* UART ÇIKTISI */
  snprintf(buf, sizeof(buf),
           "\r\n======== TUFAN SENSOR OKUMASI #%lu ========\r\n",
           (unsigned long)g_meas_cnt);

  HAL_StatusTypeDef uart_status =
      HAL_UART_Transmit(&huart1, (uint8_t *)buf, (uint16_t)strlen(buf), 100);
  if (uart_status != HAL_OK) {
    led_error_blink();
    return;
  }

  UART_PrintUint(&huart1, "Ham ADC Degeri : ", g_sensor.raw);

  /* Gerilim ve batarya değerlerini birim ile birlikte yazdır */
  {
    char vbuf[64];
    snprintf(vbuf, sizeof(vbuf), "Gerilim        : %.2f V\r\n", g_sensor.voltage);
    UART_Print(&huart1, vbuf);

    snprintf(vbuf, sizeof(vbuf), "Batarya Sim.   : %.1f %%\r\n", g_sensor.percent);
    UART_Print(&huart1, vbuf);
  }
  UART_Print(&huart1, "LED Durumu     : ACIK (olcum)\r\n");
  UART_Print(&huart1, "=========================================\r\n");

  /* LED SÖNDÜR — ölçüm bitti */
  HAL_GPIO_WritePin(LED_PIN_GPIO_Port, LED_PIN_Pin, GPIO_PIN_SET);
}

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1) {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line) {
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line
     number, ex: printf("Wrong parameters value: file %s on line %d\r\n", file,
     line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
