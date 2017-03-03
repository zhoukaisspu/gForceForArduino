#include <Servo.h>
#include "Metro.h"
#include "robotAdapter.h"
#include "gForceAdapter.h"


RobotAdapter    robot = RobotAdapter();
gForceAdapter   gforce = gForceAdapter();
void setup() {
    robot.InitRobot();
}

void loop() {
        if(gforce.updateData()){
                GforceData_t  gForceData;
                gforce.GetAvaliableData(&gForceData);
                GforceMsg_t msgtype = gForceData.msgType;
                 Gesture_t gesture;
                switch (msgtype){
                      case Gforce_Quaternion:
                          break;
                      case Gforce_gesture:
                           gesture = gForceData.value.gesture;
                          if(gesture == Fist) {
                                 robot.go(200);
                          } else if(gesture == Spread) {
                                 robot.back(200);
                          } else if(gesture == WaveIn) {
                                  robot.turnLeftAtAcc(125,2,Forward);
                          } else if(gesture == WaveOut) {
                                  robot.turnLeftAtAcc(125,2,Backward);
                          } else {
                                 robot.shutoff();
                          }
                          break;
                      default:
                           break;
                }
        }
}





