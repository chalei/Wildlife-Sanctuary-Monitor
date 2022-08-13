#include "Seeed_Arduino_GroveAI.h"
#include <Wire.h>
#include <TFT_eSPI.h> 
GroveAI ai(Wire);
uint8_t state = 0;
TFT_eSPI tft;
TFT_eSprite spr = TFT_eSprite(&tft);  //sprite

void setup()
{
  tft.begin();
    tft.setRotation(3);
  Wire.begin();
  Serial.begin(115200);
 
  Serial.println("begin");
  if (ai.begin(ALGO_OBJECT_DETECTION, MODEL_EXT_INDEX_1)) // Object detection and pre-trained model 1
  {
    Serial.print("Version: ");
    Serial.println(ai.version());
    Serial.print("ID: ");
    Serial.println( ai.id());
    Serial.print("Algo: ");
    Serial.println( ai.algo());
    Serial.print("Model: ");
    Serial.println(ai.model());
    Serial.print("Confidence: ");
    Serial.println(ai.confidence());
    state = 1;
  }
  else
  {
    Serial.println("Algo begin failed.");
  }
   tft.fillScreen(TFT_BLACK);
  tft.setFreeFont(&FreeSansBoldOblique18pt7b);
  tft.setTextColor(TFT_WHITE);
  tft.drawString("AI Demo", 50, 10 , 1);

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
  tft.drawString("Total", 7 , 65 , 1);
  tft.setTextColor(TFT_GREEN);
 

  //CO Text
  tft.setFreeFont(&FreeSansBoldOblique12pt7b);
  tft.setTextColor(TFT_RED);
  tft.drawString("Conf", 7 , 150 , 1);
  tft.setTextColor(TFT_GREEN);
  tft.drawString("%", 55, 193, 1);

  //level
  tft.setFreeFont(&FreeSansBoldOblique12pt7b);
  tft.setTextColor(TFT_YELLOW);
  tft.drawString("Result:", 165 , 120 , 1);
}
 
void loop()
{
  if (state == 1)
  {
    uint32_t tick = millis();
    if (ai.invoke()) // begin invoke
    {
      while (1) // wait for invoking finished
      {
        CMD_STATE_T ret = ai.state(); 
        if (ret == CMD_STATE_IDLE)
        {
          break;
        }
        delay(20);
      }
 
     uint8_t len = ai.get_result_len(); // receive how many people detect
     if(len)
     {
       int time1 = millis() - tick; 
       Serial.print("Time consuming: ");
       Serial.println(time1);
       Serial.print("Number of people: ");
       Serial.println(len);
       object_detection_t data;       //get data
 
       for (int i = 0; i < len; i++)
       {
          Serial.println("result:detected");
          Serial.print("Detecting and calculating: ");
          Serial.println(i+1);
          ai.get_result(i, (uint8_t*)&data, sizeof(object_detection_t)); //get result
 
          Serial.print("confidence:");
          Serial.print(data.confidence);
          Serial.println();
        }
          spr.createSprite(40, 30);
        spr.fillSprite(TFT_BLACK);
        spr.setFreeFont(&FreeSansBoldOblique12pt7b);
        spr.setTextColor(TFT_WHITE);
        spr.drawNumber(len, 0, 0, 1);
        spr.pushSprite(15, 100);
        spr.deleteSprite();
        spr.createSprite(40, 30);
        spr.setFreeFont(&FreeSansBoldOblique12pt7b);
        spr.setTextColor(TFT_WHITE);
        spr.drawNumber(data.confidence, 0, 0, 1);
        spr.setTextColor(TFT_GREEN);
        spr.pushSprite(15, 185);
        spr.deleteSprite();
        spr.createSprite(120, 30);
        spr.setFreeFont(&FreeSansBoldOblique12pt7b);
        spr.setTextColor(TFT_GREEN);
        if(data.target == 0){
            spr.drawString("Anoa", 0 , 0 , 1);
            spr.pushSprite(150, 150);
            spr.deleteSprite();
        }
         else if(data.target == 1){
            spr.drawString("Bekantan", 0 , 0 , 1);
            spr.pushSprite(150, 150);
            spr.deleteSprite();
        }
        else if(data.target == 2){
            spr.drawString("Jalak", 0 , 0 , 1);
            spr.pushSprite(150, 150);
            spr.deleteSprite();
        }
        else if(data.target == 3){
            spr.drawString("Komodo", 0 , 0 , 1);
            spr.pushSprite(150, 150);
            spr.deleteSprite();
        }
        else if(data.target == 4){
            spr.drawString("Orgutan", 0 , 0 , 1);
            spr.pushSprite(150, 150);
            spr.deleteSprite();
        }
     }
     else
     {
       Serial.println("No identification");
            spr.createSprite(40, 30);
        spr.fillSprite(TFT_BLACK);
        spr.setFreeFont(&FreeSansBoldOblique12pt7b);
        spr.setTextColor(TFT_WHITE);
        spr.drawNumber(0, 0, 0, 1);
        spr.pushSprite(15, 100);
        spr.deleteSprite();
        spr.createSprite(40, 30);
        spr.setFreeFont(&FreeSansBoldOblique12pt7b);
        spr.setTextColor(TFT_WHITE);
        spr.drawNumber(0, 0, 0, 1);
        spr.setTextColor(TFT_GREEN);
        spr.pushSprite(15, 185);
        spr.deleteSprite();
        spr.createSprite(120, 30);
        spr.setFreeFont(&FreeSansBoldOblique12pt7b);
            spr.setTextColor(TFT_RED);
            spr.drawString("empty", 0 , 0 , 1);
            spr.pushSprite(150, 150);
            spr.deleteSprite();
     }
     
    }
    else
    {
      delay(1000);
      Serial.println("Invoke Failed.");
    }
  }
  else
  {
    state == 0;
  }
}
