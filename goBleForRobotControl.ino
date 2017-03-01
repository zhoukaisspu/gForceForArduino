#include <Servo.h>
#include "Metro.h"
#include "GoBLE.h"
#include "QueueArray.h"
// create a queue of characters.
QueueArray <byte> apiQueue;

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

 //////////////////////////////////////////////////////////
 /////global variable
Servo BulletServo;        
Servo headingServo;
Servo pitchServo;

/********************************************************
 * peripheral Init
 *******************************************************/
 //left motor control
 void InitLeftMotor(void)
 {
      pinMode(LEFT_ENABLE_PIN,OUTPUT);  // set left motor enable pin output
      pinMode(LEFT_PWM_PIN,OUTPUT);     // set left motor pwm pin output
      digitalWrite(LEFT_ENABLE_PIN,LOW);  // set left motor disable
 }
 void SetLeftSpeed(int lSpeed)
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

 // right motor control
 void InitRightMotor(void)
 {
      pinMode(RIGHT_ENABLE_PIN,OUTPUT); // set right motor enable pin output
      pinMode(RIGHT_PWM_PIN,OUTPUT);    // set right pwm pin output
      digitalWrite(RIGHT_ENABLE_PIN,LOW); // set right motor disable
 }
 void SetRightSpeed(int rSpeed)
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
 
 //weapon motor control
 void IninWeaponMotor(void)
 {
      pinMode(WEAPON_ENABLE_PIN,OUTPUT);
      pinMode(WEAPON_PWN_PIN,OUTPUT);
      digitalWrite(WEAPON_ENABLE_PIN,LOW);
 }
 void SetWeaponSpeed(int wSpeed)
 {
      if(wSpeed>0) {
          digitalWrite(WEAPON_ENABLE_PIN,HIGH);
          analogWrite(WEAPON_ENABLE_PIN,wSpeed);
      } else {
          digitalWrite(WEAPON_ENABLE_PIN,LOW);
      }
 }

 //load bullet control
void InitLoadBulletServo(void)
{
    BulletServo.attach(LOAD_BULLET_SERVO);
    BulletServo.write(LOAD_BULLET_ANGLE);
}
void SetLoadBulletServoAngle(int angle)
{
    BulletServo.write(angle);
}

//heading control
void InitHeadingServo(void)
{
    headingServo.attach(HEADING_SERVO);
    headingServo.write(HEADING_ANGLE);
}
void SetHeadingServoAngle(int angle)
{
    headingServo.write(angle);
}

//pitch servo control
void InitPitchServo(void)
{ 
    pitchServo.attach(PITCH_SERVO);
    pitchServo.write(PITCH_ANGLE);
}
void SetPitchServoAngle(int angle)
{
    pitchServo.write(angle);
}

//init uart
void InitUart() {
  Serial.begin(115200);
}


void setup() {
  InitLeftMotor();
  InitRightMotor();
  IninWeaponMotor();
  InitLoadBulletServo();
  InitHeadingServo();
  InitPitchServo();
  InitUart();
}

#define MAGNUM1_INDEX     0
#define MAGNUM2_INDEX     1
#define EVENTTYP_INDEX    2
#define MSG_LEN_INDEX     3
unsigned int GetControlPackage(int* data,unsigned int length)
{
  int s_receiveByteIndex = 0;
  while(1)
  {
      if(Serial.available())
      {
          data[s_receiveByteIndex] = Serial.read();
          if(data[s_receiveByteIndex]==-1)
          {
              continue;
          }
          if(s_receiveByteIndex == 0) {
            if(data[s_receiveByteIndex] == 0xFF){
               s_receiveByteIndex++;
            }
          } else if(s_receiveByteIndex == 1) {
            if(data[s_receiveByteIndex] == 0xAA)
            {
              s_receiveByteIndex++;  
            } else {
              s_receiveByteIndex = 0;
            }
          } else if(s_receiveByteIndex <= 3) {
            s_receiveByteIndex++;
          } else if(s_receiveByteIndex>3) {
            s_receiveByteIndex++;
            if(s_receiveByteIndex>=(data[3]+4))
            {
              s_receiveByteIndex = 0;
              return (data[3]+4);
            }
          }
      } 
  }
}
typedef struct DForbot
{
  //1:go 2:down 3:left 4:right 5:stop
    char DeviceLastStatus;
    char DeviceStatus;
    int  deviceSpeed;
}dfRobot;
  dfRobot robot;
void loop() {
  int data[30];
  unsigned int len = GetControlPackage(data,30);
  if(len>0)
  {
    if((data[2]&0x7f)==0x0f)
    {
      if(data[5] == 0x01) {
        robot.deviceSpeed = 150;
        robot.DeviceStatus = 1;
        SetLeftSpeed(150);
        SetRightSpeed(150);
      } else if(data[5]==0x02){
        robot.deviceSpeed = -150;
        robot.DeviceStatus = 2;
        SetLeftSpeed(-150);
        SetRightSpeed(-150);
      } else if(data[5] == 0x03) {
        if(robot.DeviceStatus==1)
        {
               SetLeftSpeed(-150);
               SetRightSpeed( robot.deviceSpeed/3+robot.deviceSpeed);
               robot.DeviceLastStatus = 1;
               robot.DeviceStatus = 3;
        } else if(robot.DeviceStatus==2){
               SetLeftSpeed(150);
               SetRightSpeed( robot.deviceSpeed/3+robot.deviceSpeed);
               robot.DeviceLastStatus = 2;
               robot.DeviceStatus = 3;
        } else if(robot.DeviceStatus==0) {
               SetLeftSpeed(-150);
               SetRightSpeed(150);
               robot.DeviceLastStatus = 0;
               robot.DeviceStatus = 3;
        }
      } else if(data[5]== 0x04){
         if(robot.DeviceStatus==1)
        {
               SetRightSpeed(-150);
               SetLeftSpeed( robot.deviceSpeed/3+robot.deviceSpeed);
               robot.DeviceLastStatus = 1;
               robot.DeviceStatus = 4;
        } else if(robot.DeviceStatus==2){
               SetRightSpeed(150);
               SetLeftSpeed( robot.deviceSpeed/3+robot.deviceSpeed);
               robot.DeviceLastStatus = 2;
               robot.DeviceStatus = 4;
        } else if(robot.DeviceStatus==0) {
               SetRightSpeed(-150);
               SetLeftSpeed(150);
               robot.DeviceLastStatus = 0;
               robot.DeviceStatus = 4;
        }
      } else if(data[5]==0x05){
               SetRightSpeed(0);
               SetLeftSpeed(0);
               robot.DeviceLastStatus = 0;
               robot.DeviceStatus = 0;
               robot.deviceSpeed = 0;
      } else if(data[5]==0x06){
        SetWeaponSpeed(70);
      }else if(data[5] ==0x00){
         if((robot.DeviceStatus == 3)||(robot.DeviceStatus ==4) ){
               SetRightSpeed(robot.deviceSpeed);
               SetLeftSpeed(robot.deviceSpeed);
               robot.DeviceStatus = robot.DeviceLastStatus;
               robot.DeviceLastStatus = 0;
         }
         SetWeaponSpeed(0);
      }
    }
  }
}





