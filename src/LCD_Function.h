// Input the LCD display library
#include "TFT_22_ILI9225.h"

// Pictures and the callout funtions
#include "IEEE_logo.h"
#include "Alert_symbol.h"
#include "Raining_symbol.h"
#include <UV_Function.h>

// Pin for LCD module
#define lcd_RST 8
#define lcd_RS  9
#define lcd_CS  53  // SS
#define lcd_SDI 51  // MOSI
#define lcd_CLK 52  // SCK
#define lcd_LED 3   // 0 if wired to +5V directly

#define Buzzer_PIN 4

// set 5 seconds buffer for each page changing
#define DELAY_BETWEEN_DISPLAY_PAGE 15000
#define DELAY_BETWEEN_EACH_SET_DATA 500

// Initialize lcd instance from LCD display library (to access to the library functionalities)
TFT_22_ILI9225 lcd = TFT_22_ILI9225(lcd_RST, lcd_RS, lcd_CS, lcd_LED);

// reset the screen and set the rotation of the display
void resetScreen(TFT_22_ILI9225 lcd){
    lcd.clear();
    lcd.setOrientation(3);
}

// Constants
// Temperature
const float TEMPERATURE_UPPER_LIMIT = 34.0;
const float TEMPERATURE_LOWER_LIMIT = 27.5;
// Humidity
const float HUMIDITY_UPPER_LIMIT = 75.0;
const float HUMIDITY_LOWER_LIMIT = 67.5;
// UV Level
const float UV_LEVEL_UPPER_LIMIT = 11.0;
const float UV_LEVEL_LOWER_LIMIT = 0.0;

// Functions related to Meters

// Position for the meters
enum Position{
  Left,
  Right
}Position; 

enum Type{
  Temperature,
  Humidity,
  UV_Level,
  Pressure,
  Altitude
}Type;

enum Page{
  Logo_Display,
  Temperature_and_Humidity_Display,
  UV_Display,
  Raining_Display,
  Pressure_and_Altitude_Display
}Page;

void setupBuzzer(){
    pinMode(Buzzer_PIN,OUTPUT);
    digitalWrite(Buzzer_PIN,HIGH);
}

void singleBeeps(){
    digitalWrite(Buzzer_PIN,LOW);
    delay(500);
    digitalWrite(Buzzer_PIN,HIGH);
}

void drawLabels(TFT_22_ILI9225 lcd, enum Type type, enum Position pos, String name){

    lcd.setOrientation(3);

    float lowerLimit, upperLimit;
    if(type == Temperature){
        lowerLimit = TEMPERATURE_LOWER_LIMIT;
        upperLimit = TEMPERATURE_UPPER_LIMIT;
    }else if(type == Humidity){
        lowerLimit = HUMIDITY_LOWER_LIMIT;
        upperLimit = HUMIDITY_UPPER_LIMIT;
    }else if(type == UV_Level){
        lowerLimit = UV_LEVEL_LOWER_LIMIT;
        upperLimit = UV_LEVEL_UPPER_LIMIT;
    }

    lcd.setFont(Terminal6x8);
        
    byte leftBoundary;
    if(pos == Left){
        leftBoundary = 40;
        if(type == Temperature){
            lcd.drawText(15, 5, name, COLOR_WHITE);
        }else if(type == UV_Level){
            lcd.drawText(20, 5, name, COLOR_WHITE);
        }
    }else if(pos == Right){
        leftBoundary = 155;
        lcd.drawLine(110,10,110,165,COLOR_WHITE);
        lcd.drawText(140, 5, name, COLOR_WHITE);
    }

    byte yUpperLimit = 30;
    for(byte levels = 1; levels <= 10; levels++){
        if(levels == 1 || levels == 10){
            if(levels == 1){
                lcd.drawText(leftBoundary - 30, 27, String(upperLimit,1), COLOR_RED);
                lcd.drawLine(leftBoundary,yUpperLimit,leftBoundary+25,yUpperLimit,COLOR_RED);
            }else if(levels == 10){
                if(type == UV_Level){
                    lcd.drawText(leftBoundary - 30, 115, String(lowerLimit,1), COLOR_GREEN);
                    lcd.drawLine(leftBoundary,yUpperLimit,leftBoundary+25,yUpperLimit,COLOR_GREEN);
                }else{
                    lcd.drawText(leftBoundary - 30, 115, String(lowerLimit,1), COLOR_RED);
                    lcd.drawLine(leftBoundary,yUpperLimit,leftBoundary+25,yUpperLimit,COLOR_RED);
                }
            }
            
        }else{
            lcd.drawLine(leftBoundary,yUpperLimit,leftBoundary+15,yUpperLimit,COLOR_GREEN);
        }
        yUpperLimit += 10;
    }
}

void drawIndicator(TFT_22_ILI9225 lcd, enum Type type, enum Position pos, float value){

    lcd.setOrientation(3);

    float lowerLimit, upperLimit;
    if(type == Temperature){
        lowerLimit = TEMPERATURE_LOWER_LIMIT;
        upperLimit = TEMPERATURE_UPPER_LIMIT;
    }else if(type == Humidity){
        lowerLimit = HUMIDITY_LOWER_LIMIT;
        upperLimit = HUMIDITY_UPPER_LIMIT;
    }else if(type == UV_Level){
        lowerLimit = UV_LEVEL_LOWER_LIMIT;
        upperLimit = UV_LEVEL_UPPER_LIMIT;
    }

    lcd.setFont(Terminal12x16);
    byte xStartingPosition;
    byte textYPosition = 135;
    if(pos == Left){
        xStartingPosition = 70;
        lcd.fillRectangle(45 - 2,textYPosition - 2,45 + 60, 150 + 2, COLOR_BLACK);
        if(value >= upperLimit || value <= lowerLimit){
            if(type == UV_Level && value == lowerLimit){
                lcd.drawText(45, textYPosition, String(value,1),COLOR_GREEN);
            }else{
                lcd.drawText(45, textYPosition, String(value,1),COLOR_RED);
            }
            
        }else{
            lcd.drawText(45, textYPosition, String(value,1),COLOR_GREEN);
        }
    }else if(pos == Right){
        xStartingPosition = 185;
        lcd.fillRectangle(160 - 2,textYPosition - 2,160 + 60, 150 + 2, COLOR_BLACK);
        if(value >= upperLimit || value <= lowerLimit){
            lcd.drawText(160, textYPosition, String(value,1),COLOR_RED);
        }else{
            lcd.drawText(160, textYPosition, String(value,1),COLOR_GREEN);
        }
    }
    byte needleWidth = xStartingPosition + 5;

    byte yPosition;
    if(value > upperLimit){
        singleBeeps();
        yPosition = 30;
    }else if(value < lowerLimit){
        singleBeeps();
        yPosition = 120;
    }else{
        yPosition = ((upperLimit - value) / (upperLimit - lowerLimit) * (30 - 120) - 30) * -1;
    }
    lcd.fillRectangle(xStartingPosition,30-5,needleWidth,120+5,COLOR_BLACK);
    lcd.fillTriangle(xStartingPosition,yPosition,needleWidth,yPosition + 5,needleWidth,yPosition - 5,COLOR_WHITE);
}

// Pressure and Altitude Interface
void drawPressureAndAltitudeLabels(TFT_22_ILI9225 lcd){
    lcd.setOrientation(3);
    lcd.setFont(Terminal11x16);
    lcd.drawText(25,10,"Pressure",COLOR_WHITE);
    lcd.drawText(25,85,"Altitude",COLOR_WHITE);
}

void drawPressureAndAltitudeValue(TFT_22_ILI9225 lcd, enum Type type, float values){
    lcd.setOrientation(3);
    lcd.setFont(Terminal12x16);
    if(type == Pressure){
        lcd.drawText(120,45,String(values,0),COLOR_WHITE);
    }else if(type == Altitude){
        lcd.drawText(120,120,String(values,0),COLOR_WHITE);
    }
}