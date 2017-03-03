#ifndef ROBOTADAPTER_H
#define ROBOTADAPTER_H
#include <Arduino.h>
#include <Servo.h>
enum RobotSta{
    Go,
    Back,
    Left,
    Right,
    LeftAcc,
    RightAcc,
    Shutoff
};

enum AccDirection{
  Forward,
  Backward
};

class RobotAdapter{
    public:
        RobotAdapter();
        ~RobotAdapter();
        void InitRobot(void);
        void go(int dspeed);
        void back(int dspeed);
        void turnLeftAtOrigin(int oSpeed);
        void turnRightAtOrigin(int oSpeed);
        void turnLeftAtAcc(int baseSpeed,int multiple,AccDirection dire);
        void trunRightAtAcc(int baseSpeed,int multiple,AccDirection dire);
        void shutoff(void);
        void fire(int fSpeed);
        void setHeadingAngle(int angle);    
        int  getSpeed(void);
        RobotSta getRobotStatus(void);

        int getUartdata(void);
    private:
       RobotSta mRobotStatus;
       int      mRobotSpeed;
       Servo BulletServo;        
       Servo headingServo;
       Servo pitchServo;
       
       void InitLeftMotor(void);
       void SetLeftSpeed(int lSpeed);
       
       void InitRightMotor(void); 
       void SetRightSpeed(int rSpeed);
       
       void IninWeaponMotor(void);
       void SetWeaponSpeed(int wSpeed);
       
       void InitLoadBulletServo(void);
       void SetLoadBulletServoAngle(int angle);
       
       void InitPitchServo(void);
       void SetPitchServoAngle(int angle);
       
       void InitHeadingServo(void);
       void SetHeadingServoAngle(int angle);  

       void InitUart(void);
};
#endif
