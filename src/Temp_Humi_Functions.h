// Include sensor functionalities
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

// define SIGNAL output pin from sensor module
#define DHTPIN 2

// define type of sensor
#define DHTTYPE DHT22

// Initialize DHT22 instance
DHT_Unified dht(DHTPIN, DHTTYPE);

// Setup DHT22 sensor
void DHT22Setup(){
    // Start DHT22 sensor
    dht.begin();
    // sensor_t sensor;
}

float getTemperature(){
    sensors_event_t event;
    dht.temperature().getEvent(&event);
    return event.temperature;
}

float getHumidity(){
    sensors_event_t event;
    dht.humidity().getEvent(&event);
    return event.relative_humidity;
}