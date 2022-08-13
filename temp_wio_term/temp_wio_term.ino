#include <Arduino.h>
#include <SensirionI2CSht4x.h>
#include <Wire.h>
#include <TFT_eSPI.h>
SensirionI2CSht4x sht4x;
 
TFT_eSPI tft;
TFT_eSprite spr = TFT_eSprite(&tft);  //sprite


void setup() {
 
    Serial.begin(115200);
     tft.begin();
    tft.setRotation(3);
  
 
    Wire.begin();
 
    uint16_t error;
    char errorMessage[256];
 
    sht4x.begin(Wire);
 
    uint32_t serialNumber;
    error = sht4x.serialNumber(serialNumber);
    if (error) {
        Serial.print("Error trying to execute serialNumber(): ");
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
    } else {
        Serial.print("Serial Number: ");
        Serial.println(serialNumber);
    }
   tft.fillScreen(TFT_BLACK);
  tft.setFreeFont(&FreeSansBoldOblique18pt7b);
  tft.setTextColor(TFT_WHITE);
  tft.drawString("Suhu Ruang", 50, 10 , 1);

  //Line
  for (int8_t line_index = 0; line_index < 5 ; line_index++)
  {
    tft.drawLine(0, 50 + line_index, tft.width(), 50 + line_index, TFT_GREEN);
  }


  //VCO & CO Rect
  tft.drawRoundRect(5, 60, (tft.width() / 2) - 20 , tft.height() - 65 , 10, TFT_WHITE); // L1

  //VCO Text
  tft.setFreeFont(&FreeSansBoldOblique12pt7b);
  tft.setTextColor(TFT_RED);
  tft.drawString("Temp", 7 , 65 , 1);
  tft.setTextColor(TFT_GREEN);
  tft.drawString("C", 55, 108, 1);

  //CO Text
  tft.setFreeFont(&FreeSansBoldOblique12pt7b);
  tft.setTextColor(TFT_RED);
  tft.drawString("Hum", 7 , 150 , 1);
  tft.setTextColor(TFT_GREEN);
  tft.drawString("%", 55, 193, 1);

  //level
  tft.setFreeFont(&FreeSansBoldOblique12pt7b);
  tft.setTextColor(TFT_RED);
  tft.drawString("Level:", 165 , 120 , 1);
}
 
void loop() {
    uint16_t error;
    char errorMessage[256];
 
    delay(1000);
 
    float temperature;
    float humidity;
    error = sht4x.measureHighPrecision(temperature, humidity);
    if (error) {
        Serial.print("Error trying to execute measureHighPrecision(): ");
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
    } else {
        Serial.print("Temperature:");
        Serial.print(temperature);
        Serial.print("\t");
        Serial.print("Humidity:");
        Serial.println(humidity);
        spr.createSprite(40, 30);
        spr.fillSprite(TFT_BLACK);
        spr.setFreeFont(&FreeSansBoldOblique12pt7b);
        spr.setTextColor(TFT_WHITE);
        spr.drawNumber(temperature, 0, 0, 1);
        spr.pushSprite(15, 100);
        spr.deleteSprite();
        spr.createSprite(40, 30);
        spr.setFreeFont(&FreeSansBoldOblique12pt7b);
        spr.setTextColor(TFT_WHITE);
        spr.drawNumber(humidity, 0, 0, 1);
        spr.setTextColor(TFT_GREEN);
        spr.pushSprite(15, 185);
        spr.deleteSprite();
        if (temperature > 30 ){
            spr.createSprite(100, 30);
            spr.setFreeFont(&FreeSansBoldOblique12pt7b);
            spr.setTextColor(TFT_RED);
            spr.drawString("anget", 0 , 0 , 1);
            spr.pushSprite(180, 150);
            spr.deleteSprite();
        }
        else{
            spr.createSprite(100, 30);
            spr.setFreeFont(&FreeSansBoldOblique12pt7b);
            spr.setTextColor(TFT_GREEN);
            spr.drawString("Normal", 0 , 0 , 1);
            spr.pushSprite(180, 150);
            spr.deleteSprite();
    }
  }
}
