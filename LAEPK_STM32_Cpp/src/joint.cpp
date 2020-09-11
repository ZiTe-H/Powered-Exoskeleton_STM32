/**
 ******************************************************************************
 * @file    joint.cpp
 * @author  Huang, Tzu-Fu
 *          National Formosa University
 *          Department of Electronic Engineering
 *          Intelligent Robot System Laboratory
 * @version V1.0.0
 * @date    31-August-2020
 * @brief   Program body of L.A.E.P.K. of joint.
 ******************************************************************************
 * @attention
 *
 * None
 *
 ******************************************************************************
 */

#include "joint.hpp"

/* Uncomment one of the line below to select mode. */
#define MODE_FOLLOWING
// #define MODE_START_STOP_TRIGGER
// #define MODE_CONTINUOUS_START_STOP_TRIGGER

#if !defined(MODE_FOLLOWING) && !defined(MODE_START_STOP_TRIGGER) && !defined(MODE_CONTINUOUS_START_STOP_TRIGGER)
#error No joint-mode selected.
#endif

Joint::Joint(void)
{
  MotionState = NoInMotion;
}

void Joint::Init(void)
{
  Motor.PortPin_SpeedPWM = PortPin_SpeedPWM;
  Motor.Timer_SpeedPWM = Timer_SpeedPWM;
  Motor.Channel_SpeedPWM = Channel_SpeedPWM;
  Motor.PortPin_FunctionState = PortPin_FunctionState;
  Motor.PortPin_Direction = PortPin_Direction;
  Motor.PortPin_ReadyState = PortPin_ReadyState;
  Motor.PortPin_RPM = PortPin_RPM;
  Motor.Init();
  Motor.setSpeed(15);
  Motor.setDirection(EC90Motor::CW);
  Motor.Disable();

  AnglePOT.PortPin = PortPin_AnglePOT;
  AnglePOT.ADCx = ADCx_AnglePOT;
  AnglePOT.ADC_Channel = ADC_Channel_AnglePOT;
  AnglePOT.Init();
  AnglePOT.Enable();

  FrontFSR.PortPin = PortPin_FrontFSR;
  FrontFSR.ADCx = ADCx_FrontFSR;
  FrontFSR.ADC_Channel = ADC_Channel_FrontFSR;
  FrontFSR.Init();
  FrontFSR.Enable();

  BackFSR.PortPin = PortPin_BackFSR;
  BackFSR.ADCx = ADCx_BackFSR;
  BackFSR.ADC_Channel = ADC_Channel_BackFSR;
  BackFSR.Init();
  BackFSR.Enable();
}

bool Joint::ExtensionStartTriggered(void)
{
  return ((FrontFSR.getValue() > ExtensionFSRStartThreshold) && (getLimitState() != FullExtension));
}

bool Joint::FlexionStartTriggered(void)
{
  return ((BackFSR.getValue() > FlexionFSRStartThreshold) && (getLimitState() != FullFlexion));
}

bool Joint::ExtensionStopTriggered(void)
{
  return ((BackFSR.getValue() > ExtensionFSRStopThreshold) || (getLimitState() == FullExtension));
}

bool Joint::FlexionStopTriggered(void)
{
  return ((FrontFSR.getValue() > FlexionFSRStopThreshold) || (getLimitState() == FullFlexion));
}

void Joint::MotionExtensionStart(void)
{
  MotionState = Extensioning;
  // USART_Send(USART2, "Ex-Start\r\n");

  Motor.setDirection(EC90Motor::CCW);
  Motor.setSpeed(15);
  Motor.Enable();
}

void Joint::MotionFlexionStart(void)
{
  MotionState = Flexioning;
  // USART_Send(USART2, "Fl-Start\r\n");

  Motor.setDirection(EC90Motor::CW);
  Motor.setSpeed(15);
  Motor.Enable();
}

Joint::SoftwareLimitStateTypeDef Joint::MotionExtensionStop(void)
{
  Motor.Disable();
  Motor.setSpeed(0);

#if defined(MODE_FOLLOWING) || defined(MODE_CONTINUOUS_START_STOP_TRIGGER)
  MotionState = NoInMotion;
#elif defined(MODE_START_STOP_TRIGGER)
  MotionState = WaitStop;
#endif
  // USART_Send(USART2, "Ex-Stop\r\n");

  return getLimitState();
}

Joint::SoftwareLimitStateTypeDef Joint::MotionFlexionStop(void)
{
  Motor.Disable();
  Motor.setSpeed(0);

#if defined(MODE_FOLLOWING) || defined(MODE_CONTINUOUS_START_STOP_TRIGGER)
  MotionState = NoInMotion;
#elif defined(MODE_START_STOP_TRIGGER)
  MotionState = WaitStop;
#endif
  // USART_Send(USART2, "Fl-Stop\r\n");

  return getLimitState();
}

void Joint::MotionWaitStop(void)
{
  if ((FrontFSR.getValue() < (ExtensionFSRStartThreshold * 0.8)) &&
      (BackFSR.getValue() < (FlexionFSRStartThreshold * 0.8)))
  {
    MotionState = NoInMotion;
  }
}

uint16_t Joint::getAnglePOTValue(void)
{
  return AnglePOT.getValue();
}

uint16_t Joint::getFrontFSRValue(void)
{
  return FrontFSR.getValue();
}

uint16_t Joint::getBackFSRValue(void)
{
  return BackFSR.getValue();
}

void Joint::MotionHandler(void)
{
#if defined(MODE_FOLLOWING)
  switch (this->MotionState)
  {
  case Extensioning:
    if (this->ExtensionStartTriggered() == false)
    {
      this->MotionExtensionStop();
      USART_Send(USART2, "R: Ex-Stop\r\n");
    }
    break;

  case Flexioning:
    if (this->FlexionStartTriggered() == false)
    {
      this->MotionFlexionStop();
      USART_Send(USART2, "R: Fl-Stop\r\n");
    }
    break;

  case NoInMotion:
  default:
    if (this->ExtensionStartTriggered())
    {
      this->MotionExtensionStart();
      USART_Send(USART2, "R: Ex-Start\r\n");
    }
    else if (this->FlexionStartTriggered())
    {
      this->MotionFlexionStart();
      USART_Send(USART2, "R: Fl-Start\r\n");
    }
    break;
  }
#elif defined(MODE_CONTINUOUS_START_STOP_TRIGGER) || defined(MODE_START_STOP_TRIGGER)
  switch (this->MotionState)
  {
  case NoInMotion:
    if (this->ExtensionStartTriggered())
    {
      this->MotionExtensionStart();
      USART_Send(USART2, "R: Ex-Start\r\n");
    }
    else if (this->FlexionStartTriggered())
    {
      this->MotionFlexionStart();
      USART_Send(USART2, "R: Fl-Start\r\n");
    }
    break;

  case Extensioning:
    if (this->ExtensionStopTriggered())
    {
      this->MotionExtensionStop();
      USART_Send(USART2, "R: Ex-Stop\r\n");
    }
    break;

  case Flexioning:
    if (this->FlexionStopTriggered())
    {
      this->MotionFlexionStop();
      USART_Send(USART2, "R: Fl-Stop\r\n");
    }
    break;

  case WaitStop:
  default:
    this->MotionWaitStop();
    break;
  }
#endif
}

Joint::SoftwareLimitStateTypeDef Joint::MotionStop(void)
{
  Motor.Disable();
  Motor.setSpeed(0);
}

Joint::SoftwareLimitStateTypeDef Joint::getLimitState(void)
{
  uint16_t value = AnglePOT.getValue();

  if (value < FullExtensionPOTValue)
    return FullExtension;
  else if (value > FullFlexionPOTValue)
    return FullFlexion;
  else
    return Unlimited;
}

void Joint::SendInfo(void)
{
  USART_Send(USART2, "  F-FSR: ");
  USART_Send(USART2, Convert::ToString(getFrontFSRValue()));
  USART_Send(USART2, "\n");

  USART_Send(USART2, "  B-FSR: ");
  USART_Send(USART2, Convert::ToString(getBackFSRValue()));
  USART_Send(USART2, "\n");

  USART_Send(USART2, "  A-POT: ");
  USART_Send(USART2, Convert::ToString(getAnglePOTValue()));
  USART_Send(USART2, "\n");

  USART_Send(USART2, "  Limit: ");
  switch (getLimitState())
  {
  case Joint::FullExtension:
    USART_Send(USART2, "Full-Ex");
    break;

  case Joint::FullFlexion:
    USART_Send(USART2, "Full-Fl");
    break;

  case Joint::Unlimited:
  default:
    USART_Send(USART2, "Unlimited");
    break;
  }
  USART_Send(USART2, "\n");
}

bool Joint::StartExtensionIsTriggered(void)
{
  /*
   * If Start-Extension is triggered,
   * that is if Front FSR ADC Value > Extension Start Threshold,
   * then return TRUE, else return FALSE.
   */
  return (FrontFSR.getValue() > ExtensionFSRStartThreshold);
}

bool Joint::StartFlexionIsTriggered(void)
{
  /*
   * If Start-Flexion is triggered,
   * that is if Back FSR ADC Value > Flexion Start Threshold,
   * then return TRUE, else return FALSE.
   */
  return (BackFSR.getValue() > FlexionFSRStartThreshold);
}

bool Joint::StopExtensionIsTriggered(void)
{
  /*
   * If Stop-Extension is triggered,
   * that is if Back FSR ADC Value > Extension Stop Threshold,
   * then return TRUE, else return FALSE.
   */
  return (BackFSR.getValue() > ExtensionFSRStopThreshold);
}

bool Joint::StopFlexionIsTriggered(void)
{
  /*
   * If Stop-Flexion is triggered,
   *
   *
   * that is if Front FSR ADC Value > Flexion Stop Threshold,
   * then return TRUE, else return FALSE.
   */
  return (FrontFSR.getValue() > FlexionFSRStopThreshold);
}

float Joint::Convert_ADCValueToAngle(uint16_t ADCValue)
{
  /**
   * The POT_Gear is 20 teeth, Joint_Gear is 250 teeth.
   * The Gear Ratio = 250/20 = 12.5:1
   *
   * ADC resolution is 12bits = 4096 quantization levels (0 ~ 4095).
   * POT Maximum degree = 3600 degrees = 10 turn.
   *
   * The POT_Angle per ADC_Quantization levels = 3600/4096
   *                                           = 225/256
   *                                           = 0.878 906 25
   *                                           (unit in deg/ADC_Lv)
   *
   * That,
   * The Joint_Angle per ADC_Quantization levels = POT_deg per ADC_lv / 12.5
   *                                             = (3600/4096)/12.5
   *                                             = 9/128
   *                                             = 0.070 312 5
   *                                             (unit in deg/ADC_Lv)
   *
   * And,
   * Knee joint full extension is defined as -5 deg.
   * So,
   * Joint_Angle = [(ADC_Value - Full_Extension_ADC_Value)*(9/128)]-5
   */

  return ((float)((ADCValue - FullExtensionPOTValue) * (0.0703125) - 5));
}

uint8_t Joint::Convert_DegPerSecToPWMDutyCycle(float DegPerSec)
{
  /**
   * The nominal speed of Maxon EC90 flat brushless motor(515458) is 2720rpm.
   * The gear ratio of Harmoinc Drive SHD-25-100-2SH is 100:1.
   *
   * So, the max speed of joint is 2720/100 = 27.2rpm(ideal).
   *
   * Mode of motor controller is open loop speed control,
   * 0...95 % PWM depending on the «Set value speed» input voltage.
   *
   * Minimum speed: Nmin (rpm)
   * Maximum speed: Nmax (rpm)
   * Set respectively speed: N (rpm)
   * Set value voltage: Vset (V)
   * Vset = {[(N-Nmin)/(Nmax-Nmin)]*4.9}+0.1
   *
   * 1 Degree Per Second = 1/6 rpm
   */

  /* gear ration * RespectivelySpeed in rpm */
  float RespectivelySpeed = 100 * (DegPerSec / 6.0);

  /* Vset = {[(N-Nmin)/(Nmax-Nmin)]*4.9}+0.1 */
  float Vset = ((RespectivelySpeed / 2720.0) * 4.9) + 0.1;

  /* PWM Dutu Cucly = (Vset/Vp)*100% */
  float PWMDutyCycle = (Vset / 3.3) * 100;

  if (PWMDutyCycle >= 100)
    return ((uint8_t)100);
  else
    return ((uint8_t)PWMDutyCycle);
}

JointWithoutHallSensor::JointWithoutHallSensor(void)
{
  VirtualHall1.Mode = GPIO_Mode_Out_PP;
  VirtualHall1.Speed = GPIO_Speed_50MHz;

  VirtualHall2.Mode = GPIO_Mode_Out_PP;
  VirtualHall2.Speed = GPIO_Speed_50MHz;

  VirtualHall3.Mode = GPIO_Mode_Out_PP;
  VirtualHall3.Speed = GPIO_Speed_50MHz;

  VirtualHallStep = 0;

  MotionState = NoInMotion;
}

void JointWithoutHallSensor::Init(void)
{
  VirtualHall1.PortPin = PortPin_VirtualHall1;
  VirtualHall1.Init();
  VirtualHall1.setValue(HIGH);

  VirtualHall2.PortPin = PortPin_VirtualHall2;
  VirtualHall2.Init();
  VirtualHall2.setValue(LOW);

  VirtualHall3.PortPin = PortPin_VirtualHall3;
  VirtualHall3.Init();
  VirtualHall3.setValue(LOW);

  Motor.PortPin_SpeedPWM = PortPin_SpeedPWM;
  Motor.Timer_SpeedPWM = Timer_SpeedPWM;
  Motor.Channel_SpeedPWM = Channel_SpeedPWM;
  Motor.PortPin_FunctionState = PortPin_FunctionState;
  Motor.PortPin_Direction = PortPin_Direction;
  Motor.PortPin_ReadyState = PortPin_ReadyState;
  Motor.PortPin_RPM = PortPin_RPM;
  Motor.Init();
  Motor.setSpeed(15);
  Motor.setDirection(EC90Motor::CW);
  Motor.Disable();

  AnglePOT.PortPin = PortPin_AnglePOT;
  AnglePOT.ADCx = ADCx_AnglePOT;
  AnglePOT.ADC_Channel = ADC_Channel_AnglePOT;
  AnglePOT.Init();
  AnglePOT.Enable();

  FrontFSR.PortPin = PortPin_FrontFSR;
  FrontFSR.ADCx = ADCx_FrontFSR;
  FrontFSR.ADC_Channel = ADC_Channel_FrontFSR;
  FrontFSR.Init();
  FrontFSR.Enable();

  BackFSR.PortPin = PortPin_BackFSR;
  BackFSR.ADCx = ADCx_BackFSR;
  BackFSR.ADC_Channel = ADC_Channel_BackFSR;
  BackFSR.Init();
  BackFSR.Enable();
}

void JointWithoutHallSensor::MotionExtensionStart(void)
{
  MotionState = Extensioning;
  // USART_Send(USART2, "JWHS: Ex-Start\r\n");

  VirtualHallHandler(EC90Motor::CW);
  Motor.Enable();
}

void JointWithoutHallSensor::MotionFlexionStart(void)
{
  MotionState = Flexioning;
  // USART_Send(USART2, "JWHS: Fl-Start\r\n");

  VirtualHallHandler(EC90Motor::CCW);
  Motor.Enable();
}

JointWithoutHallSensor::SoftwareLimitStateTypeDef JointWithoutHallSensor::MotionExtensionStop(void)
{
#if defined(MODE_FOLLOWING) || defined(MODE_CONTINUOUS_START_STOP_TRIGGER)
  MotionState = NoInMotion;
#elif defined(MODE_START_STOP_TRIGGER)
  MotionState = WaitStop;
#endif
  // USART_Send(USART2, "JWHS: Ex-Stop\r\n");

  Motor.Disable();
  Motor.setSpeed(0);
}

JointWithoutHallSensor::SoftwareLimitStateTypeDef JointWithoutHallSensor::MotionFlexionStop(void)
{
#if defined(MODE_FOLLOWING) || defined(MODE_CONTINUOUS_START_STOP_TRIGGER)
  MotionState = NoInMotion;
#elif defined(MODE_START_STOP_TRIGGER)
  MotionState = WaitStop;
#endif
  // USART_Send(USART2, "JWHS: Fl-Stop\r\n");

  Motor.Disable();
  Motor.setSpeed(0);
}

void JointWithoutHallSensor::VirtualHallHandler(EC90Motor::RotationDirectionTypeDef Direction)
{
  if (Direction == EC90Motor::CW)
  {
    if (VirtualHallStep >= 5)
      VirtualHallStep = 0;
    else
      VirtualHallStep++;
  }
  else if (Direction == EC90Motor::CCW)
  {
    if (VirtualHallStep <= 0)
      VirtualHallStep = 5;
    else
      VirtualHallStep--;
  }

  switch (VirtualHallStep)
  {
  case 0:
    VirtualHall1.setValue(HIGH);
    VirtualHall2.setValue(LOW);
    VirtualHall3.setValue(LOW);
    break;
  case 1:
    VirtualHall1.setValue(HIGH);
    VirtualHall2.setValue(HIGH);
    VirtualHall3.setValue(LOW);
    break;
  case 2:
    VirtualHall1.setValue(LOW);
    VirtualHall2.setValue(HIGH);
    VirtualHall3.setValue(LOW);
    break;
  case 3:
    VirtualHall1.setValue(LOW);
    VirtualHall2.setValue(HIGH);
    VirtualHall3.setValue(HIGH);
    break;
  case 4:
    VirtualHall1.setValue(LOW);
    VirtualHall2.setValue(LOW);
    VirtualHall3.setValue(HIGH);
    break;
  case 5:
    VirtualHall1.setValue(HIGH);
    VirtualHall2.setValue(LOW);
    VirtualHall3.setValue(HIGH);
    break;

  default:
    VirtualHallStep = 0;
    break;
  }
}

void JointWithoutHallSensor::MotionHandler(void)
{
#if defined(MODE_FOLLOWING)
  switch (this->MotionState)
  {
  case Extensioning:
    if (this->ExtensionStartTriggered() == false)
    {
      this->MotionExtensionStop();
      USART_Send(USART2, "R: Ex-Stop\r\n");
    }
    else
    {
      this->MotionExtensionStart();
    }
    break;

  case Flexioning:
    if (this->FlexionStartTriggered() == false)
    {
      this->MotionFlexionStop();
      USART_Send(USART2, "R: Fl-Stop\r\n");
    }
    else
    {
      this->MotionFlexionStart();
    }
    break;

  case NoInMotion:
  default:
    if (this->ExtensionStartTriggered())
    {
      this->MotionExtensionStart();
      USART_Send(USART2, "R: Ex-Start\r\n");
    }
    else if (this->FlexionStartTriggered())
    {
      this->MotionFlexionStart();
      USART_Send(USART2, "R: Fl-Start\r\n");
    }
    break;
  }
#elif defined(MODE_CONTINUOUS_START_STOP_TRIGGER) || defined(MODE_START_STOP_TRIGGER)
  switch (this->MotionState)
  {
  case NoInMotion:
    if (this->ExtensionStartTriggered())
    {
      this->MotionExtensionStart();
      USART_Send(USART2, "R: Ex-Start\r\n");
    }
    else if (this->FlexionStartTriggered())
    {
      this->MotionFlexionStart();
      USART_Send(USART2, "R: Fl-Start\r\n");
    }
    break;

  case Extensioning:
    if (this->ExtensionStopTriggered())
    {
      this->MotionExtensionStop();
      USART_Send(USART2, "R: Ex-Stop\r\n");
    }
    else
    {
      this->MotionExtensionStart();
    }
    break;

  case Flexioning:
    if (this->FlexionStopTriggered())
    {
      this->MotionFlexionStop();
      USART_Send(USART2, "R: Fl-Stop\r\n");
    }
    else
    {
      this->MotionFlexionStart();
    }
    break;

  case WaitStop:
  default:
    this->MotionWaitStop();
    break;
  }
#endif
}

JointWithoutHallSensor::SoftwareLimitStateTypeDef JointWithoutHallSensor::MotionStop(void)
{
  Motor.Disable();
  Motor.setSpeed(0);

  VirtualHall1.setValue(HIGH);
  VirtualHall2.setValue(LOW);
  VirtualHall3.setValue(LOW);
  VirtualHallStep = 0;
}

// void Joint_SetAbsoluteAngle(float TargetAngle)
// {
//   float NowAngle = Joint_GetAbsoluteAngle();

//   GPIO_SetValue(RightMotor_FunctionStatePin, LOW); // LOW(0) = Disable; HIGH(1) = Enbale
//   PWM_SetDutyCycle(TIM3, CH2, 0);

//   /* Extension */
//   if ((NowAngle > TargetAngle) && (NowAngle > Convert_ADCValueToAngle(Joint_FullExtensionADCValue)))
//   {
//     GPIO_SetValue(RightMotor_DirectionPin, HIGH);     // CW(0) = Flexion; CCW(1) = Extension
//     GPIO_SetValue(RightMotor_FunctionStatePin, HIGH); // LOW(0) = Disable; HIGH(1) = Enbale
//     PWM_SetDutyCycle(TIM3, CH2, PWM_DefaultDutyCycle);

//     while ((NowAngle > TargetAngle) && (NowAngle > Convert_ADCValueToAngle(Joint_FullExtensionADCValue)))
//     {
//       NowAngle = Joint_GetAbsoluteAngle(); // Update angle.
//       USART_Send(USART2,
//                  convertIntToString(
//                      round(Convert_ADCValueToAngle(ADC_GetValue(ADC1, ADC_Channel_1, 1,
//                                                                 ADC_SampleTime_55Cycles5)))));
//       USART_Send(USART2, "\r\n");
//     }

//     GPIO_SetValue(RightMotor_FunctionStatePin, LOW); // LOW(0) = Disable; HIGH(1) = Enbale
//     PWM_SetDutyCycle(TIM3, CH2, 0);
//   }
//   /* Flexion */
//   else if ((NowAngle < TargetAngle) && (NowAngle < Convert_ADCValueToAngle(Joint_FullFlexionADCValue)))
//   {
//     GPIO_SetValue(RightMotor_DirectionPin, LOW);      // CW(0) = Flexion; CCW(1) = Extension
//     GPIO_SetValue(RightMotor_FunctionStatePin, HIGH); // LOW(0) = Disable; HIGH(1) = Enbale
//     PWM_SetDutyCycle(TIM3, CH2, PWM_DefaultDutyCycle);

//     while ((NowAngle < TargetAngle) && (NowAngle < Convert_ADCValueToAngle(Joint_FullFlexionADCValue)))
//     {
//       NowAngle = Joint_GetAbsoluteAngle(); // Update angle.
//       USART_Send(USART2,
//                  convertIntToString(
//                      round(Convert_ADCValueToAngle(ADC_GetValue(ADC1, ADC_Channel_1, 1,
//                                                                 ADC_SampleTime_55Cycles5)))));
//       USART_Send(USART2, "\r\n");
//     }

//     GPIO_SetValue(RightMotor_FunctionStatePin, LOW); // LOW(0) = Disable; HIGH(1) = Enbale
//     PWM_SetDutyCycle(TIM3, CH2, 0);
//   }
// }

// inline float Joint_GetAbsoluteAngle(void)
// {
//   uint16_t ADC_Value = ADC_GetValue(ADC1, ADC_Channel_1, 1, ADC_SampleTime_55Cycles5);
//   return Convert_ADCValueToAngle(ADC_Value);
// }

// inline Joint_LimitStateTypeDef getLimitState(void)
// {
//   if (ADC_GetValue(ADC1, ADC_Channel_1, 1, ADC_SampleTime_55Cycles5) <= Joint_FullExtensionADCValue)
//     return ((Joint_LimitStateTypeDef)FullExtension);
//   else if (ADC_GetValue(ADC1, ADC_Channel_1, 1, ADC_SampleTime_55Cycles5) >= Joint_FullFlexionADCValue)
//     return ((Joint_LimitStateTypeDef)FullFlexion);
//   else
//     return ((Joint_LimitStateTypeDef)Unlimited);
// }

/*********************************END OF FILE**********************************/
