/*****************************************************************************************
 * 
 * 
 * It is used to simulate dfrbot's robot
 * 
 */

#include "robotAdapter.h"
/****************************************************/
/*PIN define*/
/****************************************************/
//It is use to control motor
#define LEFT_ENABLE_PIN       6
#define LEFT_PWM_PIN          7

#define RIGHT_ENABLE_PIN      5
#define RIGHT_PWM_PIN         4

#define WEAPON_ENABLE_PIN     11
#define WEAPON_PWN_PIN        3

//It is used to control Servo
#define LOAD_BULLET_SERVO     8
#define PITCH_SERVO           9
#define HEADING_SERVO        10

/****************************************************
 * Servo angle
 */
 #define HEADING_ANGLE            90         //heading angle(50~140)
 #define LOAD_BULLET_ANGLE        0         
 #define PITCH_ANGLE              75
///////////////////////////////////////////////////////////////////////////////////////
/////////private function in class robotAdapter
void RobotAdapter::InitLeftMotor(void)
{
     pinMode(LEFT_ENABLE_PIN,OUTPUT);  // set left motor enable pin output
     pinMode(LEFT_PWM_PIN,OUTPUT);     // set left motor pwm pin output
     digitalWrite(LEFT_ENABLE_PIN,LOW);  // set left motor disable
}

void RobotAdapter::SetLeftSpeed(int lSpeed)
{
    if(lSpeed>0) {
        digitalWrite(LEFT_PWM_PIN,LOW);  // set left motor enable
        analogWrite(LEFT_ENABLE_PIN,lSpeed);    // set left motor speed equal to lSpeed
    } else if(lSpeed<0){
        digitalWrite(LEFT_PWM_PIN,HIGH);  // set left motor enable
        analogWrite(LEFT_ENABLE_PIN,abs(lSpeed));  // set left motor enable
    } else {
        digitalWrite(LEFT_ENABLE_PIN,LOW);  // set left motor enable
    }
}

void RobotAdapter::InitRightMotor(void)
{
      pinMode(RIGHT_ENABLE_PIN,OUTPUT); // set right motor enable pin output
      pinMode(RIGHT_PWM_PIN,OUTPUT);    // set right pwm pin output
      digitalWrite(RIGHT_ENABLE_PIN,LOW); // set right motor disable
}

void RobotAdapter::SetRightSpeed(int rSpeed)
{
      if(rSpeed>0) {
          digitalWrite(RIGHT_PWM_PIN,LOW);
          analogWrite(RIGHT_ENABLE_PIN,rSpeed);
      } else if(rSpeed<0){
          digitalWrite(RIGHT_PWM_PIN,HIGH);
          analogWrite(RIGHT_ENABLE_PIN,abs(rSpeed));
      } else {
          digitalWrite(RIGHT_ENABLE_PIN,LOW);
      }
}

void RobotAdapter::IninWeaponMotor(void)
{
      pinMode(WEAPON_ENABLE_PIN,OUTPUT);
      pinMode(WEAPON_PWN_PIN,OUTPUT);
      digitalWrite(WEAPON_ENABLE_PIN,LOW);
}

void RobotAdapter::SetWeaponSpeed(int wSpeed)
{
      if(wSpeed>0) {
          digitalWrite(WEAPON_ENABLE_PIN,HIGH);
          analogWrite(WEAPON_ENABLE_PIN,wSpeed);
      } else {
          digitalWrite(WEAPON_ENABLE_PIN,LOW);
      }
}

void RobotAdapter::InitLoadBulletServo(void)
{
    BulletServo.attach(LOAD_BULLET_SERVO);
    BulletServo.write(LOAD_BULLET_ANGLE);
}

void RobotAdapter::SetLoadBulletServoAngle(int angle)
{
  BulletServo.write(angle);
}

void RobotAdapter::InitPitchServo(void)
{
    pitchServo.attach(PITCH_SERVO);
    pitchServo.write(PITCH_ANGLE);
}

void RobotAdapter::SetPitchServoAngle(int angle)
{
  pitchServo.write(angle);
}

void RobotAdapter::InitHeadingServo(void)
{
    headingServo.attach(HEADING_SERVO);
    headingServo.write(HEADING_ANGLE);  
}

void RobotAdapter::SetHeadingServoAngle(int angle)
{
    headingServo.write(angle);  
}

void RobotAdapter::InitUart(void)
{
    Serial.begin(115200);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////public function in class robotAdapter
RobotAdapter::RobotAdapter();
{
    mRobotSpeed = 0;
    mRobotStatus =  Shutoff;
}

void RobotAdapter::InitRobot(void)
{
    InitLeftMotor();
    InitRightMotor();
    IninWeaponMotor();
    InitLoadBulletServo();
    InitPitchServo();
    InitHeadingServo();
    InitUart();
}

void RobotAdapter::go(int dspeed)
{
  SetLeftSpeed(dspeed);
  SetRightSpeed(dspeed);
  mRobotSpeed = dspeed;
  mRobotStatus = Go;
}

void RobotAdapter::back(int dspeed)
{
  int tmpS = 0 - dspeed;
  SetLeftSpeed(tmpS);
  SetRightSpeed(tmpS);
  mRobotSpeed = tmpS;
  mRobotStatus = Back;
}

RobotSta RobotAdapter::getRobotStatus(void)
{
  return mRobotStatus;
}

int  RobotAdapter::getSpeed(void)
{
  return mRobotSpeed;
}

void RobotAdapter::turnLeftAtOrigin(int oSpeed)
{
    int tmps = 0 - oSpeed;
    SetLeftSpeed(tmps);
    SetRightSpeed(oSpeed);
    mRobotStatus = Left;
    mRobotSpeed = oSpeed;
}

void RobotAdapter::turnRightAtOrigin(int oSpeed)
{
    int tmps = 0 - oSpeed;
    SetLeftSpeed(oSpeed);
    SetRightSpeed(tmps);
    mRobotStatus = Right;
    mRobotSpeed = oSpeed;
}

void RobotAdapter::turnLeftAtAcc(int baseSpeed,int multiple,AccDirection dire)
{
  int tmps = 0 - baseSpeed;
  if(dire == Forward) {
       SetLeftSpeed(baseSpeed);
       SetRightSpeed(baseSpeed*multiple);
  }else if(dire == Backward) {
       SetLeftSpeed(tmps*multiple);
       SetRightSpeed(tmps);
  }
  mRobotStatus =  LeftAcc;
  mRobotSpeed = baseSpeed;
}


void RobotAdapter::trunRightAtAcc(int baseSpeed,int multiple,AccDirection dire)
{
    int tmps = 0 - baseSpeed;
    if(dire == Forward) {
        SetLeftSpeed(baseSpeed*multiple);
        SetRightSpeed(baseSpeed);
    }else if(dire == Backward) {
        SetLeftSpeed(tmps);
         SetRightSpeed(tmps*multiple);
     }
     mRobotStatus =  RightAcc;
     mRobotSpeed = baseSpeed;
}

void RobotAdapter::shutoff(void)
{
     SetLeftSpeed(0);
     SetRightSpeed(0);
     mRobotStatus =  Shutoff;
     mRobotSpeed = 0;
}

void RobotAdapter::fire(int fSpeed)
{
     SetWeaponSpeed(fSpeed);
}

int RobotAdapter::getUartdata()
{
  if(Serial.available())
  {
     return Serial.read();
  }
  return -1;
}

