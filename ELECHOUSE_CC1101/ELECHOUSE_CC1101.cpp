#include "ELECHOUSE_CC1101.h"
#include <Arduino.h>
#include <SPI.h>

/****************************************************************/
#define 	WRITE_BURST     	0x40						//write burst
#define 	READ_SINGLE     	0x80						//read single
#define 	READ_BURST      	0xC0						//read burst
#define 	BYTES_IN_RXFIFO     0x7F  						//byte number in RXfifo

#define         ChipSelect_Pin 2
#define         GDNull_Pin 16
/****************************************************************/
byte PaTabel[8] = {0x60 ,0x60 ,0x60 ,0x60 ,0x60 ,0x60 ,0x60 ,0x60};

/****************************************************************
*FUNCTION NAME:SpiInit
*FUNCTION     :spi communication initialization
*INPUT        :none
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_CC1101::SpiInit(void)
{
  // Pin initialisieren
  pinMode(ChipSelect_Pin, OUTPUT);
  
   // Clock-Signal auf Pin 14 mappen
  //SPI.setSCK(14);
  
    //SPI initialisieren
  SPI.begin();
 
  
  
}
/****************************************************************
*FUNCTION NAME:SpiTransfer
*FUNCTION     :spi transfer
*INPUT        :value: data to send
*OUTPUT       :data to receive
****************************************************************/
byte ELECHOUSE_CC1101::SpiTransfer(byte value)
{
  SPI.transfer(value);
  return SPI.transfer(0);
}

/****************************************************************
*FUNCTION NAME: GDO_Set()
*FUNCTION     : set GDO0,GDO2 pin
*INPUT        : none
*OUTPUT       : none
****************************************************************/
void ELECHOUSE_CC1101::GDO_Set (void)
{
	 pinMode(GDNull_Pin, INPUT);
	
}

/****************************************************************
*FUNCTION NAME:Reset
*FUNCTION     :CC1101 reset //details refer datasheet of CC1101/CC1100//
*INPUT        :none
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_CC1101::Reset (void)
{
	digitalWrite(ChipSelect_Pin, LOW);
	delay(1);
	digitalWrite(ChipSelect_Pin, HIGH);
        delay(1);
	digitalWrite(ChipSelect_Pin, LOW);
	while(digitalRead(12));
	SpiTransfer(CC1101_SRES);
	while(digitalRead(12));
	digitalWrite(ChipSelect_Pin, HIGH);
}

/****************************************************************
*FUNCTION NAME:Init
*FUNCTION     :CC1101 initialization
*INPUT        :none
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_CC1101::Init(void)
{
	SpiInit();										//spi initialization
	GDO_Set();										//GDO set
	digitalWrite(ChipSelect_Pin, HIGH);
        digitalWrite(14, HIGH);
	digitalWrite(11, LOW);
	Reset();										//CC1101 reset
	RegConfigSettings();							//CC1101 register config
	SpiWriteBurstReg(CC1101_PATABLE,PaTabel,8);		//CC1101 PATABLE config
}


/****************************************************************
*FUNCTION NAME:SpiWriteReg
*FUNCTION     :CC1101 write data to register
*INPUT        :addr: register address; value: register value
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_CC1101::SpiWriteReg(byte addr, byte value)
{
        SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
	digitalWrite(ChipSelect_Pin, LOW);
        while(digitalRead(12));
	SpiTransfer(addr);
	SpiTransfer(value);
	digitalWrite(ChipSelect_Pin, HIGH);
        SPI.endTransaction();
}

/****************************************************************
*FUNCTION NAME:SpiWriteBurstReg
*FUNCTION     :CC1101 write burst data to register
*INPUT        :addr: register address; buffer:register value array; num:number to write
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_CC1101::SpiWriteBurstReg(byte addr, byte *buffer, byte num)
{
	byte i, temp;

	temp = addr | WRITE_BURST;
    SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
    digitalWrite(ChipSelect_Pin, LOW);
    while(digitalRead(12));
    SpiTransfer(temp);
    for (i = 0; i < num; i++)
 	{
        SpiTransfer(buffer[i]);
    }
    digitalWrite(ChipSelect_Pin, HIGH);
      SPI.endTransaction();
}

/****************************************************************
*FUNCTION NAME:SpiStrobe
*FUNCTION     :CC1101 Strobe
*INPUT        :strobe: command; //refer define in CC1101.h//
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_CC1101::SpiStrobe(byte strobe)
{
	SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
        digitalWrite(ChipSelect_Pin, LOW);
	while(digitalRead(12));
	SpiTransfer(strobe);
	digitalWrite(ChipSelect_Pin, HIGH);
        SPI.endTransaction();
}

/****************************************************************
*FUNCTION NAME:SpiReadReg
*FUNCTION     :CC1101 read data from register
*INPUT        :addr: register address
*OUTPUT       :register value
****************************************************************/
byte ELECHOUSE_CC1101::SpiReadReg(byte addr) 
{
	byte temp, value;

    temp = addr|READ_SINGLE;
	SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
        digitalWrite(ChipSelect_Pin, LOW);
	while(digitalRead(12));
	SpiTransfer(temp);
	value=SpiTransfer(0);
	digitalWrite(ChipSelect_Pin, HIGH);
        SPI.endTransaction();

	return value;
}

/****************************************************************
*FUNCTION NAME:SpiReadBurstReg
*FUNCTION     :CC1101 read burst data from register
*INPUT        :addr: register address; buffer:array to store register value; num: number to read
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_CC1101::SpiReadBurstReg(byte addr, byte *buffer, byte num)
{
	byte i,temp;

	temp = addr | READ_BURST;
	SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
        digitalWrite(ChipSelect_Pin, LOW);
	while(digitalRead(12));
	SpiTransfer(temp);
	for(i=0;i<num;i++)
	{
		buffer[i]=SpiTransfer(0);
	}
	digitalWrite(ChipSelect_Pin, HIGH);
        SPI.endTransaction();
}

/****************************************************************
*FUNCTION NAME:SpiReadStatus
*FUNCTION     :CC1101 read status register
*INPUT        :addr: register address
*OUTPUT       :status value
****************************************************************/
byte ELECHOUSE_CC1101::SpiReadStatus(byte addr) 
{
	byte value,temp;

	temp = addr | READ_BURST;
	SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
        digitalWrite(ChipSelect_Pin, LOW);
	while(digitalRead(12));
	SpiTransfer(temp);
	value=SpiTransfer(0);
	digitalWrite(ChipSelect_Pin, HIGH);
        SPI.endTransaction();

	return value;
}

/****************************************************************
*FUNCTION NAME:RegConfigSettings
*FUNCTION     :CC1101 register config //details refer datasheet of CC1101/CC1100//
*INPUT        :none
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_CC1101::RegConfigSettings(void) 
{
  
  //Abweichungen
    SpiWriteReg(CC1101_FSCTRL1,  0x08);
    
        SpiWriteReg(CC1101_FREQ2,    0x10);
         
    SpiWriteReg(CC1101_FREQ1,    0xA7);

    SpiWriteReg(CC1101_FREQ0,    0x62);
     
    SpiWriteReg(CC1101_MDMCFG4,  0x5B);
    
    SpiWriteReg(CC1101_MDMCFG3,  0xF8);
 
    SpiWriteReg(CC1101_MDMCFG2,  0x03);
       
   SpiWriteReg(CC1101_DEVIATN,  0x47);
      
   SpiWriteReg(CC1101_FOCCFG,   0x1D);
      
     SpiWriteReg(CC1101_FSCAL3,   0xEA);
        
        SpiWriteReg(CC1101_FSCAL0,   0x11);
  
    SpiWriteReg(CC1101_FSCTRL0,  0x00);

    SpiWriteReg(CC1101_MDMCFG1,  0x22);
    SpiWriteReg(CC1101_MDMCFG0,  0xF8);
    SpiWriteReg(CC1101_CHANNR,   0x00);
   
    SpiWriteReg(CC1101_FREND1,   0xB6);
    SpiWriteReg(CC1101_FREND0,   0x10);
    SpiWriteReg(CC1101_MCSM0 ,   0x18);
   
    SpiWriteReg(CC1101_BSCFG,    0x1C);
    SpiWriteReg(CC1101_AGCCTRL2, 0xC7);
	SpiWriteReg(CC1101_AGCCTRL1, 0x00);
    SpiWriteReg(CC1101_AGCCTRL0, 0xB2);
  
	SpiWriteReg(CC1101_FSCAL2,   0x2A);
	SpiWriteReg(CC1101_FSCAL1,   0x00);

    SpiWriteReg(CC1101_FSTEST,   0x59);
    SpiWriteReg(CC1101_TEST2,    0x81);
    SpiWriteReg(CC1101_TEST1,    0x35);
    SpiWriteReg(CC1101_TEST0,    0x09);
    SpiWriteReg(CC1101_IOCFG2,   0x0B); 	//serial clock.synchronous to the data in synchronous serial mode
    SpiWriteReg(CC1101_IOCFG0,   0x06);  	//asserts when sync word has been sent/received, and de-asserts at the end of the packet 
    SpiWriteReg(CC1101_PKTCTRL1, 0x04);		//two status bytes will be appended to the payload of the packet,including RSSI LQI and CRC OK
											//No address check
    SpiWriteReg(CC1101_PKTCTRL0, 0x05);		//whitening off;CRC Enable��variable length packets, packet length configured by the first byte after sync word
    SpiWriteReg(CC1101_ADDR,     0x00);		//address used for packet filtration.
    SpiWriteReg(CC1101_PKTLEN,   0x3D); 	//61 bytes max length
    
    //Abweichungen
  /*  SpiWriteReg(CC1101_FSCTRL1,  0x06);
    SpiWriteReg(0x03,0x47);
    SpiWriteReg(CC1101_FREQ2,    0x21);
    SpiWriteReg(CC1101_FREQ1,    0x62);
    SpiWriteReg(CC1101_FREQ0,    0x76);
    SpiWriteReg(CC1101_MDMCFG4,  0xF5);
    SpiWriteReg(CC1101_MDMCFG3,  0x83);
    SpiWriteReg(CC1101_MDMCFG2,  0x13);
    SpiWriteReg(CC1101_DEVIATN,  0x15);
    SpiWriteReg(CC1101_FOCCFG,   0x16);
    SpiWriteReg(0x20,   0xFB);
    SpiWriteReg(CC1101_FSCAL3,   0xE9);
    SpiWriteReg(CC1101_FSCAL0,   0x1F);
     SpiWriteReg(CC1101_TEST2,    0x81);*/
}

/****************************************************************
*FUNCTION NAME:SendData
*FUNCTION     :use CC1101 send data
*INPUT        :txBuffer: data array to send; size: number of data to send, no more than 61
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_CC1101::SendData(byte *txBuffer,byte size)
{
	SpiWriteReg(CC1101_TXFIFO,size);
	SpiWriteBurstReg(CC1101_TXFIFO,txBuffer,size);			//write data to send
	SpiStrobe(CC1101_STX);									//start send	
    while (!digitalRead(GDNull_Pin));								// Wait for GDO0 to be set -> sync transmitted  
    while (digitalRead(GDNull_Pin));	
 //  delay(1);							// Wait for GDO0 to be cleared -> end of packet
	SpiStrobe(CC1101_SFTX);									//flush TXfifo
}

/****************************************************************
*FUNCTION NAME:SetReceive
*FUNCTION     :set CC1101 to receive state
*INPUT        :none
*OUTPUT       :none
****************************************************************/
void ELECHOUSE_CC1101::SetReceive(void)
{
	SpiStrobe(CC1101_SRX);
}

/****************************************************************
*FUNCTION NAME:CheckReceiveFlag
*FUNCTION     :check receive data or not
*INPUT        :none
*OUTPUT       :flag: 0 no data; 1 receive data 
****************************************************************/
byte ELECHOUSE_CC1101::CheckReceiveFlag(void)
{
	if(digitalRead(GDNull_Pin))			//receive data
	{
		while (digitalRead(GDNull_Pin));
		return 1;
	}
	else							// no data
	{
		return 0;
	}
}


/****************************************************************
*FUNCTION NAME:ReceiveData
*FUNCTION     :read data received from RXfifo
*INPUT        :rxBuffer: buffer to store data
*OUTPUT       :size of data received
****************************************************************/
byte ELECHOUSE_CC1101::ReceiveData(byte *rxBuffer)
{
	byte size;
	byte status[2];

	if(SpiReadStatus(CC1101_RXBYTES) & BYTES_IN_RXFIFO)
	{
		size=SpiReadReg(CC1101_RXFIFO);
		SpiReadBurstReg(CC1101_RXFIFO,rxBuffer,size);
		SpiReadBurstReg(CC1101_RXFIFO,status,2);
		SpiStrobe(CC1101_SFRX);
		return size;
	}
	else
	{
		SpiStrobe(CC1101_SFRX);
		return 0;
	}
	
}

ELECHOUSE_CC1101 ELECHOUSE_cc1101;