/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "app_mems.h"
#include "maps.h"
#include "math.h"
#include <stdio.h>
#include <stdlib.h>

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lcd.h"

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
ADC_HandleTypeDef hadc1;

RTC_HandleTypeDef hrtc;

/* USER CODE BEGIN PV */
volatile int fps_flag = 0;

//time
RTC_TimeTypeDef time;
RTC_DateTypeDef date;


RTC_TimeTypeDef czas_przejscia_mapy;

char longText[] = "Potezny Technomag Nekromanta Kurdas Pejon ubezswlasnowolnil przy pomocy swej Magnetometrycznej Rekawicy Mocy dinozaura Babaluge, dzielnego obronce zwierzat z teczowej krainy i uczynil z niego swojego niewolnika. Zlowrogi czarodziej ma zamiar wykorzystac sile pradawnego gadziego ptaka by dystrybuowac po teczowej krainie Mroczne Krysztaly Mocy ze swej zaczarowanej kopalni. Lecz Kurdas musi byc ostrozny, Babaluga nie jest rad z bycia podrzednym niewolnikim i caly czas probuje zlamac uscisk Magnetycznej Rekawicy Mocy Maga. W tym celu stara sie wykorzystac potezny fizyczny bol aplikowany prosto w smoczy pysk. Przeszywajcy dusze nerwowy szok powinien byc w stanie uwolnic Babaluge spod wladzy Rekawicy. Wciel sie w Technomaga Kurdasa i pokieruj dzialaniami Babalugi tak by pozostal on pod twoja wladza, ale badz ostrozny!!! Sprytny smok bedzie celowo biegl po trasach bogatych w kaktusy, pokrzywy i inne nieprzyjemnosci by w chwili twej nieuwagi powaznie sie okaleczyc i zlamac twoj drogocenny czar. Powodzenia Potezny Nekromanto, uzytkownicy Mrocznych Krysztalow licza na Ciebie.";

//textures
const uint8_t babaluga[] = {
										   0b00100,
										   0b01000,
										   0b11111,
										   0b00010,
										   0b00100,
										   0b01000,
										   0b11111,
										   0b00000
	};

const uint8_t kaktus[] = {
										   0b00100,
										   0b01000,
										   0b11111,
										   0b00010,
										   0b01110,
										   0b01110,
										   0b11111,
										   0b00000
	};

const uint8_t krzak[] = {
										   0b00100,
										   0b01110,
										   0b11111,
										   0b00010,
										   0b00100,
										   0b01000,
										   0b11111,
										   0b00000
	};

//game variables
unsigned int score = 0;
unsigned int lifes = 3;

IKS01A3_MOTION_SENSOR_Axes_t magnetic_field_recent;
IKS01A3_MOTION_SENSOR_Axes_t magnetic_field_current;

char data_buffer[16]; //to na testy magnetometra
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_RTC_Init(void);
static void MX_ADC1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void print_score(unsigned int score)
{
	char score_buffer[16];
	lcd_line2();
	sprintf(score_buffer, "Wynik: %d", score);
	lcd_print(2, 1, score_buffer);
}

void print_time_spent_on_map(RTC_TimeTypeDef time_end)
{
	char time_buffer[16];
	int time_spent = time_end.Minutes*60 + time_end.Seconds;

  lcd_line1();
	sprintf(time_buffer, "Czas(sek): %d", time_spent);
	lcd_print(1, 1, time_buffer);
}

void wyswietl_babaluge(int row, int position)
{
		lcd_char(row, position, 0);
}

void wyswietl_kaktus(int position)
{
		lcd_char(2, position, 1);
}

void wyswietl_krzak(int position)
{
		lcd_char(2, position, 2);
}


void start_screen(const char *text)
{
  int len = strlen(text);
  int i, j;

  for (i = 0; i < len + 16 - 1; i++)
  {
    lcd_clear(); // Clear the line
    lcd_line1();

    for (j = 0; j < 16; j++)
    {
      int index = i + j - 16 + 1;
      if (index >= 0 && index < len)
        putchar(text[index]);
      else
        putchar(' ');
    }

    HAL_Delay(500); // Adjust the delay time according to your preference
  }
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	
  int current_map_up[500];
  int current_map_down[500];

  int current_map_length;

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
  MX_RTC_Init();
  MX_ADC1_Init();
  MX_MEMS_Init();
  /* USER CODE BEGIN 2 */

  lcd_init(_LCD_4BIT, _LCD_FONT_5x8, _LCD_2LINE);
  HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);

  ////////////// WGGRYWANIE TEKSTIR
  lcd_cmd(0x40);
  lcd_cmd(0x40 + 0x08); // inicjacja ramu na tekstury
  lcd_cmd(0x40 + 0x10);
 
  for (int i = 0; i < 8; i++)
  {
	  lcd_char_cp(babaluga[i]);
  }

  for (int i = 0; i < 8; i++)
  {
	  lcd_char_cp(kaktus[i]);
  }

  for (int i = 0; i < 8; i++)
  {
	  lcd_char_cp(krzak[i]);
  }
  ///////////////////
  //ustawienie zegara
  czas_przejscia_mapy.Hours = 0;
  czas_przejscia_mapy.Minutes = 0;
  czas_przejscia_mapy.Seconds = 0;

  HAL_RTC_SetTime(&hrtc, &czas_przejscia_mapy, RTC_FORMAT_BIN);

  //////////////// zmienne

  int map_buffor_up[16];
  int map_buffor_down[16];

  int level_number = 1;

  int select_flag = 0;

  int bab_pos = 0;
  int map_pos = 0;
  int jumpCounter = 0;
  int is_jumping = 0;
  int game_state = 3; // 0-stan startowy, 1-gra, 2-tranzycja, 3-wyświetlanie lore

  int texture_value_up = 0;
  int texture_value_down = 0;
 
  ///////////////
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  //print_time_spent_on_map(czas_przejscia_mapy);
    uint32_t wartosc_z_adc = HAL_ADC_GetValue(&hadc1);
	  int stan_guzika = (wartosc_z_adc + 300) / 700;

    //magnetometr
      if(abs(magnetic_field_current.x - magnetic_field_recent.x) > 500)
      {
        is_jumping = 1;
        jumpCounter = 0;
      }

//tryby gry
    switch (stan_guzika) {
	  	case 1:
	  		//obsluga skoku przez guzik UP
        if(!is_jumping){
          is_jumping = 1;
          jumpCounter = 0;
        }
	  	  break;
	  	case 4:
	  		if(!select_flag){
          select_flag = 1;
        }
	  	  break;
	  	default:
	  	  break;
	  	}

	  if (fps_flag && game_state == 1) //mamy stan gry "gra"
    {
      for(int i=map_pos, j =0; i < map_pos + 16; i++, j++){ // ładowanie mapy do bufora
          map_buffor_up[j] = current_map_up[i];
          map_buffor_down[j] = current_map_down[i];
        }
//dodajemy punkty za kazdy tick pętli(za kazdy blok ktory sie przeszlo)
      score++;

	    HAL_RTC_GetTime(&hrtc, &czas_przejscia_mapy, RTC_FORMAT_BIN);
	    HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN);
      HAL_ADC_Start(&hadc1);
	  	HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
      MX_MEMS_Process();

	  	lcd_clear();

//wyswietlanie babalugi i skoku
      if(is_jumping){
        jumpCounter ++;
        bab_pos = 1;
        wyswietl_babaluge(1, 2);
      }
      else{
        wyswietl_babaluge(2, 2);
      }

//wyswietlanie mapy
      for(int i=0; i < 16; i++){
        texture_value_up = map_buffor_down[i];
        texture_value_down = map_buffor_down[i];
        
        
        switch(texture_value_up) //up row switch
        {
          case 0:
            lcd_print(1, i, " ");
            break;
          case 4:
            lcd_print(1, i, "V");
            break;
          default:
            lcd_print(1, i, "!");
            break;
        }
        switch(texture_value_down) // down row switch
        {
          case 0:
            lcd_print(2, i, " ");
            break;
          case 1:
            wyswietl_kaktus(i);
            break;
          case 2:
            wyswietl_krzak(i);
            break;
          case 3:
            lcd_print(2, i, "A");
            break;
          default:
            lcd_print(2, i, "!");
            break;
        }
      }
	  	map_pos ++;
//przejscie do wyniku, ukonczenie mapy
      if(map_pos + 16 >= current_map_length){
        game_state = 2;
      }

	  }

    if(game_state == 0){
      // intro gry
    }

    if(game_state == 2){

      //wczytanie kolejnej mapy
      
      switch(level_number){
        case 2:
          
          break;
        case 3:

          break;
      }

      while(select_flag){
        print_score(score);
        print_time_spent_on_map(czas_przejscia_mapy);
      }
      game_state = 1;
      // level transition
    }

    
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

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_MultiModeTypeDef multimode = {0};
  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc1.Init.LowPowerAutoWait = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc1.Init.OversamplingMode = DISABLE;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure the ADC multi-mode
  */
  multimode.Mode = ADC_MODE_INDEPENDENT;
  if (HAL_ADCEx_MultiModeConfigChannel(&hadc1, &multimode) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_5;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_2CYCLES_5;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

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
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LCD_D6_Pin|LCD_D5_Pin|LCD_D4_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LCD_EN_GPIO_Port, LCD_EN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LCD_D7_Pin|LCD_RS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : LCD_D6_Pin LCD_D5_Pin LCD_D4_Pin */
  GPIO_InitStruct.Pin = LCD_D6_Pin|LCD_D5_Pin|LCD_D4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : LCD_EN_Pin */
  GPIO_InitStruct.Pin = LCD_EN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LCD_EN_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LCD_D7_Pin LCD_RS_Pin */
  GPIO_InitStruct.Pin = LCD_D7_Pin|LCD_RS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

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
