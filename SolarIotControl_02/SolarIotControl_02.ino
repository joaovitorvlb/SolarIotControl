#include <stdio.h>
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <Adafruit_ADS1015.h>
#include "RTClib.h"

#include "MyDevices.h"      //cOMENT 

#define VOLTAGE_MAX      24.0
#define ADC_MAXCOUNTS    17130.0

#define LD1       12
#define RL1       15
#define RL2       13

#define S1        0
#define S2        2
#define S3        14

int status = WL_IDLE_STATUS;
WiFiClient  client;

const char* host      = ip_oran_l_1;
const int httpPort    = 500;

char ssid[]           = SSID_02;         // your network SSID (name) 
char pass[]           = PSK_02;          // your network password

String buff;

String StrIr0;
String StrIr1;
String StrIr2;
String StrIr3;

int TimeSock          = 0;
int TimeAdc           = 0;

int CtAdMs            = 0;
int CtAdMseg          = 0;

int ano               = 0;
int mes               = 0;
int dia               = 0;
int horr              = 0;
int minn              = 0;
int segg              = 0;
char flag             = 0;
char flag_sckt        = 0;

int32_t AcumAdc0;
int32_t AcumAdc1;
int32_t AcumAdc2;
int32_t AcumAdc3;

int32_t  acumvc0      = 0;
int32_t  acumvc1      = 0;
int32_t  acumvc2      = 0;
int32_t  acumvc3      = 0;
float    vresult0     = 0;
float    vresult1     = 0;
float    vresult2     = 0;
float    vresult3     = 0;



RTC_DS1307 rtc;
char daysOfTheWeek[7][12] = {"domingo", "segunda", "terça", "quarta", "quinta", "sexta", "sábado"};

Adafruit_ADS1115 ads;

void timer0_ISR (void) 
{
  TimeSock++;
  TimeAdc++;
  timer0_write(ESP.getCycleCount() + 80000);      //Starts timer again from nodemcu to 1ms
}

void setup(void) 
{
  Serial.begin(9600);

  Serial.println();
  Serial.print("conectando no ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, pass);
  
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi conectado");  
  Serial.println("Endereço de IP: ");
  Serial.println(WiFi.localIP());

  ads.begin();
  
  if (! rtc.begin()) 
  {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  rtc.adjust(DateTime(2014, 1, 22, 3, 2, 3));

  pinMode(LD1, OUTPUT);
  pinMode(RL1, OUTPUT);
  pinMode(RL2, OUTPUT);

  pinMode(S1, INPUT);
  pinMode(S2, INPUT);
  pinMode(S3, INPUT);

  digitalWrite(LD1, HIGH);
  digitalWrite(RL1, HIGH);
  digitalWrite(RL2, HIGH);

  noInterrupts();
  timer0_isr_init();
  timer0_attachInterrupt(timer0_ISR);
  timer0_write(ESP.getCycleCount() + 80000);     //1ms Interruption of 1ms in nodemcu
  interrupts();
}

void loop(void) 
{
 //------------Rotina de envio via socket------------------------------
  if (flag_sckt == 1)
  {
    flag_sckt = 0; 
    char cnt = 0;
    if (client.connected() == 0) 
    {
      while (client.connect(host, httpPort) == 0) 
      {
        cnt++;
        Serial.print(":");
        delay(500);
        if (cnt >= 4)
        {
          Serial.println("  --> Pacote perdido! <--  ");
          break; 
        }
      }

      if (client.connected() == 1)
      {
        Serial.println("Conectado ao servidor");
        buff = "ESP02-VVVV:" + StrIr0 + ":" + StrIr1 + ":" + StrIr2 + ":" + StrIr3;
        client.print(buff);
        client.stop();
        Serial.println("Pacote enviado!");
        Serial.println("Conecao encerrada");
      }
    }
  }
//----------Rotina do rtc------------------------------
  DateTime now = rtc.now();

  horr = (now.hour());
  minn = (now.minute());
  segg = (now.second());

  delay(500);

  //----------Rotina de varrredura adc-----------------

  if (TimeAdc >= 250)
  {
    TimeAdc = 0;
    CtAdMs++;
    AcumAdc0 = AcumAdc0 + ads.readADC_SingleEnded(0);
    AcumAdc1 = AcumAdc1 + ads.readADC_SingleEnded(1);
    AcumAdc2 = AcumAdc2 + ads.readADC_SingleEnded(2);
    AcumAdc3 = AcumAdc3 + ads.readADC_SingleEnded(3);
  }
 
  if((segg == 0) && (flag == 0))
  {
    flag = 10;
    acumvc0 = acumvc0 + (AcumAdc0 / CtAdMs);
    acumvc1 = acumvc1 + (AcumAdc1 / CtAdMs);
    acumvc2 = acumvc2 + (AcumAdc2 / CtAdMs);
    acumvc3 = acumvc3 + (AcumAdc3 / CtAdMs);

    CtAdMs = 0;
    AcumAdc0 = 0;
    AcumAdc1 = 0;
    AcumAdc2 = 0;
    AcumAdc3 = 0;

    Serial.print(" (");
    Serial.print(horr);
    Serial.print(':');
    Serial.print(minn);
    Serial.print(':');
    Serial.print(segg);
    Serial.print(")--");

    Serial.print(" VC-0: "); Serial.print(acumvc0);
    Serial.print(" VC-1: "); Serial.print(acumvc1);
    Serial.print(" VC-3: "); Serial.print(acumvc2);
    Serial.print(" VC-3: "); Serial.println(acumvc3);
  }
  
  if((segg == 10) && (flag == 10))
  {
    flag = 20;
    acumvc0 = acumvc0 + (AcumAdc0 / CtAdMs);
    acumvc1 = acumvc1 + (AcumAdc1 / CtAdMs);
    acumvc2 = acumvc2 + (AcumAdc2 / CtAdMs);
    acumvc3 = acumvc3 + (AcumAdc3 / CtAdMs);

    CtAdMs = 0;
    AcumAdc0 = 0;
    AcumAdc1 = 0;
    AcumAdc2 = 0;
    AcumAdc3 = 0;

    Serial.print(" (");
    Serial.print(horr);
    Serial.print(':');
    Serial.print(minn);
    Serial.print(':');
    Serial.print(segg);
    Serial.print(")--");

    Serial.print(" VC-0: "); Serial.print(acumvc0);
    Serial.print(" VC-1: "); Serial.print(acumvc1);
    Serial.print(" VC-3: "); Serial.print(acumvc2);
    Serial.print(" VC-3: "); Serial.println(acumvc3);
  }


  if((segg == 20) && (flag == 20))
  {
    flag = 30;
    acumvc0 = acumvc0 + (AcumAdc0 / CtAdMs);
    acumvc1 = acumvc1 + (AcumAdc1 / CtAdMs);
    acumvc2 = acumvc2 + (AcumAdc2 / CtAdMs);
    acumvc3 = acumvc3 + (AcumAdc3 / CtAdMs);

    CtAdMs = 0;
    AcumAdc0 = 0;
    AcumAdc1 = 0;
    AcumAdc2 = 0;
    AcumAdc3 = 0;

    Serial.print(" (");
    Serial.print(horr);
    Serial.print(':');
    Serial.print(minn);
    Serial.print(':');
    Serial.print(segg);
    Serial.print(")--");

    Serial.print(" VC-0: "); Serial.print(acumvc0);
    Serial.print(" VC-1: "); Serial.print(acumvc1);
    Serial.print(" VC-3: "); Serial.print(acumvc2);
    Serial.print(" VC-3: "); Serial.println(acumvc3);
  }

  if((segg == 30) && (flag == 30))
  {
    flag = 40;
    acumvc0 = acumvc0 + (AcumAdc0 / CtAdMs);
    acumvc1 = acumvc1 + (AcumAdc1 / CtAdMs);
    acumvc2 = acumvc2 + (AcumAdc2 / CtAdMs);
    acumvc3 = acumvc3 + (AcumAdc3 / CtAdMs);

    CtAdMs = 0;
    AcumAdc0 = 0;
    AcumAdc1 = 0;
    AcumAdc2 = 0;
    AcumAdc3 = 0;

    Serial.print(" (");
    Serial.print(horr);
    Serial.print(':');
    Serial.print(minn);
    Serial.print(':');
    Serial.print(segg);
    Serial.print(")--");

    Serial.print(" VC-0: "); Serial.print(acumvc0);
    Serial.print(" VC-1: "); Serial.print(acumvc1);
    Serial.print(" VC-3: "); Serial.print(acumvc2);
    Serial.print(" VC-3: "); Serial.println(acumvc3);
  }

  if((segg == 40) && (flag == 40))
  {
    flag = 50;
    acumvc0 = acumvc0 + (AcumAdc0 / CtAdMs);
    acumvc1 = acumvc1 + (AcumAdc1 / CtAdMs);
    acumvc2 = acumvc2 + (AcumAdc2 / CtAdMs);
    acumvc3 = acumvc3 + (AcumAdc3 / CtAdMs);

    CtAdMs = 0;
    AcumAdc0 = 0;
    AcumAdc1 = 0;
    AcumAdc2 = 0;
    AcumAdc3 = 0;

    Serial.print(" (");
    Serial.print(horr);
    Serial.print(':');
    Serial.print(minn);
    Serial.print(':');
    Serial.print(segg);
    Serial.print(")--");

    Serial.print(" VC-0: "); Serial.print(acumvc0);
    Serial.print(" VC-1: "); Serial.print(acumvc1);
    Serial.print(" VC-3: "); Serial.print(acumvc2);
    Serial.print(" VC-3: "); Serial.println(acumvc3);
  }

  if((segg == 50) && (flag == 50))
  {  
    flag = 0;
    acumvc0 = acumvc0 + (AcumAdc0 / CtAdMs);
    acumvc1 = acumvc1 + (AcumAdc1 / CtAdMs);
    acumvc2 = acumvc2 + (AcumAdc2 / CtAdMs);
    acumvc3 = acumvc3 + (AcumAdc3 / CtAdMs);

    CtAdMs = 0;
    AcumAdc0 = 0;
    AcumAdc1 = 0;
    AcumAdc2 = 0;
    AcumAdc3 = 0;

    Serial.print(" (");
    Serial.print(horr);
    Serial.print(':');
    Serial.print(minn);
    Serial.print(':');
    Serial.print(segg);
    Serial.print(")--");

    Serial.print(" VC-0: "); Serial.print(acumvc0);
    Serial.print(" VC-1: "); Serial.print(acumvc1);
    Serial.print(" VC-3: "); Serial.print(acumvc2);
    Serial.print(" VC-3: "); Serial.println(acumvc3);

    vresult0 = ((acumvc0 / 6) * (VOLTAGE_MAX / ADC_MAXCOUNTS));
    vresult1 = ((acumvc1 / 6) * (VOLTAGE_MAX / ADC_MAXCOUNTS));
    vresult2 = ((acumvc2 / 6) * (VOLTAGE_MAX / ADC_MAXCOUNTS));
    vresult3 = ((acumvc3 / 6) * (VOLTAGE_MAX / ADC_MAXCOUNTS));

    acumvc0 = 0;
    acumvc1 = 0;
    acumvc2 = 0;
    acumvc3 = 0;

    StrIr0 = String(vresult0, 2);
    StrIr1 = String(vresult1, 2);
    StrIr2 = String(vresult2, 2);
    StrIr3 = String(vresult3, 2);

    flag_sckt = 1;
  }
}





