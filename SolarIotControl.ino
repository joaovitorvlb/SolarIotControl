#include <ESP8266WiFi.h>
#include <Wire.h>
#include <Adafruit_ADS1015.h>
#include "RTClib.h"

const char* host = "192.168.1.112";
const int httpPort = 500;


char ssid[] = "IOT-2";               // your network SSID (name) 
char pass[] = "mj110032";            // your network password

char c ; 

char ct = 0;

int status = WL_IDLE_STATUS;
WiFiClient  client;

#define VOLTAGE_MAX 24.0
#define CURRENT_MAX 1200.0
#define ADC_MAXCOUNTS 17130.0

#define LD1   12
#define RL1   15
#define RL2   13

#define S1    0
#define S2    2
#define S3    14

int cont = 0;
int ano = 0;
int mes = 0;
int dia = 0;
int horr = 0;
int minn = 0;
int segg = 0;
int flag = 0;
float acumvcc = 0;
float acumicc = 0;
float vresult = 0;
float iresult = 0;

RTC_DS1307 rtc;
char daysOfTheWeek[7][12] = {"domingo", "segunda", "terça", "quarta", "quinta", "sexta", "sábado"};

Adafruit_ADS1115 ads;

void timer0_ISR (void) 
{
  cont++;
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
  if (cont >= 10000)
  {
    cont = 0; 
    Serial.println(ct);
    ct++;
    if (client.connected() == 0) 
    {
      while (client.connect(host, httpPort) == 0) 
      {
        delay(500);
        Serial.print(":");
      }

      if (client.connected() == 1)
      {
        Serial.println();
        Serial.println("Conectado ao servidor");

        client.print(ct);
        c = client.read();
        Serial.println(c);
        client.stop();
        Serial.println("Conecao encerrada");
      }
    }
    
  }
//----------Rotina do rtc------------------------------
  DateTime now = rtc.now();

  
  //Serial.print(now.year(), DEC);
 // Serial.print('/');
  //Serial.print(now.month(), DEC);
  //Serial.print('/');
  //Serial.print(now.day(), DEC);
  //Serial.print(" (");
  //Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);

  horr = (now.hour());
  minn = (now.minute());
  segg = (now.second());
  
  //Serial.print(") ");
  //Serial.print(horr);
  //Serial.print(':');
 // Serial.print(minn);
 // Serial.print(':');
 // Serial.print(segg);
 // Serial.println();
  
  //Serial.println();
  delay(500);

  int16_t adc0, adc1, adc2, adc3;

  adc0 = ads.readADC_SingleEnded(0);
  adc1 = ads.readADC_SingleEnded(1);
  adc2 = ads.readADC_SingleEnded(2);
  adc3 = ads.readADC_SingleEnded(3);

  float voltage = adc0 * (VOLTAGE_MAX / ADC_MAXCOUNTS);
  float current = adc1 * (CURRENT_MAX / ADC_MAXCOUNTS);

  //Serial.print("vcc: "); Serial.print(voltage);
  //Serial.print("  icc: "); Serial.println(current);
 
  if((segg == 0) && (flag == 0))
  {
    flag = 10;
    acumvcc = acumvcc + voltage;
    acumicc = acumicc + current;
  }
  
  if((segg == 10) && (flag == 10))
  {
    flag = 20;
    acumvcc = acumvcc + voltage;
    acumicc = acumicc + current;
  }

  if((segg == 20) && (flag == 20))
  {
    flag = 30;
    acumvcc = acumvcc + voltage;
    acumicc = acumicc + current;
  }

  if((segg == 30) && (flag == 30))
  {
    flag = 40;
    acumvcc = acumvcc + voltage;
    acumicc = acumicc + current;
  }

  if((segg == 40) && (flag == 40))
  {
    flag = 50;
    acumvcc = acumvcc + voltage;
    acumicc = acumicc + current;
  }

  if((segg == 50) && (flag == 50))
  {
    flag = 0;
    acumvcc = acumvcc + voltage;
    acumicc = acumicc + current;
    vresult = acumvcc / 6;
    iresult = acumicc / 6;
    acumvcc = 0;
    acumicc = 0;
    Serial.print("vcc: "); Serial.println(vresult);
    Serial.print("icc: "); Serial.println(iresult);
    Serial.println();
  }
}





