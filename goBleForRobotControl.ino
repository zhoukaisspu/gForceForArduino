#include <Servo.h>
#include <Metro.h>
#include "GoBLE.h"
#include "QueueArray.h"

// create a queue of characters.
QueueArray <byte> apiQueue;

/**************** Motor parameter *******************/

#define MAXBUTTONEVENT                 6
#define MAXBUTTONID                    7
int joystickX, joystickY;
int buttonEvent[MAXBUTTONEVENT];
int buttonState[MAXBUTTONID];

int speedLeft, speedRight;

/******************* Declear Motor Functions and Parameters ******************/
// Motor Configuration

struct MotorDriver
{
  // command motor variables
  int commLeftSpeed;
  int commRightSpeed;

  // motor variables
  int leftSpeed;
  int rightSpeed;

  // Driver pins
  byte leftEnablePin;
  byte leftPWMPin;
  byte rightEnablePin;
  byte rightPWMPin;

};

MotorDriver L298;

struct Weapon {
  byte weaponMotion;

  int pitchAngle;
  int headingAngle;

  int motorSpeed;
  byte motorEnablePin;
  byte motorPWMPin;
  
  boolean shooting;
};

Weapon weapon;

Servo filterServo;
Servo loadBulletServo;
Servo pitchServo;
Servo headingServo;

//DL package
#pragma pack(1)
typedef struct
{
  byte  header1;  // 0x55
  byte  header2;  // 0xAA
  byte  address;
  byte  latestDigitalButton;
  byte  digitalButton;
  byte  joystickPosition;
  byte	buttonPayload[MAXBUTTONEVENT];
  byte	joystickPayload[4];
  byte  checkSum;

  byte 	commandLength;
  boolean newCommand;
  byte newCommandNumber;
  byte parseState;
} sDataLink;
#pragma pack()

sDataLink rDataPack;
sDataLink sDataPack;

#define PACKHEADER	        1
#define PACKHEADER2		2
#define PACKADDRESS		3
#define PACKBUTTONSIGN		4
#define PACKJOYSTICKSIGN	5
#define PACKPAYLOAD             6

#define DEFAULTPACKLENGTH	10

#define PARSEERROR              0x11
#define PARSESUCCESS            0x10

/**************** Debugger Configuration ******************/

#define DEBUGAPIPROCESSOR	false
#define DEBUGPARSERAW      	false
#define DEBUGPARSER      	false

#define DEBUGREADCHANNEL	false
#define DEBUGLOOP               true
#define DEBUGCOMMANDSPEED       false



void setup() {
  // put your setup code here, to run once:
  initDebugger();

  initQueue();
  initRecvDataPack();

  initParameter();

  initWeapon();
  initMotorDiver();

}

void initWeapon(){
  /*
  byte weaponMotion;

  int pitchAngle;
  int headingAngle;

  int motorSpeed;
  byte motorEnablePin;
  byte motorPWMPin;
  */
  
  weapon.pitchAngle = 75;
  weapon.headingAngle = 90;
  
  pitchServo.attach(13);
  pitchServo.write(weapon.pitchAngle);
  
  
  headingServo.attach(12);
  headingServo.write(weapon.headingAngle);
  
  loadBulletServo.attach(8);
  loadBulletServo.write(96);
  
  filterServo.attach(9);
  filterServo.write(0);

  weapon.motorSpeed = 150;
  weapon.motorEnablePin = 3;
  weapon.motorPWMPin = 11;
  
  pinMode(weapon.motorEnablePin,OUTPUT);
  digitalWrite(weapon.motorEnablePin,LOW);
  
  
  stopLauncher();
}


/*** Motor Driver functions ***/
void initMotorDiver() {
  L298.leftEnablePin = 4;
  L298.leftPWMPin = 5;
  L298.rightPWMPin = 6;
  L298.rightEnablePin = 7;

  pinMode(L298.leftEnablePin, OUTPUT);
  pinMode(L298.leftPWMPin, OUTPUT);

  pinMode(L298.rightEnablePin, OUTPUT);
  pinMode(L298.rightPWMPin, OUTPUT);

  analogWrite(L298.leftPWMPin, 0);
  analogWrite(L298.rightPWMPin, 0);

  L298.leftSpeed = 0;
  L298.rightSpeed = 0;
}

void initParameter() {

  clearButtonBuffer();
  joystickX = 0;
  joystickY = 0;

}

void clearButtonBuffer() {
  for (int i = 0; i < MAXBUTTONEVENT; i++) {
    buttonEvent[i] = -1;
  }
  for (int i = 0; i < MAXBUTTONID; i++) {
    buttonState[i] = RELEASED;
  }
}

void initDebugger() {
  Serial.begin(115200);
}

void initQueue() {

}


void initRecvDataPack() {
  rDataPack.header1 = 0x55;
  rDataPack.header2 = 0xAA;
  rDataPack.address = 0x11;

  rDataPack.commandLength = DEFAULTPACKLENGTH;

  rDataPack.parseState = 0x00;
  rDataPack.newCommand = false;
  rDataPack.newCommandNumber = 0;
}

void loop() {
	
  // put your main code here, to run repeatedly:
  apiProcessor();

  if (rDataPack.newCommandNumber > 0) {
    readCommandChannel(rDataPack.newCommandNumber);
    rDataPack.newCommandNumber = 0;
  }

  runMotor(joystickX, joystickY);
//  MotorControl(0,0);    // motor tester, go forward stright

  // When the right button pressed, just shoot!!!!! = =!
  
  if(buttonState[SWITCH_RIGHT] == PRESSED || weapon.shooting)  fireTarget();
  else stopLauncher();

  if(buttonState[SWITCH_UP] == PRESSED || buttonState[SWITCH_DOWN] == PRESSED)  weaponPitchController();
  
  if (DEBUGLOOP)  Printer();
  //  WeaponTrigger();
}

void weaponPitchController(){
  static Metro pitchServoManager = Metro(10,true);
  
  if(pitchServoManager.check()){

    pitchServoManager.reset();
    if(buttonState[SWITCH_UP] == PRESSED && weapon.pitchAngle != 55){
      delay(2);
      if(weapon.pitchAngle > 55)  weapon.pitchAngle--;
      else weapon.pitchAngle = 55;
    }
    
   else if(buttonState[SWITCH_DOWN] == PRESSED && weapon.pitchAngle != 110){
         delay(5);
      if(weapon.pitchAngle < 110)  weapon.pitchAngle++;
      else weapon.pitchAngle = 110;
    }
    pitchServoManager.interval(30);
    pitchServo.write(weapon.pitchAngle);
  }
  
}

void runLauncherMotor(int runSpeed){
  analogWrite(weapon.motorPWMPin,runSpeed);
}

void fireTarget(){
  
  static Metro fireManager = Metro(10,true);
  static int fireSteps = 0;
  static int launcherAngle = 0;
  
  loadBulletServo.write(100);
  runLauncherMotor(weapon.motorSpeed);
  
  
  if(fireManager.check()){
    switch(fireSteps){
      case 0:
        weapon.shooting = true;
        if(launcherAngle < 150)  launcherAngle += 2;
        else fireSteps = 1;
        fireManager.interval(10);
        break;  
      
      case 1:
        weapon.shooting = false;
        launcherAngle = 0;
        fireManager.interval(1500);
        fireSteps = 0;
        break;
      
      default:
        fireSteps = 0;
        break;
    }
    
    filterServo.write(launcherAngle);
  }
  
}

void stopLauncher(){
  weapon.shooting = false;
  runLauncherMotor(50);
  loadBulletServo.write(97);
  filterServo.write(0);
}

void runMotor(int _joystickx, int _joysticky) {

  static Metro motorManager = Metro(50, true);
  static Metro convertionManager = Metro(50, true);
  static Metro commandPrinterManager = Metro(50, true);


  if (convertionManager.check()) {
    int  rValue = 0, lValue = 0;

    uint8_t rh = _joystickx + 128;
    if (rh > 0x80) {
      rh = constrain (rh, 0x80, 0xff);
      rValue = map (rh, 0x80, 0xff, 0, 255);
    }
    else {
      rh = constrain (rh, 0x00, 0x80);
      rValue = map (rh, 0x00, 0x80, -255, 0);
    }
    lValue = rValue;
    uint8_t lh = _joysticky + 128;
    int  value;
    if (lh > 0x80) {
      lh = constrain (lh, 0x80, 0xff);
      value = map (lh, 0x80, 0xff, 0, 255);

    }
    else {
      lh = constrain (lh, 0x00, 0x80);
      value = map (lh, 0x00, 0x80, -255, 0);
    }
    lValue += value;
    rValue -= value;
    lValue = constrain (lValue, -255, 255);
    rValue = constrain (rValue, -255, 255);

    // convert the command value to the control signal

    speedLeft = constrain(lValue, -255, 255);
    speedRight = constrain(rValue, -255, 255);

    MotorControl(speedLeft, speedRight);
  }
  if (commandPrinterManager.check() && DEBUGCOMMANDSPEED) {

//    Serial.print("runMotor ");
//    Serial.print("->");
//    Serial.print(speedLeft);
//    Serial.print(" ");
//    Serial.println(speedRight);
  }


}

void MotorControl(int lSpeed, int rSpeed) {
  if (lSpeed > 0) {
    digitalWrite(L298.leftEnablePin, HIGH);
  }
  else {
    digitalWrite(L298.leftEnablePin, LOW);
  }
  analogWrite(L298.leftPWMPin, abs(lSpeed));

  if (rSpeed > 0) {
    digitalWrite(L298.rightEnablePin, HIGH);
  }
  else {
    digitalWrite(L298.rightEnablePin, LOW);
  }
  analogWrite(L298.rightPWMPin, abs(rSpeed));
}



void Printer() {

  static Metro printerManager = Metro(1000, true);

  if (printerManager.check() == true) {

    //    Serial.print("Joystick: ");
    //    Serial.print(joystickX);
    //    Serial.print(" ");
    //    Serial.print(joystickY);
    //    Serial.print("\n");

//    for (int i = 0; i < MAXBUTTONID; i++) {
//      Serial.print("Button ID: ");
//      Serial.print(i);
//      Serial.print("\t State: ");
//      if (buttonState[i] == PRESSED)   Serial.println("Pressed!");
//      if (buttonState[i] == RELEASED)  Serial.println("Released!");
//    }

    /*
    int buttonEventNumber = scanButtonEvent();

    if (buttonEventNumber != 0)
      for (int i = 0; i < buttonEventNumber; i ++) {
        switch (buttonEvent[i]) {
          case SWITCH_DOWN:
            Serial.println("Printer -> Button Event:  DOWN! ");
            break;

          case SWITCH_LEFT:
            Serial.println("Printer -> Button Event:  LEFT! ");
            break;

          case SWITCH_UP:
            Serial.println("Printer -> Button Event:  UP! ");
            break;

          case SWITCH_RIGHT:
            Serial.println("Printer -> Button Event:  RIGHT! ");
            break;

          case SWITCH_SELECT:
            Serial.println("Printer -> Button Event:  SELECT! ");
            break;

          case SWITCH_START:
            Serial.println("Printer -> Button Event:  START! ");
            break;

          default:
            Serial.print("Printer -> Button Event:  UNDEFINED!!! Number: ");
            Serial.println(buttonEvent[i]);
            break;
        }
      }
    clearButtonEvent();
    */
//    Serial.print("\r\n");
  }
}

int scanButtonEvent() {
  int latestButtonEventNumber = 0;
  for (int i = 0; i < MAXBUTTONEVENT; i++) {
    if (buttonEvent[i] != -1)  latestButtonEventNumber++;
  }
  return latestButtonEventNumber;
}

void readCommandChannel(int channel) {

  if (channel == 1 && rDataPack.latestDigitalButton != 0) {
    for (int i = 0; i < MAXBUTTONID; i++) {
      if (buttonState[i] == PRESSED) {
        if (DEBUGREADCHANNEL) {
//          Serial.print("read commmand channel -> clear Pressed button number: ");
//          Serial.println(i);
        }
        buttonState[i] = RELEASED;
      }
    }
  }

  for (int i = 0; i < channel - 1; i++) {
    buttonEvent[i] = rDataPack.buttonPayload[i];
    buttonState[buttonEvent[i]] = PRESSED;
  }

  joystickX = readJoystickX();
  joystickY = readJoystickY();

}

int readJoystickX() {
  return int(rDataPack.joystickPayload[0] - 128);
}

int readJoystickY() {
  return int(rDataPack.joystickPayload[1] - 128);
}

void apiProcessor() {

  /*
  function introduction:
  	* push the new valid data to the data buffer package
  	* throw away the invalid byte
  	* parse the data package when the command length is matching the protocol
  */

  if (Serial.available()) {
    byte inputByte = Serial.read();
//
//
//    if (DEBUGAPIPROCESSOR) {
//      Serial.print("api -> new data:");
//      Serial.println(inputByte, HEX);
//    }

    // throw the trash data and restore the useful data to the queue buffer
    if (inputByte == rDataPack.header1 || rDataPack.newCommand == true) {
      apiQueue.push(inputByte);
      rDataPack.newCommand = true;

      // auto adjust the command length based on the button command value
      if (apiQueue.count() == PACKBUTTONSIGN) {  //get the count of pressed key

        // max button input at one moment should less than 6 buttons
        if (inputByte > 0 && inputByte < MAXBUTTONEVENT) {

          // default command length + button number
          rDataPack.commandLength = DEFAULTPACKLENGTH + inputByte;
 //         if (DEBUGAPIPROCESSOR)	Serial.print("api -> Command Length:"), Serial.println(rDataPack.commandLength);
        }
        else	rDataPack.commandLength = DEFAULTPACKLENGTH;
      }
    }
  }

//  if (DEBUGPARSERAW) {
//    Serial.println("apiProcessor -> new Pack!");
//    for (int i = 0; i < rDataPack.commandLength; i++) {
//      Serial.print(apiQueue.pop(), HEX);
//    }
//    Serial.println();
//  }

  if (rDataPack.newCommand && apiQueue.count() == rDataPack.commandLength) {

    /*
    byte  header1;          // 0x55
    byte  header2;          // 0xAA
    byte  address;
    byte  digitalButton;
    byte  joystickPosition;
    byte  buttonPayload[6];
    byte  joystickPayload[4];
    byte  checkSum;
    */
    byte calculateSum = 0;

    rDataPack.header1 = apiQueue.pop(), calculateSum +=  rDataPack.header1;
    rDataPack.header2 = apiQueue.pop(), calculateSum +=  rDataPack.header2;
    rDataPack.address = apiQueue.pop(), calculateSum +=  rDataPack.address;

    rDataPack.latestDigitalButton = rDataPack.digitalButton;
    rDataPack.digitalButton = apiQueue.pop(), calculateSum +=  rDataPack.digitalButton;

    int digitalButtonLength = rDataPack.digitalButton;

    rDataPack.joystickPosition = apiQueue.pop(), calculateSum +=  rDataPack.joystickPosition;

    // read button data package - dynamic button payload length
    for (int buttonPayloadPointer = 0; buttonPayloadPointer < digitalButtonLength; buttonPayloadPointer++) {
      rDataPack.buttonPayload[buttonPayloadPointer] = apiQueue.pop();
      calculateSum +=  rDataPack.buttonPayload[buttonPayloadPointer];
    }
    // read 4 byte joystick data package
    for (int i = 0; i < 4; i++)  rDataPack.joystickPayload[i] = apiQueue.pop(), calculateSum +=  rDataPack.joystickPayload[i];

    rDataPack.checkSum = apiQueue.pop();

    if (rDataPack.checkSum == calculateSum)  rDataPack.parseState = PARSESUCCESS, rDataPack.newCommandNumber = rDataPack.digitalButton + 1;
    else                                    rDataPack.parseState = PARSEERROR;


//    if (DEBUGPARSER) {
//      Serial.print("Parser -> sum calculation: ");
//      Serial.println(calculateSum);
//
//      Serial.print("Parser -> checkSum byte value: ");
//      Serial.println(rDataPack.checkSum);
//    }

    rDataPack.newCommand = false;

  }
}


/*


int WeaponTrigger(){

  static Metro scanButtonManager = Metro(5, true);

  // scan new button event input from the phone control every 5ms once!
  // if the user click the right button, trigger the weapon firing action

  if(scanButtonManager.check()){

    int buttonEventNumber = scanButtonEvent();
    if (buttonEventNumber != 0){

      for (int i = 0; i < buttonEventNumber; i ++) {
        switch (buttonEvent[i]) {
          case SWITCH_DOWN:
            Serial.println("Printer -> Button Event:  DOWN! ");
            break;

          case SWITCH_LEFT:
            Serial.println("Printer -> Button Event:  LEFT! ");
            break;

          case SWITCH_UP:
            Serial.println("Printer -> Button Event:  UP! ");
            break;

          case SWITCH_RIGHT:
            Serial.println("Printer -> Button Event:  RIGHT! ");
            break;

          case SWITCH_SELECT:
            Serial.println("Printer -> Button Event:  SELECT! ");
            break;

          case SWITCH_START:
            Serial.println("Printer -> Button Event:  START! ");
            break;

          default:
            Serial.print("Printer -> Button Event:  UNDEFINED!!! Number: ");
            Serial.println(buttonEvent[i]);
            break;
        }
      }
//      clearButtonEvent();
    }
  }
}

*/

