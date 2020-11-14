/**
 ******************************************************************************
 * @file    USART_Functions.h
 * @author  ZiTe (honmonoh@gmail.com)
 * @version V1.0
 * @date    14-November-2020
 * @brief   Header for USART_Functions.c module
 ******************************************************************************
 * @attention
 *
 * None
 *
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USART_FUNCTIONS_H
#define __USART_FUNCTIONS_H

#include <string>
#include "convert.hpp"

#ifdef __cplusplus
extern "C"
{
#endif

#include "stm32f10x.h"

#ifdef __cplusplus
} /* extern "C" { */
#endif
/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void USART_Send(USART_TypeDef *USARTx, uint8_t *Data);
void USART_Send(USART_TypeDef *USARTx, std::string Data);

#endif /* __USART_FUNCTIONS_H */

/********************************END OF FILE***********************************/
