/*****************************************************************
File:          BM25S2021-1.cpp
Author:        BEST MODULES CORP.
Description:   The sensor with IIC and obtain the corresponding value
Version:       V1.0.2   -- 2025-06-30
******************************************************************/
#include "BM25S2021-1.h"

/**********************************************************
Description: Constructor
Parameters:  *twotheWire: &Wire or &Wire1 or &Wire2
Return:  None
Others:  None     
**********************************************************/
BM25S2021_1::BM25S2021_1(TwoWire *theWire)
{
   _theWire = theWire;
}
/**********************************************************
Description: Constructor
Parameters:  datapin: As a pin for transmitting data 
Return:  None
Others:  None    
**********************************************************/
BM25S2021_1::BM25S2021_1(uint8_t datapin)
{
   _theWire = NULL;
   oneWirePin = datapin;
}
/**********************************************************
Description: BM25S2021_1 initial
Parameters: void
Return:     void
Others:     None 
**********************************************************/
void BM25S2021_1::begin()
{
   if(_theWire != NULL)
   {
      _theWire->begin(); 
      _theWire->setClock(40000); //IIC rate = 40kHz
   } 
}

/**********************************************************
Description: readTemperature
Parameters:  isFahrenheit = true:Fahrenheit degree
             isFahrenheit = false:centigrade(default)
Return:      temperature , unit:℃（default） or  ℉ 
Others:      None
**********************************************************/
float BM25S2021_1::readTemperature(bool isFahrenheit)
{
    float temperature = 0;
    /****** if select IICMode  ******/
    if(_theWire != NULL)     
    {    
       uint8_t sendbuf[3] = {3,2,2};
       writeBytes(sendbuf, 0x03);
       delay(2);
       if(readBytes(dataBuff,0x06) == 0)
       {
          temperature = (dataBuff[2]<<8)|dataBuff[3];
          if(isFahrenheit == true)
          {
             temperature = temperature*0.18 + 32;  
          }
          else
          {
             temperature = temperature / 10;
          }
          return temperature;
       }
       else
       {
         return 0;//READ_ERROR
       }
    }
    /****** if select ONEWIRE  ******/
    else
    {
      if(h_Flag == 0)      //read temperature for the first time
      {
        t_Flag = 1;
        readOneWireData();  //read data
      }
      else
      {
        h_Flag = 0;
      }
      if(checkSum() == READ_OK)
      {
         temperature = (bytes[2]<<8) | bytes[3];
         if(isFahrenheit == true)
         {
            temperature = temperature*0.18 + 32;  
         }
         else temperature = temperature / 10;
         return temperature;//READ_OK
      }
      else return 0;//READ_ERROR
    }
}

/**********************************************************
Description: readHumidity
Parameters:  void
Return:      humidity
Others:      None
**********************************************************/
float BM25S2021_1::readHumidity()
{
    float humidity = 0;
    /****** if select IICMode  ******/
    if(_theWire != NULL)
    {
        uint8_t sendbuf[3] = {3,0,2};
        writeBytes(sendbuf, 0x03);
        delay(2);
        if(readBytes(dataBuff,0x06) == 0)
        {
          humidity = (dataBuff[2]<<8)|dataBuff[3];
          humidity = humidity / 10;
          return humidity; //READ_OK
        }
        else return 0;//READ_ERROR
    }
    /****** if select ONEWIRE  ******/
    else
    {
      if(t_Flag == 0)      //read humidity for the first time
      {
        h_Flag = 1;
        readOneWireData();  //read data
      }
      else
      {
        t_Flag = 0; 
      }
      if(checkSum() == READ_OK)
      {
         humidity = (bytes[0]<<8) | bytes[1];
         humidity = humidity / 10;            
         return humidity;//READ_OK
      }
      else return 0;//READ_ERROR
   }
}

/**********************************************************
Description: Get device Ver
Parameters:  void     
Return:      Ver       
Others:      None 
**********************************************************/
uint8_t BM25S2021_1::getFWVer()
{
    uint8_t sendbuf[3] = {3,8,7};
    writeBytes(sendbuf, 0x03);
    delay(2);
    if(readBytes(dataBuff,0x0b) == 0)
    {
       _ver = dataBuff[4];
       return _ver;
    }
    else return 0; //READ_ERROR
}

/**********************************************************
Description: Get device PID
Parameters:  void     
Return:      PID       
Others:      None
**********************************************************/
uint16_t BM25S2021_1::getPID()
{
    uint8_t sendbuf[3] = {3,8,7};
    writeBytes(sendbuf, 0x03);
    delay(2);
    if(readBytes(dataBuff,0x0b) == 0)
    {
       _pid = (dataBuff[2]<<8)|dataBuff[3];
       return _pid;
    }
    else return 0; //READ_ERROR
}

/**********************************************************
Description: Get device SN
Parameters:  void     
Return:      SN       
Others:      None
**********************************************************/
uint32_t BM25S2021_1::getSN()
{
    uint8_t sendbuf[3] = {3,8,7};
    writeBytes(sendbuf, 0x03);
    delay(2);
    if(readBytes(dataBuff,0x0b) == 0)
    {
       _sn  = ((long)dataBuff[5]<<24)|((long)dataBuff[6]<<16)|((long)dataBuff[7]<<8)|dataBuff[8];
       return _sn;
    }
    else return 0; //READ_ERROR
}

/**********************************************************
Description: read Temperature RawData
Parameters:  rawTemperature:Variables for storing temperature RawData
Return:      temperature Raw Data(uint16_t)       
Others:      None
**********************************************************/
uint16_t BM25S2021_1::readRawTemperature()
{
    uint16_t temperature = 0;
    /****** if select IICMode  ******/
    if(_theWire != NULL)     
    {    
       uint8_t sendbuf[3] = {3,2,2};
       writeBytes(sendbuf, 0x03);
       delay(2);
       if(readBytes(dataBuff,0x06) == 0)
       {
          temperature = (dataBuff[2]<<8)|dataBuff[3];
          return temperature;
       }
       else
       {
         return 0;//READ_ERROR
       }
    }
    /****** if select ONEWIRE  ******/
    else
    {
      if(h_Flag == 0)
      {
        t_Flag = 1;
        readOneWireData();
      }
      else
      {
        h_Flag = 0;
      }
      if(checkSum() == READ_OK)
      {
         temperature = (bytes[2]<<8) | bytes[3];
         return temperature;//READ_OK
      }
      else return 0;//READ_ERROR
    }
}

/**********************************************************
Description: read Humidity RawData
Parameters:  rawHumidity:Variables for storing humidity  Rawdata
Output:      humidity  Raw data(uint16_t)
Return:      READ_OK / READ_ERROR       
Others:      None
**********************************************************/
uint16_t BM25S2021_1::readRawHumidity()
{
    uint16_t humidity = 0;
    /****** if select IICMode  ******/
    if(_theWire != NULL)
    {
        uint8_t sendbuf[3] = {3,0,2};
        writeBytes(sendbuf, 0x03);
        delay(2);
        if(readBytes(dataBuff,0x06) == 0)
        {
          humidity = (dataBuff[2]<<8)|dataBuff[3];
          return humidity; //READ_OK
        }
        else return 0;//READ_ERROR
    }
    /****** if select ONEWIRE  ******/
    else
    {
      if(t_Flag == 0)
      {
        h_Flag = 1;
        readOneWireData();
      }
      else
      {
        t_Flag = 0;
      }
      if(checkSum() == READ_OK)
      {
         humidity = (bytes[0]<<8) | bytes[1];         
         return humidity;//READ_OK
      }
      else return 0;//READ_ERROR
   }
}

/**********************************************************
Description: Clear data buff
Parameters:  void
Return:    void 
Others:    None  
**********************************************************/
void BM25S2021_1::clearBuff()
{
  if(_theWire != NULL)
  {
      for(int a = 0; a < 12; a++)
      {
        dataBuff[a] = 0;
      } 
  }
  else
  {
      for(int a = 0; a < 5; a++)
      {
        bytes[a] = 0;
      } 
  }   
}

/**********************************************************
Description: writeData
Parameters:  wbuf:Variables for storing Data to be sent
             wlen:Length of data sent
Return:    void
Others:    None  
**********************************************************/
void BM25S2021_1::writeBytes(uint8_t wbuf[], uint8_t wlen)
{
    if(_theWire != NULL)
    {
      while (_theWire->available() > 0) //clear receive buff
      {
        _theWire->read();
      }
      _theWire->beginTransmission(BM25S2021_1_ADDR);
      _theWire->write(wbuf, wlen);
      _theWire->endTransmission();
    }
}
/**********************************************************
Description: readData
Parameters:  rbuf:Variables for storing Data to be obtained
             rlen:Length of data to be obtained
Return:      READ_OK/READ_ERROR  
Others:      None
**********************************************************/
uint8_t BM25S2021_1::readBytes(uint8_t rbuf[], uint8_t rlen)
{
    if(_theWire != NULL)
    {
       clearBuff();
       _theWire->requestFrom(BM25S2021_1_ADDR, rlen);
       for (uint8_t i = 0; i < rlen; i++)
       {
         rbuf[i] = _theWire->read();
       }
       /* Check Sum */
       if(rbuf[0] != 0x03)
       {
         return READ_ERROR;
       }
       if(cheackCRC(rbuf,rlen) == 0)
       {
         return READ_OK; //READ_OK
       }
    }
    return 0x00;
}

/**********************************************************
Description: CRC Correction
Parameters:  *buff：Data obtained through IIC
              len ：Gets the total length of the data
Return:      Checksum calculate by CRC-16
Others:      None
**********************************************************/
uint8_t BM25S2021_1::cheackCRC(uint8_t *buff,uint8_t len)
{
  uint16_t temp_crc=0xffff;
  uint8_t i;
  while(len--)
  {
    temp_crc ^= *buff++;
    for(i=0;i<8;i++)
    {
      if(temp_crc & 0x01)
      {
        temp_crc >>= 1;
        temp_crc ^= 0xa001;
      }
      else
      {
        temp_crc >>=1;
      }
    }
  }
  uint8_t temp = temp_crc;
  return temp;
}
/**********************************************************
Description: readOneWireData
Parameters:  void
Return:      READ_OK/ERROR_TIMEOUT
Others:      None
**********************************************************/
uint8_t BM25S2021_1::readOneWireData()
{
    /*****   start    ******/
     pinMode(oneWirePin, OUTPUT);
     digitalWrite(oneWirePin, LOW);
     delay(15);
     digitalWrite(oneWirePin, HIGH);
     pinMode(oneWirePin, INPUT);
     loopCnt = 10000;  
     while (digitalRead(oneWirePin) == HIGH)
     {
       delayMicroseconds(1);
       if (loopCnt-- == 0)
       {
         clearBuff();
         return ERROR_TIMEOUT;
       }
     }
     
    /*****   ACK    ******/
     pinMode(oneWirePin, INPUT);
     loopCnt = 1000;
     while (digitalRead(oneWirePin) == LOW)
     {
       delayMicroseconds(1);
       if (loopCnt-- == 0) return ERROR_TIMEOUT;
     }
     loopCnt = 1000;
     while (digitalRead(oneWirePin) == HIGH)
     {
       delayMicroseconds(1);
       if (loopCnt-- == 0) return ERROR_TIMEOUT;
     }
    
    /*****   readData    ******/
     clearBuff();
     cnt = 7;
     idx = 0;
     for (int i = 0; i < 40; i++)
     {  
          while (digitalRead(oneWirePin) == LOW);
          uint32_t t = micros();
          
          while (digitalRead(oneWirePin) == HIGH);
          if (micros() - t > 35) bytes[idx] |= (1 << cnt);
          if (cnt == 0)   // next byte?
          {
            cnt = 7;    // restart at MSB
            idx++;      // next byte!
          }
          else cnt--;
          if(idx > 4) idx = 0;
     }
     pinMode(oneWirePin, OUTPUT);
     digitalWrite(oneWirePin, HIGH);
     return READ_OK; 
}

/**********************************************************
Description: check Sum
Parameters:  pin : ONEWIRE data pin
Return:      READ_OK/ERROR_CHECKSUM   
Others:      None
**********************************************************/
int BM25S2021_1::checkSum()
{
      uint8_t sum = bytes[0] + bytes[1] + bytes[2] + bytes[3];
      if(bytes[4] != sum) return ERROR_CHECKSUM;
      else return READ_OK;
}
