// SPI communication used to communicate with LCD module
#include <SPI.h>

// Inport related functionalities
#include "Temp_Humi_Functions.h"  // DHT22 sensor functionalities
#include "LCD_Function.h"
#include "BMP180_Function.h"

#define Rain_INPUT 3

// Variable that handle the state of the system
byte currentState;

// Setup
void setup() {

  //Setup Serial monitor for debug purpose
  Serial.begin(9600);

  // Initialize LCD module
  lcd.begin();
  //Reset to default settings for LCD module
  resetScreen(lcd);

  // Initialize DHT22 sensor
  DHT22Setup();

  // Pin Setup for Raining sensor
  pinMode(Rain_INPUT,INPUT);

  setupBuzzer();

  setupBMP180();

  currentState = Logo_Display;
  drawLogo(lcd,DELAY_BETWEEN_DISPLAY_PAGE);
  resetScreen(lcd);
}

// Variable for record previous values
float previousValue1 = -1.00;
float previousValue2 = -1.00;

// Timer for Page Changing
unsigned long pageChangingTimer = millis();
// Timer for Data Changing
unsigned long dataChangingTimer = millis();

bool isChangingPage = true;

byte previousState = -1;
bool isRaining = false;

// Loop
void loop() {

  // Variable for current value
  float currentValue1;
  float currentValue2;

  // get current time
  unsigned long currentTime = millis();

  if(currentTime - pageChangingTimer >= DELAY_BETWEEN_DISPLAY_PAGE){
    //reset memory
    previousValue1 = -1.00;
    previousValue2 = -1.00;
    pageChangingTimer = currentTime;
    dataChangingTimer = currentTime;
    isChangingPage = true;
  }

  if(isChangingPage){
    resetScreen(lcd);
    if(currentState == Temperature_and_Humidity_Display){
      currentState = UV_Display;
      drawLabels(lcd, UV_Level, Left, "UV Level");
    }else if(currentState == UV_Display){
      if(digitalRead(Rain_INPUT) == LOW){
        currentState = Raining_Display;
        drawRaining(lcd);
        singleBeeps();
      }else{
        currentState = Pressure_and_Altitude_Display;
        drawPressureAndAltitudeLabels(lcd);
      }
    }else if(currentState == Raining_Display){
      currentState = Pressure_and_Altitude_Display;
      drawPressureAndAltitudeLabels(lcd);
    }else if(currentState == Pressure_and_Altitude_Display || currentState == Logo_Display){
      currentState = Temperature_and_Humidity_Display;
      drawLabels(lcd, Temperature, Left, "TEMPERATURE");
      drawLabels(lcd, Humidity, Right, "HUMIDITY");
    }
    isChangingPage = false;
  }else{
    // Page data updating
    if(currentTime - dataChangingTimer >= DELAY_BETWEEN_EACH_SET_DATA){
      if(currentState == Temperature_and_Humidity_Display){

        currentValue1 = getTemperature();
        if(currentValue1 != previousValue1){
          drawIndicator(lcd, Temperature, Left, currentValue1);
          previousValue1 = currentValue1;
        }

        currentValue2 = getHumidity();
        if(currentValue2 != previousValue2){
          drawIndicator(lcd, Humidity, Right, currentValue2);
          previousValue2 = currentValue2; 
        }

      }else if(currentState == UV_Display){
        //For simulation
        currentValue1 = getUVLevel();
        if(currentValue1 != previousValue1){
          drawIndicator(lcd, UV_Level, Left, currentValue1);
          if(currentValue1 > 10){
            drawAlert(lcd);
          }else{
            lcd.setOrientation(3);
            lcd.fillRectangle(125,15,200,90,COLOR_BLACK);
          }
          previousValue1 = currentValue1;
        }
      }else if(currentState == Pressure_and_Altitude_Display){
        currentValue1 = getPressure();
        currentValue2 = getAltitude();
        if(currentValue1 != previousValue1){
          lcd.fillRectangle(118, 43, 210, 62, COLOR_BLACK);
          drawPressureAndAltitudeValue(lcd, Pressure, currentValue1);
          previousValue1 = currentValue1;
        }

        if(currentValue2 != previousValue2){
          lcd.fillRectangle(118, 117, 210, 137, COLOR_BLACK);
          drawPressureAndAltitudeValue(lcd, Altitude, currentValue1);
          previousValue2 = currentValue2;
        }
      }
      dataChangingTimer = currentTime;
    }
  }
}