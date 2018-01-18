/*! \mainpage I2C Sensor lesen und über CAN senden
 *
 * \section intro_sec Einführung
 *
 * Mit Hilfe dieses Skripts, kann mit Hilfe des Boards STM Nucleo f072rb der Sensor DS1621 über den I2C Bus ausgelesen werden und der ausgelesene Wert über den CAN Bus verschickt werden.
 * Dieses Programm wurde erstellt von: Yannick Mayer, Simon Petersen, Ulrich Ludmann
 * \section install_sec Installation
 *
 * Die kompilierte datei finden Sie im Verzeichnis Debug ABGABE_LN.elf, außerdem die main.c File mit dem Code.
 *
 */

/**
  ******************************************************************************
  * @file    main.c
  * @author  Petersen, Mayer, Ludmann
  * @version V0.5.8
  * @date    16-Januar-2018
  * @brief   Hier ist unsere main.c Datei
  ******************************************************************************
*/


//   Dependencies includen
 #include "stm32f0xx.h"
 #include "stm32f0xx_nucleo.h"
 #include "stm32f0xx_hal_can.h"  //*CAN
 #include "stm32f0xx_hal_gpio.h" //*I2C
 #include "stm32f0xx_hal_gpio_ex.h"  //*I2C
// #include "cli_uart.h"                                                                                     //*TeraTerm


 //*Festlegen der Slave-Adresse (8Bit) des DS1621 Temperatursensors
 #define SLAVE_ADD_8BIT (0x9E)

 /*********************************************************************//**
 * @brief   Funktionsaufruf SystemClock und Error Handler
 *  **********************************************************************/
 void SystemClock_Config(void);


 void Error_Handler(void);



 /*********************************************************************//**
 * @brief  main routine
 **********************************************************************/
 int main(void)
 {
	 //*Initialisierung
	 HAL_Init();
	 SystemClock_Config();   /*!< Takt geht los */

	 /*!< Variablen Deklaration */
	 uint8_t RxBuf[2]; /*!< Variable für Datenempfang des Sensors (2 Byte) */
	 uint8_t TxBuf[0]; /*!< Variable zum Senden der Commandobytes an DS1621 Temperatursensor */
	 uint8_t nach_komma[1]; /*!< Variable zur Nachkommastellung Umrechnung */
	 uint32_t timeout = 500; /*!< Variable zur Aktualisierung der Messwerte alle 500ms */
	 char CANstring[8]; /*!< Variable (der Feldgröße 8) zum Senden der CAN-Nachricht (Inhalt: Temperatur des DS1621) */


/********************************************//**
 *  I2C
 ***********************************************/
	 __HAL_RCC_GPIOB_CLK_ENABLE();

 /*!< I2C GPIO Einstellung */
			GPIO_InitTypeDef G_I2C;
			G_I2C.Pin = GPIO_PIN_8|GPIO_PIN_9;     /*!< pinouts: PB8 -- SCL / PB9--- SDA */
			G_I2C.Mode = GPIO_MODE_AF_OD;
			G_I2C.Pull = GPIO_PULLUP;
			G_I2C.Speed = GPIO_SPEED_FREQ_HIGH;
			G_I2C.Alternate = GPIO_AF1_I2C1;
			HAL_GPIO_Init(GPIOB, &G_I2C);


__HAL_RCC_I2C1_CLK_ENABLE();     /*!< I2C-Clock */

			/*!< Initialisierung des I2C-Bus Kanals */
			I2C_HandleTypeDef hi2c1;
			hi2c1.Instance = I2C1;
			hi2c1.Init.Timing = 0x10805E89; /*! <Taktfrequenz SCL: 100kHz */
			hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
			hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
			hi2c1.Init.OwnAddress2 = 0;
			hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
			hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
			hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
			HAL_I2C_Init(&hi2c1);


	/*!< Senden der Commandobytes an DS1621 Temperatursensor */
	TxBuf[0] = 0x00;                                                                                                                             //*ONE-SHOT-Mode auf kontinuierliches Senden stellen
	HAL_I2C_Master_Transmit(&hi2c1, SLAVE_ADD_8BIT, TxBuf, 1, timeout); /*!<  Kommandobyte senden */

	HAL_Delay(50);                                                                                                                              //*Verzögerung zwischen den beidenKommandobytes

	TxBuf[0] = 0xEE;                                                                                                                             //*Kommandobyte zum Starten der Konversation zwischen DS1621 und µC
	HAL_I2C_Master_Transmit(&hi2c1, SLAVE_ADD_8BIT, TxBuf, 1, timeout); /*!<Kommandobyte senden */



	/*!< Einstellen des Blinkens der LED-Lampe */
	RCC->AHBENR |= (1<<17); /*< GPIOA Takt einschalten */
	GPIOA->MODER|= (0b01 << 10); /*< wird auf Ausgang geschalten */



	/*!< CAN-Einstellung */
	__HAL_RCC_GPIOA_CLK_ENABLE(); /*< GPIO Port Clock aktivieren (Port A) */

	/*!< CAN GPIO Einstellung */
	GPIO_InitTypeDef G_CAN;
	G_CAN.Pin = GPIO_PIN_11|GPIO_PIN_12;
	G_CAN.Mode = GPIO_MODE_AF_PP;
	G_CAN.Pull = GPIO_NOPULL;
	G_CAN.Speed = GPIO_SPEED_FREQ_HIGH;
	G_CAN.Alternate = GPIO_AF4_CAN;
	HAL_GPIO_Init(GPIOA, &G_CAN);


	__HAL_RCC_CAN1_CLK_ENABLE(); /*!< CAN-Clock aktivieren */

	/*!< Initialisierung des CAN-Bus Kanals */
	CAN_HandleTypeDef C;
	C.Instance = CAN;
	C.Init.Mode = CAN_MODE_NORMAL;
	C.Init.Prescaler= 24;
	C.Init.SJW = CAN_SJW_1TQ;
	C.Init.BS1 = CAN_BS1_11TQ;
	C.Init.BS2 = CAN_BS2_4TQ;
	C.Init.TTCM = DISABLE;
	C.Init.ABOM = DISABLE;
	C.Init.AWUM = DISABLE;
	C.Init.NART = DISABLE;
	C.Init.RFLM = DISABLE;
	C.Init.TXFP = DISABLE;
	HAL_CAN_Init(&C);


	/*!y CAN-Bus für Senden Einstellen */
	static CanTxMsgTypeDef TxMessage;
	C.pTxMsg = &TxMessage;

	C.pTxMsg->DLC =  sizeof(CANstring);
	C.pTxMsg->IDE = CAN_ID_STD;
	C.pTxMsg->RTR = CAN_RTR_DATA;
	C.pTxMsg->StdId = 0x33; //*Identifier 0x33



	/*!< I2C Read, Blink LED, CAN senden */
	while(1)
	{
	/*!< I2C Read */
	TxBuf[0] = 0xAA; //*Kommandobyte zum Auslesen der Temperatur
	HAL_I2C_Master_Transmit(&hi2c1, SLAVE_ADD_8BIT, TxBuf, 1, timeout); /*!< Kommandobyte senden */
	HAL_I2C_Master_Receive(&hi2c1, SLAVE_ADD_8BIT, RxBuf, 2, timeout);  /*!< Temperaturdaten empfangen */


	/*!< Nachkomma Temp */
	if (RxBuf[1] != 0)
					{
		nach_komma[1] = 5;
					}
	else
					{
		nach_komma[1] = 0;
					}


	/*!< LED Blinken */
	GPIOA->ODR |= (1<<5); //*an */
	HAL_Delay(750);
	GPIOA->ODR &= ~(1<<5); //*an */

	/*!< CAN senden */
	sprintf(CANstring, "%d.%d Gd\r\n", RxBuf[0], nach_komma[1]);

	for (int i=0;i<sizeof(CANstring);i++)
					{
		C.pTxMsg->Data[i] = CANstring[i];
					}

	HAL_CAN_Transmit(&C,timeout);
	HAL_Delay(500);}
 }


//* Error Handling, System Clock Config

 void Error_Handler(void)
 {
                 while(1);
 }


 void SystemClock_Config(void)
 {
                 RCC_OscInitTypeDef RCC_OscInitStruct;
                 RCC_ClkInitTypeDef RCC_ClkInitStruct;
                 RCC_PeriphCLKInitTypeDef PeriphClkInit;
                 /**Initializes the CPU, AHB and APB busses clocks*/
                 RCC_OscInitStruct.OscillatorType =
                 RCC_OSCILLATORTYPE_HSI14|RCC_OSCILLATORTYPE_HSI48
                 | RCC_OSCILLATORTYPE_LSE;
                 RCC_OscInitStruct.LSEState = RCC_LSE_ON;
                 RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
                 RCC_OscInitStruct.HSI14State = RCC_HSI14_ON;
                 RCC_OscInitStruct.HSI14CalibrationValue = 16;
                 RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
                 if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
                 {
                 Error_Handler();
                 }
                 /**Initializes the CPU, AHB and APB busses clocks */
                 RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                 |RCC_CLOCKTYPE_PCLK1;
                 RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI48;
                 RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
                 RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
                 if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
                 {
                 Error_Handler();
                 }
                 PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_I2C1
                 |RCC_PERIPHCLK_RTC;
                 PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
                 PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_SYSCLK;
                 PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
                 if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
                 {
                 Error_Handler();
                 }
                 /**Configure LSE Drive Capability */
                 __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);
                 HAL_RCC_MCOConfig(RCC_MCO, RCC_MCO1SOURCE_SYSCLK, RCC_MCODIV_1);
                 /**Configure the Systick interrupt time */
                 HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);
                 /**Configure the Systick */
                 HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
                 /* SysTick_IRQn interrupt configuration */
                 HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
 }
