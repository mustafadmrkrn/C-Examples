/*****************************************************************
File:             BM25S2021-1.h
Author:           BEST MODULES CORP.
Description:      Define classes and required variables
Version:          V1.0.2   -- 2025-06-30
******************************************************************/
#ifndef BM25S2021_1_h
#define BM25S2021_1_h
#include <Wire.h>
#include <Arduino.h>

/*return status*/
#define READ_OK         0
#define READ_ERROR      1
#define ERROR_NOACK     2
#define ERROR_TIMEOUT   3
#define ERROR_CHECKSUM  4

/*IIC Address*/
const uint8_t BM25S2021_1_ADDR = 0x5C;

class BM25S2021_1
{ 
  public:
   BM25S2021_1(TwoWire *theWire);       //I2C
   BM25S2021_1(uint8_t datapin = 8); //One Wire
   void begin();

   float readTemperature(bool isFahrenheit = false);
   float readHumidity();
   uint8_t  getFWVer();
   uint16_t getPID();
   uint32_t getSN();

   private:
   uint16_t readRawTemperature();
   uint16_t readRawHumidity();
   void clearBuff();
   
   /*IIC*/
   void writeBytes(uint8_t wbuf[], uint8_t wlen);
   uint8_t readBytes(uint8_t rbuf[], uint8_t rlen);
   TwoWire *_theWire;
   uint8_t cheackCRC(uint8_t *buff,uint8_t len);
   uint8_t dataBuff[12]={0};
   
   /*One Wire*/
   uint8_t readOneWireData(); //start、ACK、Data、STOP
   int checkSum();
   uint8_t  oneWirePin;
   uint8_t bytes[5]={0};   //Array for storing data
   
   /*other*/
   uint16_t  _pid;
   uint8_t   _ver;
   uint32_t  _sn;
   uint8_t t_Flag = 0;
   uint8_t h_Flag = 0;
   uint8_t cnt = 7;
   uint8_t idx = 0;
   uint16_t loopCnt;
};


#endif
