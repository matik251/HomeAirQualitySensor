#include <Wire.h> 
#include "SSD1306Wire.h"

#define AIR_PARTICLES_GOOD 35
#define AIR_PARTICLES_MEDIUM 75
#define AIR_PARTICLES_BAD 110

#define LOOP_DELAY 300000 //5 min

#define BYTE_COUNT 23
#define BUFF_START_CHAR1 0x42
#define BUFF_START_CHAR2 0x4d

unsigned char buffer [BYTE_COUNT];
int PM25 = 20, PM10 = 20;

int valuePM25(unsigned char *thebuf);
int valuePM10(unsigned char *thebuf);
char checkIfStringIsValid(unsigned char *thebuf, char leng);

// Initialize the OLED display using Wire library
SSD1306Wire  display(0x3c, D3, D5);

void setup(){
    Serial.begin(9600); 
    //LED green
    pinMode(D1, OUTPUT);
    //LED yellow
    pinMode(D2, OUTPUT);
    //LED red
    pinMode(D4, OUTPUT);
    digitalWrite(D1, HIGH);
    digitalWrite(D2, HIGH);
    digitalWrite(D4, HIGH);
    //pms3003 standby pin
    pinMode(D6, OUTPUT);
    //display setup
    display.init();  
    display.flipScreenVertically();
    display.setFont(ArialMT_Plain_24);
    display.setTextAlignment(TEXT_ALIGN_LEFT);
}
 
void loop(){
  wakePMS();
  delay(1000);
  measureAverage();
  switchLedState(PM25);
  showOnDisplay(PM25, PM10);
  sleepPMS();
  delay(LOOP_DELAY);
}

void measureAverage()
{
  showOnDisplayIsMeasuring(PM25, PM10);
  PM25 = 0;
  PM10 = 0;
  for(int i=0; i<20; i++)
  {
    if(Serial.find(BUFF_START_CHAR1))
    {
      Serial.readBytes(buffer,BYTE_COUNT);        
    }
    if(buffer[0] == BUFF_START_CHAR2)
    {
      if(checkIfStringIsValid(buffer,BYTE_COUNT))
      {  
        PM25 = (PM25 + valuePM25(buffer))/2;
        PM10 = (PM10 + valuePM10(buffer))/2;
      }
    }
    delay(500);
  }
}

void wakePMS()
{
    digitalWrite(D6, HIGH);
}

void sleepPMS()
{
    digitalWrite(D6, LOW);
}

void showOnDisplayIsMeasuring(int val25, int val10){
  display.clear();
  String tekst25 = "2.5: ";
  String tekst10 = "10: ";
  String pm25s = tekst25 + val25;  
  String pm10s = tekst10 + val10;
  display.drawStringMaxWidth(0, 0, 128, pm25s);
  display.drawStringMaxWidth(0, 26, 128, pm10s);
  display.drawStringMaxWidth(0, 52, 128, "+POMIAR+");
  display.display();
  
}

void showOnDisplay(int val25, int val10)
{
  display.clear();
  String tekst25 = "2.5: ";
  String tekst10 = "10: ";
  String pm25s = tekst25 + val25;  
  String pm10s = tekst10 + val10;
  display.drawStringMaxWidth(0, 0, 128, pm25s);
  display.drawStringMaxWidth(0, 26, 128, pm10s);
  display.display();
}

void switchLedState(int pmValue)
{
  if(PM25 < AIR_PARTICLES_MEDIUM){
    digitalWrite(D1, LOW);
    digitalWrite(D2, HIGH);
    digitalWrite(D4, HIGH);
  }else if(PM25 < AIR_PARTICLES_BAD){
    digitalWrite(D1, HIGH);
    digitalWrite(D2, LOW);
    digitalWrite(D4, HIGH);
  }else{
    digitalWrite(D1, HIGH);
    digitalWrite(D2, HIGH);
    digitalWrite(D4, LOW);    
  }
}

int valuePM1(unsigned char *buf)
{
  int PM1v;
  PM1v=((buf[15]<<8) + buf[16]); 
  return PM1v;
}

int valuePM25(unsigned char *buf)
{
  int PM25v;
  PM25v=((buf[11]<<8) + buf[12]);  
  return PM25v;
}

int valuePM10(unsigned char *buf)
{
  int PM10v;
  PM10v=((buf[13]<<8) + buf[14]); 
  return PM10v;
}

bool checkIfStringIsValid(unsigned char *buf, int length) 
{  
  bool flag=0;
  int sum=0;

  for(int i=0; i<(length-2); i++){
  sum+=buf[i];
  }
 sum=sum + 0x42;
 
  if(sum == ((buf[length-2]<<8)+buf[length-1]))  
  {
    sum = 0;
    flag = 1;
  }
  return flag;
}
