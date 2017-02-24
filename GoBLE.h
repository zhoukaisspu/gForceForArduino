#ifndef GOBLE_H_
#define GOBLE_H_

#include <Arduino.h>

const byte CH_SWITCH_1    = 1;
const byte CH_SWITCH_2    = 2;
const byte CH_SWITCH_3    = 3;
const byte CH_SWITCH_4    = 4;
const byte CH_SWITCH_5    = 5;
const byte CH_SWITCH_6    = 6;
const byte CH_JOYSTICK_SW = 7;
const byte CH_JOYSTICK_X  = 8;
const byte CH_JOYSTICK_Y  = 9;

const byte SWITCH_1       = 1;
const byte SWITCH_2       = 2;
const byte SWITCH_3       = 3;
const byte SWITCH_4       = 4;
const byte SWITCH_5       = 5;
const byte SWITCH_6       = 6;

const byte SWITCH_UP    	= SWITCH_1;
const byte SWITCH_RIGHT 	= SWITCH_2;
const byte SWITCH_DOWN  	= SWITCH_3;
const byte SWITCH_LEFT 		= SWITCH_4;

const byte SWITCH_SELECT 	= SWITCH_5;
const byte SWITCH_START 	= SWITCH_6;

/*
 * These constants can be use for comparison with the value returned
 * by the readButton() method.
 */
const boolean PRESSED   = LOW;
const boolean RELEASED  = HIGH;


class _GoBLE {

private:
  unsigned int readChannel(byte channel);    
    
public:
  _GoBLE();
  
  unsigned int available();
  
  inline unsigned int readJoystickSwitch() { return readChannel(CH_JOYSTICK_SW); }

  inline int readJoystickX() { 
    return readChannel(CH_JOYSTICK_X) - 512;
  }
  inline int readJoystickY() {
    return readChannel(CH_JOYSTICK_Y) - 512;
  }

  
  /*
   * Reads the current state of a button. It will return
   * LOW if the button is pressed, and HIGH otherwise.
   */
  boolean readButton(byte channel);
  boolean readJoystickButton();
  
  void writeHP(byte hp);

};



extern _GoBLE GoBLE;

#endif // GOBLE_H_
