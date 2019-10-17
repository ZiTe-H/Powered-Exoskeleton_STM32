
/**
 ******************************************************************************
 * @file    	GPIO_mapping.h
 * @author  	Huang Tzu-Fu
 * 				National Formosa University
 * 				Department of Electronic Engineering
 * 				Intelligent Robot System Laboratory
 * @version 	V1.0.0
 * @date    	08-October-2019
 * @brief   	Header for GPIO_Function.c module for STM32F103RB
 ******************************************************************************
 * @attention
 *
 * This file is used ONLY for STM32F103RB(STM32 Nucleo-64 board).
 *
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GPIO_MAPPING_H
#define __GPIO_MAPPING_H

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* -----Pin Mode and Speed----- */
//#define In		(0)
//#define Out		(1)
//
//#define GPPP	(0)
//#define GPOD	(1)
//#define AFPP	(2)
//#define AFOD	(3)
//
//#define An		(0)
//#define Fl		(1)
//#define PD		(2)
//#define PU		(3)
//
//#define s2M		(0)
//#define s10M	(1)
//#define s50M	(2)

/* STM32 Pin(Morpho) */
// Port-A
#define PA0		(0)	// WKUP/USART2_CTS/ADC12_IN0/TIM2_CH1_ETR;Arduino:A0
#define PA1		(1)	// USART2_RTS/ADC12_IN1/TIM2_CH2;Arduino:A1
#define PA2		(2)	// USART2_TX/ADC12_IN2/TIM2_CH3;Arduino:D1
#define PA3		(3)	// USART2_RX/ADC12_IN3/TIM2_CH4
#define PA4		(4)	// SPI1_NSS/USART2_CK/ADC12_IN4
#define PA5		(5)	// SPI1_SCK/ADC12_IN5
#define PA6		(6)	// SPI1_MISO/ADC12_IN6/TIM3_CH1;Remap:TIM1_BKIN
#define PA7		(7)	// SPI1_MOSI/ADC12_IN7/TIM3_CH2;Remap:TIM1_CH1N
#define PA8		(8) // USART1_CK/TIM1_CH1/MCO
#define PA9		(9) // USART1_TX/TIM1_CH2
#define PA10	(10)// USART1_RX/TIM1_CH3
#define PA11	(11)// USART1_CTS/CANRX/USBDM/TIM1_CH4
#define PA12	(12)// USART1_RTS/CANTX/USBDP/TIM1_ETR
#define PA13	(13)// MA:JTMS/SWDIO;Remap:PA13
#define PA14	(14)// MA:JTCK/SWCLK;Remap:PA14
#define PA15	(15)// MA:JTDI;Remap:TIM2_CH1_ETR/ PA15/SPI1_NSS

// Port-B
#define PB0		(16)//
#define PB1		(17)//
#define PB2		(18)//
#define PB3		(19)//
#define PB4		(20)//
#define PB5		(21)//
#define PB6		(22)//
#define PB7		(23)//
#define PB8		(24)//
#define PB9		(25)//
#define PB10	(26)//
#define PB11	(27)//
#define PB12	(28)//
#define PB13	(29)//
#define PB14	(30)//
#define PB15	(31)//

// Port-C
#define PC0		(32)//
#define PC1		(33)//
#define PC2		(34)//
#define PC3		(35)//
#define PC4		(36)//
#define PC5		(37)//
#define PC6		(38)//
#define PC7		(39)//
#define PC8		(40)//
#define PC9		(41)//
#define PC10	(42)//
#define PC11	(43)//
#define PC12	(44)//
#define PC13	(45)//
#define PC14	(46)//
#define PC15	(47)//

// Port-D
#define PD0		(48)//
#define PD1		(49)//
#define PD2		(50)//
#define PD3		(51)//
#define PD4		(52)//
#define PD5		(53)//
#define PD6		(54)//
#define PD7		(55)//
#define PD8		(56)//
#define PD9		(57)//
#define PD10	(58)//
#define PD11	(59)//
#define PD12	(60)//
#define PD13	(61)//
#define PD14	(62)//
#define PD15	(63)//

// Port-E
#define PE0		(64)//
#define PE1		(65)//
#define PE2		(66)//
#define PE3		(67)//
#define PE4		(68)//
#define PE5		(69)//
#define PE6		(70)//
#define PE7		(71)//
#define PE8		(72)//
#define PE9		(73)//
#define PE10	(74)//
#define PE11	(75)//
#define PE12	(76)//
#define PE13	(77)//
#define PE14	(78)//
#define PE15	(79)//

/* Arduino Pin */
// Analog(CN8)
// Digital(CN5)
// Digital(CN9)

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */



#endif /* __GPIO_MAPPING_H */

/********************************END OF FILE***********************************/
