#include "GoBLE.h"
#include <Arduino.h>

_GoBLE GoBLE;

/*
 * The following constants tell, for each accelerometer
 * axis, which values are returned when the axis measures
 * zero acceleration.
 */ 

_GoBLE::_GoBLE() {
  Serial.begin(115200);
}

unsigned int _GoBLE::available(){
  return true;
}


unsigned int _GoBLE::readChannel(byte channel) {
  
  
  
}


boolean _GoBLE::readButton(byte ch) {
  
  
  
}

boolean _GoBLE::readJoystickButton() {
  
  
}

