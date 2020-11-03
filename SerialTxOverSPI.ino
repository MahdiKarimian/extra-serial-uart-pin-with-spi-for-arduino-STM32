#include <SPI.h>


//send buffer for PC
char buf [99]={"Test Send data with spi to Computer serial!abcdefghijklmnopqrstuvw ABCDEFGHIJKLMNOPQRSTUVW \n \r"};


//buffer for SerialSPI
char buf_S[200];
unsigned char cnt = 200;
void Ser_Ovr_SPI(char * text)
{
  if(cnt >= 200)
  {
    ConvertBuff(text);
    cnt = 0;
    SPDR = text[0];
    SPCR |= _BV(SPIE);  
  }
  else
  {
    Serial.println("SerialSPI is busy!");
  }
}


//change buffer for send in spi
//this function write bytes in double byte with extra start and stop bits
void ConvertBuff(char * text)
{
  char temp = 0;
  int i;
  buf_S[0] = 0xFF;//always start with 0xFF
  buf_S[1] = 0xFF;//always start with 0xFF
  buf_S[2] = 0xFF;//always start with 0xFF
  buf_S[3] = 0xFF;//always start with 0xFF
  
  for( i=0; i<100; i++)
  {
    temp = (text[i]>>7) & 0x01;
    buf_S[2*i+2]     = (text[i] << 1) & 0xFE;
    if(temp == 0)
      buf_S[2*i + 3]=0xFF;
    else
      buf_S[2*i + 3]=0xFE;
  }
  
}

void setup (void)
{
    int i;
    Serial.begin (115200);   // debugging
    
    SPI.begin();
    SPI.beginTransaction(SPISettings(250000, LSBFIRST, SPI_MODE1));
    // now turn on interrupts
    SPI.attachInterrupt();
    //SPI.endTransaction();
}  // end of setup


// SPI interrupt routine
ISR (SPI_STC_vect)
{
  if(cnt < 200)
  {
    cnt++;
    SPDR = buf_S[cnt];
    SPCR |= _BV(SPIE);
  }
  else
  {
    Serial.println("end"); 
  }
}  // end of interrupt routine SPI_STC_vect

// main loop - wait for flag set in interrupt routine
void loop (void)
{

  Serial.println(cnt); 
  delay(500);
  Ser_Ovr_SPI(buf);
  delay(500);

}  // end of loop
