/**
 ******************************************************************************
 * @file    joint.hpp
 * @author  ZiTe (honmonoh@gmail.com)
 * @version V1.0
 * @date    14-November-2020
 * @brief   Header for joint.cpp module
 ******************************************************************************
 * @attention
 *
 * None
 *
 ******************************************************************************
 */

/* Define to prevent recursive inclusion */
#ifndef __JOINT_HPP
#define __JOINT_HPP

#include "EC90fl_Motor_Functions.hpp"
#include "ADC_Functions.hpp"
#include "USART_Functions.hpp"
#include "convert.hpp"

#define Joint_Motor_Speed (50)

/**
 * @brief The class of joint.
 */
class Joint
{
public:
  /* The dircetion type of joint. */
  typedef enum
  {
    Extension = 1,
    Flexion = 2
  } MotionDirectionTypeDef;

  typedef enum
  {
    NoInMotion = 0,
    Extensioning = 1,
    Flexioning = 2,
    WaitStop = 3
  } MotionStateTypeDef;

  /* The software limit state type of joint. */
  typedef enum
  {
    Unlimited = 0,
    FullExtension = 1,
    FullFlexion = 2
  } SoftwareLimitStateTypeDef;

  /* The state struct of joint. */
  typedef struct
  {
    // MotionDirectionTypeDef Direction;
    MotionStateTypeDef Motion;
    SoftwareLimitStateTypeDef SoftwareLimit;
    bool Ready;

    uint16_t AnglePOTValue;
    uint16_t FrontFSRValue;
    uint16_t BackFSRValue;
  } StateTypeDef;

  /* PWM timer used for motor speed control. */
  TIM_TypeDef *Timer_SpeedPWM;

  /* PWM timer-chammel used for motor speed control. */
  PWM_TimerChannelTypeDef Channel_SpeedPWM;

  /* PWM output pin used for motor speed control. */
  GPIO_PortPinTypeDef PortPin_SpeedPWM;

  /* Output pin used for motor functino state control. */
  GPIO_PortPinTypeDef PortPin_FunctionState;

  /* Output pin used for motor direction control. */
  GPIO_PortPinTypeDef PortPin_Direction;

  /* Input pin used for read motor ready state. */
  GPIO_PortPinTypeDef PortPin_ReadyState;

  /* Analog input pin used for read motor RPM. */
  GPIO_PortPinTypeDef PortPin_RPM;

  GPIO_PortPinTypeDef PortPin_AnglePOT;
  ADC_TypeDef *ADCx_AnglePOT;
  uint8_t ADC_Channel_AnglePOT;

  GPIO_PortPinTypeDef PortPin_FrontFSR;
  ADC_TypeDef *ADCx_FrontFSR;
  uint8_t ADC_Channel_FrontFSR;

  GPIO_PortPinTypeDef PortPin_BackFSR;
  ADC_TypeDef *ADCx_BackFSR;
  uint8_t ADC_Channel_BackFSR;

  uint16_t FullExtensionPOTValue;
  uint16_t FullFlexionPOTValue;

  uint16_t ExtensionFSRStartThreshold;
  uint16_t FlexionFSRStartThreshold;

  uint16_t ExtensionFSRStopThreshold;
  uint16_t FlexionFSRStopThreshold;

  bool  reverse;

  Joint(void);
  virtual void Init(void);

  bool ExtensionStartTriggered(void);
  bool FlexionStartTriggered(void);
  bool ExtensionStopTriggered(void);
  bool FlexionStopTriggered(void);

  virtual void MotionExtensionStart(void);
  virtual void MotionFlexionStart(void);

  virtual SoftwareLimitStateTypeDef MotionExtensionStop(void);
  virtual SoftwareLimitStateTypeDef MotionFlexionStop(void);

  uint16_t getAnglePOTValue(void);
  uint16_t getFrontFSRValue(void);
  uint16_t getBackFSRValue(void);
  SoftwareLimitStateTypeDef getLimitState(void);

  void MotionWaitStop(void);

  virtual void MotionHandler(void);
  virtual SoftwareLimitStateTypeDef MotionStop(void);

  void SendInfo(void);

  void getState(StateTypeDef *state);

protected:
  EC90Motor Motor;
  ADC AnglePOT;
  ADC FrontFSR;
  ADC BackFSR;
  MotionStateTypeDef MotionState;
  // MotionDirectionTypeDef MotionDirection;

  bool StartExtensionIsTriggered(void);
  bool StartFlexionIsTriggered(void);
  bool StopExtensionIsTriggered(void);
  bool StopFlexionIsTriggered(void);

  float Convert_ADCValueToAngle(uint16_t ADCValue);
  uint8_t Convert_DegPerSecToPWMDutyCycle(float DegPerSec);
};

class JointWithoutHallSensor : public Joint
{
public:
  GPIO_PortPinTypeDef PortPin_VirtualHall1;
  GPIO_PortPinTypeDef PortPin_VirtualHall2;
  GPIO_PortPinTypeDef PortPin_VirtualHall3;

  JointWithoutHallSensor(void);
  void Init(void);

  void MotionExtensionStart(void);
  void MotionFlexionStart(void);

  SoftwareLimitStateTypeDef MotionExtensionStop(void);
  SoftwareLimitStateTypeDef MotionFlexionStop(void);

  void MotionHandler(void);
  SoftwareLimitStateTypeDef MotionStop(void);

  void VirtualHallHandler(void);

private:
  uint8_t VirtualHallStep;
  MotionDirectionTypeDef Direction;

  GPIO VirtualHall1;
  GPIO VirtualHall2;
  GPIO VirtualHall3;
};

/**
 * @brief Set absolute angle of joint.
 * @param TargetAngle Target angle(degree).
 */
void Joint_SetAbsoluteAngle(float TargetAngle);

/**
 * @brief Get absolute angle of joint.
 * @return Angle of joint(degree). 
 */
float Joint_GetAbsoluteAngle(void);

/**
 * @brief Get limit state of joint.
 * @return Limit state of joint. 
 */
// LimitStateTypeDef Joint_GetLimitState(void);

#endif /* __JOINT_HPP */

/*********************************END OF FILE**********************************/
