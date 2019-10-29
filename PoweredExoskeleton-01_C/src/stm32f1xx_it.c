/**
  ******************************************************************************
  * @file    stm32f1xx_it.c 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    11-February-2014
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_it.h"
#include "stm32f10x.h"

/** @addtogroup IO_Toggle
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern __IO uint8_t BlinkSpeed;
uint16_t nInst = 0;
uint8_t selMotor = 0xFF;	// The motor which be selected

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M Processor Exceptions Handlers                          */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
  TimingDelay_Decrement();
}

/******************************************************************************/
/*                 STM32F1xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_md.s).                                            */
/******************************************************************************/
/**
  * @brief  This function handles EXTI15_10_IRQHandler Handler.
  * @param  None
  * @retval None
  */
void EXTI15_10_IRQHandler(void)
{
  if (EXTI_GetITStatus(USER_BUTTON_EXTI_LINE) != RESET)
  {		
    if(BlinkSpeed == 1)
    {
      BlinkSpeed = 0;
    }
    else
    {
      BlinkSpeed ++;
    }
    /* Clear the EXTI line pending bit */
    EXTI_ClearITPendingBit(USER_BUTTON_EXTI_LINE);
  }	
}

/**
  * @brief  This function handles USART2_IRQHandler Handler.
  * @param  None
  * @retval None
  */
void USART2_IRQHandler(void)
{
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) // 注意不是USART_FLAG_RXNE
	{
		uint16_t USART_ReceivData = 0xF0;
//		uint8_t InstTex[] = "";

		USART_ReceivData = USART_ReceiveData(USART2);

//		USART_Send(USART2, USART_ReceivData);
//		USART_Send(USART2, "STM32:");

		if(nInst == 0)
		{
			USART_Send(USART2, "STM32:\n");

			if(USART_ReceivData == 0xE0)				// System stop
			{
				USART_Send(USART2, "[System]Stop.\n");
			}
			else if(USART_ReceivData == 0xE1)			// System reset
			{
				USART_Send(USART2, "[System]Reset.\n");
			}
			else if((USART_ReceivData & 0xE0) == 0x20)	// Instruction start
			{
				while(selMotor != ((USART_ReceivData & 0x18) >> 3))
					selMotor = ((USART_ReceivData & 0x18) >> 3);// Select motor

				while(nInst != (USART_ReceivData & 0x07))
					nInst = (USART_ReceivData & 0x07);	// Set instruction number
			}
			else if(USART_ReceivData == 0xF0)
				/* Null */;
			else	// Unknown instruction
			{
				USART_Send(USART2, "[Error]Unknown instruction.\n");
			}
		}
		else	// nInst != 0
		{
			--nInst;

			if(selMotor == 0x00)
				USART_Send(USART2, "Motor-0\n");
			else if(selMotor == 0x01)
				USART_Send(USART2, "Motor-1\n");
			else
				USART_Send(USART2, "[Error]No motors selected");

//			InstTex = "Motor" + (selMotor) + "\n";
//			USART_Send(USART2, InstTex);

			// Set motor speed
			if(((USART_ReceivData & 0x80) >> 7) == 0x01) 		// 1xxx xxxx(b)
			{
//				MotorCtrl(selMotor, 2, 2, 127);
				USART_Send(USART2, "[Motor]Set speed.\n");
			}
			else												// 0xxx xxxx(b)
			{
				/* Motor status */
				// Enable
				if(((USART_ReceivData & 0x60) >> 5) == 0x01)	// x01x xxxx(b)
				{
					MotorCtrl(selMotor, 1, 2, 127);
					USART_Send(USART2, "[Motor]Enable.\n");
				}
				// Disable
				else if(((USART_ReceivData & 0x60) >> 5) == 0x00)// x00x xxxx(b)
				{
					MotorCtrl(selMotor, 0, 2, 127);
					USART_Send(USART2, "[Motor]Disable.\n");
				}
				// Keep
				else /* Null */;

				/* Motor direction */
				// CCW
				if(((USART_ReceivData & 0x18) >> 3) == 0x01)	// xxx0 1xxx(b)
				{
					MotorCtrl(selMotor, 2, 1, 127);
					USART_Send(USART2, "[Motor]Direction:CCW.\n");
				}
				// CW
				else if(((USART_ReceivData & 0x18) >> 3) == 0x00)// xxx0 0xxx(b)
				{
					MotorCtrl(selMotor, 2, 0, 127);
					USART_Send(USART2, "[Motor]Direction:CW.\n");
				}
				// Keep
				else /* Null */;
			}

			// End of instruction
			if(nInst == 0)
			{
				selMotor = 0xFF;	// Deselect motor
				USART_Send(USART2, "[Motor]Control done.\n");
			}
		}
		/* NO need to clears the USARTx's interrupt pending bits */
		/* USART_ClearITPendingBit(USART2,USART_IT_RXNE); */
	}
}

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
