#include <Arduino.h>
#include "esp32_snir.h"
#include <SSD1306.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Keypad.h>
#include <Adafruit_NeoPixel.h>

Adafruit_NeoPixel pixels(NB_PIXELS, DATALEDS, NEO_RGB);
void setup()
{
pixels.begin();
}
void loop()
{
pixels.clear();
pixels.show();
delay(500);
for(int i = 0 ; i < NB_PIXELS ; i++)
{
pixels.setPixelColor(i,16,0,0); // numLed, R, V, B
pixels.show();
delay(500);
}
}
