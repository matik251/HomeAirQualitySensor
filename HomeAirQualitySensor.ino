#include <Wire.h> 
#include "SSD1306Wire.h"

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
    pinMode(D6, OUTPUT);
    //LED yellow
    pinMode(D7, OUTPUT);
    //LED red
    pinMode(D8, OUTPUT);
    //display setup
    display.init();  
    display.flipScreenVertically();
    display.setFont(ArialMT_Plain_24);
    display.setTextAlignment(TEXT_ALIGN_LEFT);
}
 
void loop(){
  if(Serial.find(BUFF_START_CHAR1))
  {
    Serial.readBytes(buffer,BYTE_COUNT);        
  }

   if(buffer[0] == BUFF_START_CHAR2)
   {
    if(checkIfStringIsValid(buffer,BYTE_COUNT))
    {  
      PM25=valuePM25(buffer);
      PM10=valuePM10(buffer);  
    }
  }
  switchLedState(PM25);
  showOnDsiplay(PM25, PM10)
}


void showOnDsiplay(int val25, int val10)
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
  if(PM25 < 20){
    digitalWrite(D6, LOW);
    digitalWrite(D7, HIGH);
    digitalWrite(D8, HIGH);
  }else if(PM25 < 30){
    digitalWrite(D6, HIGH);
    digitalWrite(D7, LOW);
    digitalWrite(D8, HIGH);
    
  }else{
    digitalWrite(D6, HIGH);
    digitalWrite(D7, HIGH);
    digitalWrite(D8, LOW);    
  }
}

int valuePM1(unsigned char *buf)
{
  int PM1v;
  PM10v=((buf[15]<<8) + buf[16]); 
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
