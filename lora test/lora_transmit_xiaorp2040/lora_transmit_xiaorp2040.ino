#include <ArduinoJson.h>
#include<SoftwareSerial.h>
#include <Arduino.h>

#include "sensirion_common.h"
#include "sgp30.h"

SoftwareSerial e5(D1, D2);
int tvoc;
static char recv_buf[512];
static int led = 25;
int counter = 0;
static int at_send_check_response(char *p_ack, int timeout_ms, char *p_cmd, ...)
{
    int ch;
    int num = 0;
    int index = 0;
    int startMillis = 0;
    va_list args;
    memset(recv_buf, 0, sizeof(recv_buf));
    va_start(args, p_cmd);
    e5.print(p_cmd);
    Serial.print(p_cmd);
    va_end(args);
    delay(200);
    startMillis = millis();
 
    if (p_ack == NULL)
        return 0;
 
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
            return 1;
 
    } while (millis() - startMillis < timeout_ms);
    Serial.println();
    return 0;
}
 
void setup(void)
{
    s16 err;
    u16 scaled_ethanol_signal, scaled_h2_signal;
    Serial.begin(9600);
    pinMode(led, OUTPUT);
    digitalWrite(led, LOW);
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
    err = sgp_iaq_init();
    //
    e5.begin(9600);
    Serial.print("E5 LOCAL TEST\r\n");
    at_send_check_response("+AT: OK", 100, "AT\r\n");
    at_send_check_response("+MODE: TEST", 1000, "AT+MODE=TEST\r\n");
    delay(200);
    digitalWrite(led, HIGH);
}
 
void loop(void)
{
      
      s16 err = 0;
      u16 tvoc_ppb, co2_eq_ppm;
      err = sgp_measure_iaq_blocking_read(&tvoc_ppb, &co2_eq_ppm);counter=counter+1;
      if (err == STATUS_OK) {
        Serial.print("tVOC  Concentration:");
        Serial.print(tvoc_ppb);
        Serial.println("ppb");

        Serial.print("CO2eq Concentration:");
        Serial.print(co2_eq_ppm);
        Serial.println("ppm");
      char cmd[128];
      char tvoc[5];
      // Transmit HEX Value
      //sprintf(tvoc, "%04x", tvoc_ppb);
      sprintf(cmd, "AT+TEST=TXLRPKT,\"%d\"\r\n", tvoc_ppb);
      int ret = at_send_check_response("+TEST: TXLRPKT", 5000, cmd);
      
      if (ret)
        Serial.println("Sent");
      
      else
        Serial.println("Send failed!\r\n\r\n");
        
      delay(5000);
      }
      
      else{
        Serial.println("error reading IAQ values\n");
      }
}
