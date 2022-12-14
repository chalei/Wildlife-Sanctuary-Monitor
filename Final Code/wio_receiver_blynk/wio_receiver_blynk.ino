#include <Arduino.h>
#include "TFT_eSPI.h"
#include <SoftwareSerial.h>
#define BLYNK_PRINT Serial

#define BLYNK_TEMPLATE_ID " "
#define BLYNK_DEVICE_NAME " "
#define BLYNK_AUTH_TOKEN " "
// Comment this out to disable prints and save space
#include <rpcWiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleWioTerminal.h>

SoftwareSerial e5(D2, D3);
char ssid[] = " ";
char pass[] = " ";

char auth[] = " "; //put the value of BLYNK_AUTH_TOKEN here

BlynkTimer timer;


static char recv_buf[512];
static bool is_exist = false;

int img;
int res;
int temp;
int humi;
int tvoc;
int co2;

TFT_eSPI tft;
TFT_eSprite spr = TFT_eSprite(&tft);  //sprite

char* aqiresult[] = {"normal", "smoke", "wildfire", "anomaly"};
char* animalclass[] = {"anoa", "bekantan", "jalak", "komodo", "orangutan", "none"};
  
static int at_send_check_response(char *p_ack, int timeout_ms, char *p_cmd, ...)
{
    int ch = 0;
    int index = 0;
    int startMillis = 0;
    va_list args;
    memset(recv_buf, 0, sizeof(recv_buf));
    va_start(args, p_cmd);
    e5.printf(p_cmd, args);
    Serial.printf(p_cmd, args);
    va_end(args);
    delay(200);
    startMillis = millis();
 
    if (p_ack == NULL)
    {
        return 0;
    }
 
    do
    {
        while (e5.available() > 0)
        {
            ch = e5.read();
            recv_buf[index++] = ch;
            Serial.print((char)ch);
            delay(2);
        }
 
        if (strstr(recv_buf, p_ack) != NULL)
        {
            return 1;
        }
 
    } while (millis() - startMillis < timeout_ms);
    return 0;
}
 
static int recv_prase(void)
{
    char ch;
    int index = 0;
    memset(recv_buf, 0, sizeof(recv_buf));
    while (e5.available() > 0)
    {
        ch = e5.read();
        recv_buf[index++] = ch;
        Serial.print((char)ch);
        delay(2);
    }
 
    if (index)
    {
        char *p_start = NULL;
        char data[32] = {
            0,
        };
        int rssi = 0;
        int snr = 0;
 
        p_start = strstr(recv_buf, "+TEST: RX \"5345454544");
        if (p_start)
        {
            
            spr.fillSprite(TFT_BLACK);
            p_start = strstr(recv_buf, "5345454544");
            if (p_start && (1 == sscanf(p_start, "5345454544%s,", data)))
            {
                data[24] = 0;
              
                
                char *endptr;
                char *endptr1;

                char dataimg[5] = {data[0], data[1],data[2], data[3]};
                char datares[5] = {data[4], data[5], data[6], data[7]};
                char datatemp[5] = {data[8], data[9], data[10], data[11]};
                char datahumi[5] = {data[12], data[13],data[14], data[15]};
                char datatvoc[5] = {data[16], data[17], data[18], data[19]};
                char dataco2[5] = {data[20], data[21],data[22], data[23]};
                img = strtol(dataimg, &endptr, 16);
                res = strtol(datares, &endptr1, 16);
                temp = strtol(datatemp, &endptr, 16);
                humi = strtol(datahumi, &endptr1, 16);
                tvoc = strtol(datatvoc, &endptr, 16);
                co2 = strtol(dataco2, &endptr1, 16);
                
                spr.createSprite(100, 30);
                spr.setFreeFont(&FreeSansBoldOblique12pt7b);
                spr.setTextColor(TFT_WHITE);
                spr.drawString(animalclass[img], 0, 0, 1);
                spr.pushSprite(15, 100);
                spr.deleteSprite();
                spr.createSprite(150, 30);
                spr.setFreeFont(&FreeSansBoldOblique12pt7b);
                spr.setTextColor(TFT_WHITE);
                spr.drawString(aqiresult[res], 0, 0, 1);
                spr.pushSprite(170, 100);
                spr.deleteSprite();
                spr.createSprite(150, 30);
                spr.setFreeFont(&FreeSansBoldOblique12pt7b);
                spr.setTextColor(TFT_WHITE);
                spr.drawNumber(temp, 0 , 0 , 1);
                spr.pushSprite(180, 185);
                spr.deleteSprite();
                spr.createSprite(100, 30);
                spr.setFreeFont(&FreeSansBoldOblique12pt7b);
                spr.setTextColor(TFT_WHITE);
                spr.drawNumber(humi, 0, 0, 1);
                spr.pushSprite(15, 185);
                spr.deleteSprite();
                Blynk.virtualWrite(V0, aqiresult[res]);
                Blynk.virtualWrite(V1, animalclass[img]);
                Blynk.virtualWrite(V2, img);
                Blynk.virtualWrite(V3, res);
                Blynk.virtualWrite(V4, temp);
                Blynk.virtualWrite(V5, humi);
                Blynk.virtualWrite(V6, tvoc);
                Blynk.virtualWrite(V7, co2);
                Serial.println("all data Received: ");
                Serial.println(img);
                Serial.println(res);
                Serial.println(temp);
                Serial.println(humi);
                Serial.println(tvoc);
                Serial.println(co2);
                Serial.print("data received displaying on the wio terminal");
                Serial.print("\r\n");
                
            }
 
            p_start = strstr(recv_buf, "RSSI:");
            if (p_start && (1 == sscanf(p_start, "RSSI:%d,", &rssi)))
            {
                String newrssi = String(rssi);
          
                Serial.print(rssi);
                Serial.print("\r\n");

            }
            p_start = strstr(recv_buf, "SNR:");
            if (p_start && (1 == sscanf(p_start, "SNR:%d", &snr)))
            {
                Serial.print(snr);
                Serial.print("\r\n");

                
            }
            return 1;
        }
    }
    return 0;
}
 
static int node_recv(uint32_t timeout_ms)
{
    at_send_check_response("+TEST: RXLRPKT", 1000, "AT+TEST=RXLRPKT\r\n");
    int startMillis = millis();
    do
    {
        if (recv_prase())
        {
            return 1;
        }
    } while (millis() - startMillis < timeout_ms);
    return 0;
}

 
void setup(void)
{
 
  tft.begin();
  tft.setRotation(3);
  Serial.begin(115200);
    // while (!Serial);
  Serial.print("Receiver\r\n");
  e5.begin(9600);
  
  Blynk.begin(auth, ssid, pass);
//  timer.setInterval(2000L, sendSensor);
 
    
  tft.fillScreen(TFT_BLACK);
  tft.setFreeFont(&FreeSansBoldOblique12pt7b);
  tft.setTextColor(TFT_RED);
  tft.drawString("Img", 7 , 65 , 1);
  tft.drawString("Env", 165 , 65 , 1);


  
  tft.setFreeFont(&FreeSansBoldOblique12pt7b);
  tft.setTextColor(TFT_RED);
  tft.drawString("Humi", 7 , 150 , 1);
 

  tft.setFreeFont(&FreeSansBoldOblique12pt7b);
  tft.setTextColor(TFT_RED);
  tft.drawString("Temp:", 165 , 150 , 1);
  tft.setFreeFont(&FreeSansBoldOblique18pt7b);
  tft.setTextColor(TFT_WHITE);
  tft.fillRect(0, 0, 320, 50, TFT_DARKGREEN);
  tft.drawString("BLYNK Mode", 50, 10 , 1);
  tft.drawFastVLine(150, 50, 190, TFT_WHITE); //Drawing verticle line
  tft.drawFastHLine(0, 140, 320, TFT_WHITE); //Drawing horizontal line
    if (at_send_check_response("+AT: OK", 100, "AT\r\n"))
    {
        is_exist = true;
        at_send_check_response("+MODE: TEST", 1000, "AT+MODE=TEST\r\n");
        at_send_check_response("+TEST: RFCFG", 1000, "AT+TEST=RFCFG,866,SF12,125,12,15,14,ON,OFF,OFF\r\n");
        delay(200);
    }
    else
    {
        is_exist = false;
        Serial.print("No E5 module found.\r\n");
    }
}
 
void loop(void)
{
    if (is_exist)
    {

        Blynk.run();
        //timer.run();
        node_recv(2000);

    }
}
