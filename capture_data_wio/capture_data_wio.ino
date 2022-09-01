
#include <Arduino.h>
#include <SensirionI2CSht4x.h>
#include <Wire.h>
#include "TFT_eSPI.h"
#include "sensirion_common.h"
#include "sgp30.h"
#include <SPI.h>
#include <Seeed_FS.h>
#include "SD/Seeed_SD.h"
TFT_eSPI tft;
 
File myFile;

SensirionI2CSht4x sht4x;
TFT_eSprite spr = TFT_eSprite(&tft);

void setup() {
    s16 err;
    u16 scaled_ethanol_signal, scaled_h2_signal;

    Serial.begin(115200);
    Serial.print("Initializing SD card...");
    if (!SD.begin(SDCARD_SS_PIN, SDCARD_SPI)) {
        Serial.println("initialization failed!");
    while (1);
    }
  Serial.println("initialization done.");
 
   tft.begin();
  tft.setRotation(3);
 // tft.fillScreen(background_color);
  tft.fillScreen(TFT_BLACK);
  tft.setFreeFont(&FreeSansBoldOblique18pt7b);
  tft.setTextColor(TFT_WHITE);
  tft.drawString("SD Capture", 50, 10 , 1);

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
  tft.drawString("Tvoc:", 165 , 65 , 1);

  tft.setFreeFont(&FreeSansBoldOblique12pt7b);
  tft.setTextColor(TFT_RED);
  tft.drawString("Capture:", 165 , 150 , 1);
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
    while (sgp_probe() != STATUS_OK) {
        Serial.println("SGP failed");
        while (1);
    }
 
  pinMode(WIO_KEY_A, INPUT_PULLUP);
  pinMode(WIO_KEY_B, INPUT_PULLUP);
  pinMode(WIO_KEY_C, INPUT_PULLUP);   
}

void loop() {
    uint16_t error;
    char errorMessage[256];

    delay(1000);
    s16 err = 0;
    u16 tvoc_ppb, co2_eq_ppm;
    err = sgp_measure_iaq_blocking_read(&tvoc_ppb, &co2_eq_ppm);
    float temperature;
    float humidity;
    error = sht4x.measureHighPrecision(temperature, humidity);
    if (err == STATUS_OK) {
        Serial.print("tVOC  Concentration:");
        Serial.print(tvoc_ppb);
        Serial.println("ppb");

        Serial.print("CO2eq Concentration:");
        Serial.print(co2_eq_ppm);
        Serial.println("ppm");
        spr.createSprite(40, 30);
        spr.fillSprite(TFT_BLACK);
        spr.setFreeFont(&FreeSansBoldOblique12pt7b);
        spr.setTextColor(TFT_WHITE);
        spr.drawNumber(tvoc_ppb, 0, 0, 1);
        spr.pushSprite(165, 100);
        spr.deleteSprite();
    }
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
    }
   
    
  if(digitalRead(WIO_KEY_A) == LOW){
    myFile = SD.open("aqi.csv", FILE_APPEND);
 
  // if the file opened okay, write to it:
  if (myFile) {
    Serial.print("Writing to test.txt...");
    String data_record = String(temperature) + "," + String(humidity) + "," + String(tvoc_ppb) + "," + 0;
    // Append the data record:
    
    myFile.println(data_record);
            spr.createSprite(100, 30);
            spr.setFreeFont(&FreeSansBoldOblique12pt7b);
            spr.setTextColor(TFT_WHITE);
            spr.drawString("writing", 0 , 0 , 1);
            spr.pushSprite(165, 185);
            spr.deleteSprite();
    // close the file:
    myFile.close();
    Serial.println("done data 0.");
    
            spr.createSprite(100, 30);
            spr.setFreeFont(&FreeSansBoldOblique12pt7b);
            spr.setTextColor(TFT_GREEN);
            spr.drawString("bckgrnd", 0 , 0 , 1);
            spr.pushSprite(165, 185);
            spr.deleteSprite();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
  }
  // save_data_to_SD_Card("0");
  if(digitalRead(WIO_KEY_B) == LOW){
    myFile = SD.open("aqi.csv", FILE_APPEND);
 
  // if the file opened okay, write to it:
  if (myFile) {
    Serial.print("Writing to test.txt...");
    String data_record = String(temperature) + "," + String(humidity) + "," + String(tvoc_ppb) + "," + 1;
    myFile.println(data_record);
            spr.createSprite(100, 30);
            spr.setFreeFont(&FreeSansBoldOblique12pt7b);
            spr.setTextColor(TFT_WHITE);
            spr.drawString("writing", 0 , 0 , 1);
            spr.pushSprite(165, 185);
            spr.deleteSprite();
    // close the file:
    myFile.close();
    Serial.println("done data 1.");
    
            spr.createSprite(100, 30);
            spr.setFreeFont(&FreeSansBoldOblique12pt7b);
            spr.setTextColor(TFT_YELLOW);
            spr.drawString("fire", 0 , 0 , 1);
            spr.pushSprite(165, 185);
            spr.deleteSprite();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  } 
  }
  if(digitalRead(WIO_KEY_C) == LOW){
    myFile = SD.open("aqi.csv", FILE_APPEND);
 
  // if the file opened okay, write to it:
  if (myFile) {
    Serial.print("Writing to test.txt...");
    String data_record = String(temperature) + "," + String(humidity) + "," + String(tvoc_ppb) + "," + 2;
    // Append the data record:
    
   myFile.println(data_record);
            spr.createSprite(100, 30);
            spr.setFreeFont(&FreeSansBoldOblique12pt7b);
            spr.setTextColor(TFT_WHITE);
            spr.drawString("writing", 0 , 0 , 1);
            spr.pushSprite(165, 185);
            spr.deleteSprite();
    // close the file:
    myFile.close();
    Serial.println("done data 0.");
    
            spr.createSprite(100, 30);
            spr.setFreeFont(&FreeSansBoldOblique12pt7b);
            spr.setTextColor(TFT_WHITE);
            spr.drawString("smoke", 0 , 0 , 1);
            spr.pushSprite(165, 185);
            spr.deleteSprite();
    
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  } 
  }

}
