// SPI communication used to communicate with LCD module
#include <SPI.h>

// Inport related functionalities
#include "Temp_Humi_Functions.h"  // DHT22 sensor functionalities
#include "LCD_Function.h"

// Variable that handle the state of the system
byte currentState;

// Indicate first loop
bool isFirstEnter = false;

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

  // currentState = Logo_Display;
  // drawLogo(lcd,DELAY_BETWEEN_DISPLAY_PAGE);
  // resetScreen(lcd);

  isFirstEnter = true;
}

// Variable for record previous values
float previousValue1 = -1.00;
float previousValue2 = -1.00;

// Timer for Page Changing
unsigned long pageChangingTimer = millis();
// Timer for Data Changing
unsigned long dataChangingTimer = millis();

bool isChangingPage = false;

// Loop
void loop() {

  if(isFirstEnter){
    currentState = Temperature_and_Humidity_Display;
    isFirstEnter = false;
    drawLabels(lcd, Temperature, Left, "TEMPERATURE");
    drawLabels(lcd, Humidity, Right, "HUMIDITY");
  }

  // Variable for current value
  float currentValue1;
  float currentValue2;

  // get current time
  unsigned long currentTime = millis();

  // if(pageChangingTimer - currentTime >= DELAY_BETWEEN_DISPLAY_PAGE){
  //   //reset memory
  //   previousValue1 = -1.00;
  //   previousValue2 = -1.00;
  //   pageChangingTimer = currentTime;
  //   dataChangingTimer = currentTime;
  //   isChangingPage = true;
  // }

  // if(isChangingPage){
  //   if(currentState == Logo_Display){
  //     currentState = GPS_Display;
  //     resetScreen(lcd);
  //   }else if(currentState == Temperature_and_Humidity_Display){
  //     drawLabels(lcd, Left, "Temperature");
  //     drawLabels(lcd, Right, "Humidity");
  //   }else if(currentState == UV_Display){
  //     drawLabels(lcd, Left, "UV Level");
  //   }
  //   isChangingPage = false;
  // }

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
      for(byte i = 0; i < 20; i++){
        drawIndicator(lcd, UV_Level, Left, i/1.00);
        if(i > 10){
          drawAlert(lcd);
        }else{
          lcd.setOrientation(3);
          lcd.fillRectangle(125,15,200,90,COLOR_BLACK);
        }
        delay(500);
      }
    }
    dataChangingTimer = currentTime;
  }
}