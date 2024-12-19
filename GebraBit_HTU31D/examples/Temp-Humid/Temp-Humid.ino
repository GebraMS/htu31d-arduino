#include "GebraBit_HTU31D.h"

GebraBit_HTU31D HTU31D;

void setup() {
    Wire.begin();           // Initialize the I2C bus
    Serial.begin(9600);     // Initialize serial communication for debugging

    GB_HTU31D_initialize(&HTU31D); // Initialize the HTU31D sensor
    GB_HTU31D_Configuration(&HTU31D); // Configure the HTU31D sensor
}

void loop() {
    GB_HTU31D_Get_Data(&HTU31D); // Read data from the sensor

    Serial.print("Temperature: ");
    Serial.print(HTU31D.TEMPERATURE);
    Serial.println(" °C");

    Serial.print("Humidity: ");
    Serial.print(HTU31D.HUMIDITY);
    Serial.println(" %");

    delay(2000); // Delay between readings
}
