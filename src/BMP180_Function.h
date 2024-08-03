#include <Adafruit_BMP085.h>
#include <Adafruit_I2CDevice.h>

Adafruit_BMP085 bmp;

void setupBMP180(){
    bmp.begin();
}

float getPressure(){
    return bmp.readPressure();
}

float getAltitude(){
    return bmp.readAltitude();
}