#include <Arduino.h>
#include"TFT_eSPI.h"
#include<SoftwareSerial.h>

SoftwareSerial e5(0, 1);
#define NODE_SLAVE

static char recv_buf[512];
static bool is_exist = false;
  TFT_eSPI tft;
  TFT_eSprite spr = TFT_eSprite(&tft);  //sprite
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
                data[8] = 0;
                int x;
                int y;
                char *endptr;
                char *endptr1;
                char datatvoc[3] = { data[1],data[2], data[3]};
                char dataco2[4] = {data[4], data[5],data[6], data[7]};
                x = strtol(datatvoc, &endptr, 16);
                y = strtol(dataco2, &endptr1, 16);
                Serial.println(datatvoc);
                Serial.println(dataco2);
                Serial.println(x);
                Serial.println(y);
                
                spr.createSprite(100, 30);
                spr.setFreeFont(&FreeSansBoldOblique12pt7b);
                spr.setTextColor(TFT_WHITE);
                spr.drawNumber(x, 0, 0, 1);
                spr.pushSprite(15, 100);
                spr.deleteSprite();
                spr.createSprite(150, 30);
                spr.setFreeFont(&FreeSansBoldOblique12pt7b);
                spr.setTextColor(TFT_WHITE);
                spr.drawNumber(y, 0, 0, 1);
                spr.pushSprite(150, 100);
                spr.deleteSprite();
            
                
                Serial.print(data);
                Serial.print("\r\n");
                
            }
 
            p_start = strstr(recv_buf, "RSSI:");
            if (p_start && (1 == sscanf(p_start, "RSSI:%d,", &rssi)))
            {
                String newrssi = String(rssi);
          
                Serial.print(rssi);
                Serial.print("\r\n");
                spr.createSprite(150, 30);
                spr.setFreeFont(&FreeSansBoldOblique12pt7b);
                spr.setTextColor(TFT_WHITE);
                spr.drawString(newrssi, 0 , 0 , 1);
                spr.pushSprite(180, 185);
                spr.deleteSprite();
            }
            p_start = strstr(recv_buf, "SNR:");
            if (p_start && (1 == sscanf(p_start, "SNR:%d", &snr)))
            {
           
                spr.createSprite(100, 30);
                spr.setFreeFont(&FreeSansBoldOblique12pt7b);
                spr.setTextColor(TFT_WHITE);
                spr.drawNumber(snr, 0, 0, 1);
                spr.pushSprite(15, 185);
                spr.deleteSprite();
                
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
 
static int node_send(void)
{
    static uint16_t count = 0;
    int ret = 0;
    char data[32];
    char cmd[128];
 
    memset(data, 0, sizeof(data));
    sprintf(data, "%04X", count);
    sprintf(cmd, "AT+TEST=TXLRPKT,\"5345454544%s\"\r\n", data);
 

    ret = at_send_check_response("TX DONE", 2000, cmd);
    if (ret == 1)
    {
 
        count++;
        Serial.print("Sent successfully!\r\n");
    }
    else
    {
        Serial.print("Send failed!\r\n");
    }
    return ret;
}
 
static void node_recv_then_send(uint32_t timeout)
{
    int ret = 0;
    ret = node_recv(timeout);
    delay(100);
    if (!ret)
    {
        Serial.print("\r\n");
        return;
    }
    node_send();
    Serial.print("\r\n");
}
 
static void node_send_then_recv(uint32_t timeout)
{
    int ret = 0;
    ret = node_send();
    if (!ret)
    {
        Serial.print("\r\n");
        return;
    }
    if (!node_recv(timeout))
    {
        Serial.print("recv timeout!\r\n");
    }
    Serial.print("\r\n");
}
 
void setup(void)
{
 
    tft.begin();
    tft.setRotation(3);
    Serial.begin(115200);
    // while (!Serial);
 
    e5.begin(9600);
    Serial.print("ping pong communication!\r\n");
    
   tft.fillScreen(TFT_BLACK);

  tft.setFreeFont(&FreeSansBoldOblique12pt7b);
  tft.setTextColor(TFT_RED);
  tft.drawString("TVoC", 7 , 65 , 1);
  tft.drawString("CO2", 165 , 65 , 1);


  
  tft.setFreeFont(&FreeSansBoldOblique12pt7b);
  tft.setTextColor(TFT_RED);
  tft.drawString("SNR", 7 , 150 , 1);
 

  tft.setFreeFont(&FreeSansBoldOblique12pt7b);
  tft.setTextColor(TFT_RED);
  tft.drawString("RSSI:", 165 , 150 , 1);
    if (at_send_check_response("+AT: OK", 100, "AT\r\n"))
    {
        is_exist = true;
        at_send_check_response("+MODE: TEST", 1000, "AT+MODE=TEST\r\n");
        at_send_check_response("+TEST: RFCFG", 1000, "AT+TEST=RFCFG,866,SF12,125,12,15,14,ON,OFF,OFF\r\n");
        delay(200);
#ifdef NODE_SLAVE

          tft.setFreeFont(&FreeSansBoldOblique18pt7b);
          tft.setTextColor(TFT_WHITE);
          tft.drawString("Slave", 50, 10 , 1);
  
#else

          tft.setFreeFont(&FreeSansBoldOblique18pt7b);
          tft.setTextColor(TFT_WHITE);
          tft.drawString("Master", 50, 10 , 1);
#endif
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
#ifdef NODE_SLAVE
        node_recv_then_send(2000);
#else
        node_send_then_recv(2000);
        delay(3000);
#endif
    }
}
