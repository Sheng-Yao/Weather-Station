#include <Arduino.h>
#define UV_PIN A0

void setupUVModule(){
    pinMode(UV_PIN,INPUT);
}

byte getUVLevel(){
     return map(analogRead(UV_PIN), 0, 1024, 1, 10);
}