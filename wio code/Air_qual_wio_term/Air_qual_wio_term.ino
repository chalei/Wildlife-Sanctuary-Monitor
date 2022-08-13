#include <Arduino.h>
#include <TFT_eSPI.h>
#include "sensirion_common.h"
#include "sgp30.h"

TFT_eSPI tft;
TFT_eSprite spr = TFT_eSprite(&tft);  //sprite


void setup() {
    s16 err;
    u32 ah = 0;
    u16 scaled_ethanol_signal, scaled_h2_signal;
    Serial.begin(115200);
    tft.begin();
    tft.setRotation(3);
    Serial.println("serial start!!");
 
    /*  Init module,Reset all baseline,The initialization takes up to around 15 seconds, during which
        all APIs measuring IAQ(Indoor air quality ) output will not change.Default value is 400(ppm) for co2,0(ppb) for tvoc*/
    while (sgp_probe() != STATUS_OK) {
        Serial.println("SGP failed");
        while (1);
    }
    /*Read H2 and Ethanol signal in the way of blocking*/
    err = sgp_measure_signals_blocking_read(&scaled_ethanol_signal,
                                            &scaled_h2_signal);
    if (err == STATUS_OK) {
        Serial.println("get ram signal!");
    } else {
        Serial.println("error reading signals");
    }
 
    // Set absolute humidity to 13.000 g/m^3
    //It's just a test value
    sgp_set_absolute_humidity(13000);
    err = sgp_iaq_init();
    //Head
  tft.fillScreen(TFT_BLACK);
  tft.setFreeFont(&FreeSansBoldOblique18pt7b);
  tft.setTextColor(TFT_WHITE);
  tft.drawString("Kualitas Udara", 50, 10 , 1);

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
  tft.drawString("tVOC", 7 , 65 , 1);
  tft.setTextColor(TFT_GREEN);
  tft.drawString("ppb", 55, 108, 1);

  //CO Text
  tft.setFreeFont(&FreeSansBoldOblique12pt7b);
  tft.setTextColor(TFT_RED);
  tft.drawString("CO2", 7 , 150 , 1);
  tft.setTextColor(TFT_GREEN);
  tft.drawString("ppm", 55, 193, 1);

  //level
  tft.setFreeFont(&FreeSansBoldOblique12pt7b);
  tft.setTextColor(TFT_RED);
  tft.drawString("Level:", 165 , 120 , 1);


}
 
void loop() {
    s16 err = 0;
    u16 tvoc_ppb, co2_eq_ppm;
    err = sgp_measure_iaq_blocking_read(&tvoc_ppb, &co2_eq_ppm);
    if (err == STATUS_OK) {
        Serial.print("tVOC  Concentration:");
        Serial.print(tvoc_ppb);
        Serial.println("ppb");
         spr.createSprite(40, 30);
        spr.fillSprite(TFT_BLACK);
        spr.setFreeFont(&FreeSansBoldOblique12pt7b);
        spr.setTextColor(TFT_WHITE);
        spr.drawNumber(tvoc_ppb, 0, 0, 1);
        spr.pushSprite(15, 100);
        spr.deleteSprite();
        Serial.print("CO2eq Concentration:");
        Serial.print(co2_eq_ppm);
        Serial.println("ppm");
        spr.createSprite(40, 30);
        spr.setFreeFont(&FreeSansBoldOblique12pt7b);
        spr.setTextColor(TFT_WHITE);
        spr.drawNumber(co2_eq_ppm, 0, 0, 1);
        spr.setTextColor(TFT_GREEN);
        spr.pushSprite(15, 185);
        spr.deleteSprite();
        if (tvoc_ppb > 50 || co2_eq_ppm > 900){
            spr.createSprite(100, 30);
            spr.setFreeFont(&FreeSansBoldOblique12pt7b);
            spr.setTextColor(TFT_RED);
            spr.drawString("Bahaya", 0 , 0 , 1);
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
    } else {
        Serial.println("error reading IAQ values\n");
    }
    delay(1000);
}
