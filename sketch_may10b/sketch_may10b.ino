#include <Keyboard.h>

#define accel 13
#define drift 12
#define boost 11

void setup() {
  pinMode(accel, INPUT_PULLUP);
  pinMode(drift, INPUT_PULLUP);
  pinMode(boost, INPUT_PULLUP);

//  kEYBOARD.BEGIN();
  Keyboard.begin();
}

void loop() {
//  Serial.println(digitalRead(11));
  if(!digitalRead(accel)){
    Keyboard.press(0xDA); 
  }
  else{
    Keyboard.release(0xDA);
  }

  if(!digitalRead(drift)){
    Keyboard.press(0x81); 
  }
  else{
    Keyboard.release(0x81);
  }

  if(!digitalRead(boost)){
    Keyboard.press(0x80); 
  }
  else{
    Keyboard.release(0x80);
  }
}
