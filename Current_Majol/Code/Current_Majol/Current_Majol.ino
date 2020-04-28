/*
 * Amere meter(AC) Majol and commiunication with SPI and IC ACS712-20A || ACS712-30A && ATMEGA8
 * direct bye : Hamed Sargoli
 * version : 00.04
 * Company: IHCO
 * Date : 1399/02/08
*/
#include <avr/wdt.h>
#include <Filters.h>                       
#include <SPI.h>
#include "Classes.h"

#define D_Send_To_Serial true                                                 //for send data to serial port OR not send 
#define D_ACS_Pin A0                                                          //Sensor data pin on A0 analog input
#define D_time_Update_data 500
#define D_time_Calculate 50


//declerate variable and class
char Buffer[5];
char index = 0;
bool Flag_SPI_Send = false;
Times CALCULATE_AMPER;
Times SEND;
Ampermeter AMP;

void setup() 
{
  pinMode(D_ACS_Pin,INPUT);                                                     //Define the pin mode
  if(D_Send_To_Serial)
    Serial.begin(115200); 
  
  //Setting for SPI
  pinMode(MISO, OUTPUT); 
  SPCR |= _BV(SPE); 
  index = 0;                                                                  // buffer empty
  SPI.attachInterrupt();                                                      //intrrupt Enable
}

void loop() 
{
  wdt_enable(WDTO_500MS);                                                     // enable the watchdog
  CALCULATE_AMPER.ontime();
  SEND.ontime();
  
  RunningStatistics inputStats;                                               // create statistics to look at the raw test signal
  inputStats.setWindowSecs( AMP.windowLength );                               //Set the window length
  
  while(true)
  {  
    wdt_reset();                                                              // uncomment to avoid reboot 
    AMP.ACS_Value = analogRead(D_ACS_Pin);                                    // read the analog in value
    inputStats.input(AMP.ACS_Value);                                          // log to Stats function
        
    if(CALCULATE_AMPER.timediff() > D_time_Calculate)
    {
      CALCULATE_AMPER.ontime();                                               // update time
      AMP.TRMS = AMP.intercept + AMP.slope_ACS_20A * inputStats.sigma();
    }
 
    if(SEND.timediff() > D_time_Update_data)                                  //change time send by define change
    {                                                   
      sprintf(Buffer,"%4d",int(AMP.TRMS*100));                                //change AMPERE to String
      for(int i=0 ; i < strlen(Buffer) ; i++)
        if(Buffer[i]==' ')
          Buffer[i]='0';
      if(D_Send_To_Serial)                                                    //if for sned data to serial or not
      {
        Serial.print("Amper*100 = ");
        Serial.println(Buffer);  
      }
      SEND.ontime();                                                          // update time send
    }
  }
}
      
// SPI interrupt routine
ISR (SPI_STC_vect)  
{ 
  byte c = SPDR;
  if (c == '<')                                                               //Header for start Send
    Flag_SPI_Send = true;
  if (c == '>')                                                               //Footer for Stop Send
  {
    index = 0;
    Flag_SPI_Send = false;
  }
  if(Flag_SPI_Send)
  {
    SPDR = Buffer[index];
    index++;
  }
}
