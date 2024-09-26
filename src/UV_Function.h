#include <Arduino.h>
#define UV_PIN A0

void setupUVModule(){
    pinMode(UV_PIN,INPUT);
}

byte getUVLevel(){
     return analogRead(UV_PIN);
}