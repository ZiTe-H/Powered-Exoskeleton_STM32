/**
 ******************************************************************************
 * @file    PWM_Functions.cpp
 * @author  Huang Tzu-Fu
 *          National Formosa University
 *          Department of Electronic Engineering
 *          Intelligent Robot System Laboratory
 * @version V1.0.0
 * @date    15-October-2019
 * @brief   PWM functions program
 ******************************************************************************
 * @attention
 *
 * None
 *
 ******************************************************************************
 */

#include "PWM_Functions.hpp"

void PWM_SetFrequency(TIM_TypeDef *TIMx, uint16_t NewFrequency)
{
  /**
   *  TIM_Period = ((System_Core_Frequency / TIM_Prescaler) / PWM_Frequency) - 1
   *
   *  The Maximum of System_Core_Frequency = 72MHz (STM32F103RB)
   */

  extern RCC_ClocksTypeDef RCC_Clocks;
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

  TIM_TimeBaseStructure.TIM_Prescaler = 10;
  TIM_TimeBaseStructure.TIM_Period = ((RCC_Clocks.SYSCLK_Frequency / (TIM_TimeBaseStructure.TIM_Prescaler * 10)) / NewFrequency) - 1;

  TIM_TimeBaseInit(TIMx, &TIM_TimeBaseStructure);
}
/**
 * @brief  Set PWM Duty Cycle %
 * @param  TIMx:    TIMx, where x can be  1, 2, 3, 4, 5, 8 to select
 *                 the TIM peripheral.
 * @param  Channel: The channel of timer.
 *                  This parameter can be one of the following values:
 *                  CH1, CH2, CH3 or CH4.
 * @param  DutyCycle: The Duty Cycle of PWM in %.
 * @retval None
 */
void PWM_SetDutyCycle(TIM_TypeDef *TIMx, PWM_TimerChannelTypeDef Channel,
                      uint8_t DutyCycle)
{
  /**
   *  TIM_Pulse = (PWM-Duty Cycle % * TIM_Period)/100%
   *
   *  TIM_Pulse = CCRx
   *  TIM_Period = ARR
   */

  switch (Channel)
  {
  case CH1:
    TIM_SetCompare1(TIMx, (DutyCycle * (TIMx->ARR)) / 100);
    break;
  case CH2:
    TIM_SetCompare2(TIMx, (DutyCycle * (TIMx->ARR)) / 100);
    break;
  case CH3:
    TIM_SetCompare3(TIMx, (DutyCycle * (TIMx->ARR)) / 100);
    break;
  case CH4:
    TIM_SetCompare4(TIMx, (DutyCycle * (TIMx->ARR)) / 100);
    break;
  default:
    break;
  }
}

uint16_t PWM_GetDutyCycle(TIM_TypeDef *TIMx, PWM_TimerChannelTypeDef Channel)
{
  /**
   *  PWM-Duty Cycle % = (TIM_Pulse / TIM_Period)*100%
   *
   *  TIM_Pulse = CCRx
   *  TIM_Period = ARR
   */

  switch (Channel)
  {
  case CH1:
    return ((TIMx->CCR1) * 100) / (TIMx->ARR);
    break;
  case CH2:
    return ((TIMx->CCR2) * 100) / (TIMx->ARR);
    break;
  case CH3:
    return ((TIMx->CCR3) * 100) / (TIMx->ARR);
    break;
  case CH4:
    return ((TIMx->CCR4) * 100) / (TIMx->ARR);
    break;
  default:
    break;
  }
}

PWM::PWM(void)
{
  TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  TIM_OCStructInit(&TIM_OCInitStructure);
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Disable;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

  GPIO_PWM.Mode = GPIO_Mode_AF_PP;
  GPIO_PWM.Speed = GPIO_Speed_50MHz;
}

void PWM::Init(void)
{
  GPIO_PWM.PortPin = PortPin;
  GPIO_PWM.Init();
  this->setup();
}

void PWM::Enable(void)
{
  if (TIM_OCInitStructure.TIM_OutputState != TIM_OutputState_Enable)
  {
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    this->setup();
  }

  // TIMx peripheral Preload register on CCRx
  switch (Channel)
  {
  case CH1:
    TIM_OC1PreloadConfig(Timer, TIM_OCPreload_Enable);
    break;
  case CH2:
    TIM_OC2PreloadConfig(Timer, TIM_OCPreload_Enable);
    break;
  case CH3:
    TIM_OC3PreloadConfig(Timer, TIM_OCPreload_Enable);
    break;
  case CH4:
    TIM_OC4PreloadConfig(Timer, TIM_OCPreload_Enable);
    break;
  default:
    break;
  }
  TIM_ARRPreloadConfig(Timer, ENABLE); // TIMx peripheral Preload register on ARR
  TIM_Cmd(Timer, ENABLE);              // The specified TIM peripheral
}

void PWM::Disable(void)
{
  if (TIM_OCInitStructure.TIM_OutputState != TIM_OutputState_Disable)
  {
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Disable;
    this->setup();
  }

  // TIMx peripheral Preload register on CCRx
  switch (Channel)
  {
  case CH1:
    TIM_OC1PreloadConfig(Timer, TIM_OCPreload_Disable);
    break;
  case CH2:
    TIM_OC2PreloadConfig(Timer, TIM_OCPreload_Disable);
    break;
  case CH3:
    TIM_OC3PreloadConfig(Timer, TIM_OCPreload_Disable);
    break;
  case CH4:
    TIM_OC4PreloadConfig(Timer, TIM_OCPreload_Disable);
    break;
  default:
    break;
  }
  TIM_ARRPreloadConfig(Timer, DISABLE); // TIMx peripheral Preload register on ARR
  TIM_Cmd(Timer, DISABLE);              // The specified TIM peripheral
}

// FIXME Value of frequency error.
void PWM::setFrequency(uint16_t NewFrequency)
{
  /**
   *  TIM_Period = ((System_Frequency / TIM_Prescaler) / PWM_Frequency) - 1
   *
   *  The Maximum of System_Frequency = 72MHz (STM32F103RB)
   */

  extern RCC_ClocksTypeDef RCC_Clocks;

  TIM_TimeBaseStructure.TIM_Prescaler = 10; // !! or 100
  TIM_TimeBaseStructure.TIM_Period = ((RCC_Clocks.SYSCLK_Frequency / TIM_TimeBaseStructure.TIM_Prescaler) / NewFrequency) - 1;

  this->setup();
}

void PWM::setDutyCycle(uint16_t NewDutyCycle)
{
  if (TIM_OCInitStructure.TIM_Pulse != this->convertDutyCycleToPulse(NewDutyCycle))
  {
    TIM_OCInitStructure.TIM_Pulse = this->convertDutyCycleToPulse(NewDutyCycle);
    this->setup();
  }
}
uint16_t PWM::getFrequency(void)
{
  /**
   * PWM_Frequency = (System_Frequency / TIM_Prescaler) / (TIM_Period + 1)
   *
   * The Maximum of System_Frequency = 72MHz (STM32F103RB)
   */

  extern RCC_ClocksTypeDef RCC_Clocks;

  return (RCC_Clocks.SYSCLK_Frequency / TIM_TimeBaseStructure.TIM_Prescaler) / (TIM_TimeBaseStructure.TIM_Period + 1);
}

uint16_t PWM::getDutyCycle(void)
{
  /**
   *  PWM_Duty Cycle % = (TIM_Pulse * 100%) / TIM_Period
   *
   *  TIM_Pulse = CCRx
   *  TIM_Period = ARR
   */

  switch (Channel)
  {
  case CH1:
    return ((Timer->CCR1) * 100.0) / (Timer->ARR);
    break;
  case CH2:
    return ((Timer->CCR2) * 100.0) / (Timer->ARR);
    break;
  case CH3:
    return ((Timer->CCR3) * 100.0) / (Timer->ARR);
    break;
  case CH4:
    return ((Timer->CCR4) * 100.0) / (Timer->ARR);
    break;
  default:
    break;
  }
}

void PWM::setup(void)
{
  TIM_TimeBaseInit(Timer, &TIM_TimeBaseStructure);
  switch (Channel)
  {
  case CH1:
    TIM_OC1Init(Timer, &TIM_OCInitStructure);
    break;
  case CH2:
    TIM_OC2Init(Timer, &TIM_OCInitStructure);
    break;
  case CH3:
    TIM_OC3Init(Timer, &TIM_OCInitStructure);
    break;
  case CH4:
    TIM_OC4Init(Timer, &TIM_OCInitStructure);
    break;
  default:
    break;
  }
}

uint16_t PWM::convertDutyCycleToPulse(uint16_t DutyCycle)
{
  /* TIM_Pulse = (PWM_Duty Cycle % * TIM_Period) / 100% */
  return (DutyCycle * (TIM_TimeBaseStructure.TIM_Period) / 100.0);
}

/********************************END OF FILE***********************************/
