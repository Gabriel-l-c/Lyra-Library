/*  
    next-hack's sd-video-test (usart version).
    Copyright 2017-2019 Nicola Wrachien and Franco Gobbo (next-hack.com)
    This file is part of next-hack's sd-video-test.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program  is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with next-hack's sd-test.  If not, see <http://www.gnu.org/licenses/>.   
*/

#include "display.h"
#include "ff.h"
FATFS g_fs;
FIL g_file;
void synchronizeTo20Hz(void); // used to get exactly 20 fps.
void setup()
{
  // put your setup code here, to run once:
  unsigned char res;
  DDRC |= 0x3 << 4;  // needed to deselect display while configuring sd
  PORTC |= 0x3 << 4; //  needed to deselect display while configuring sd
  pinMode(LED_BUILTIN, OUTPUT);
  res = f_mount(&g_fs, "0:", 1);
  if (res)
  {
    while(1)   // our journey ends here!
    {
        digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
        delay(200);                       // wait for a second
        digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
        delay(200);
    }
  }
  DisplayInit();
  // initialize timer (used for synchronizeTo20Hz())
  //
  // Configure TIMER 1 as frame rate generator. Prescaler 256.
  TCCR1A = 0;
  TCCR1B = (1 << WGM12) | (0 << WGM10) ;
  TCCR1C = 0;
  OCR1A =  3124;      
  TIFR1 = (1 << OCF1A);                           // clear flag
  TIMSK1 = (1 << OCF1A); ; 
  TCNT1 = 0;
  TCCR1B = (1 << CS12) | (1 << WGM12);  
  //
  // We use Pin C0 to allow easy frequency measurements with the scope.
  DDRC |= 1;
}
void loop()
{
  unsigned int bytesAvailable;
  uint8_t res;
  res = f_open(&g_file, "video.raw",FA_READ);
  sei();
  if (res)
  {
    while(1)   // our journey ends here!
    {
        digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
        delay(1000);                       // wait for a second
        digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
        delay(1000);
    }
  }
  //
  cli();
  BeginUpdateDisplay();     // reset the display destination address so that it points to the first pixel
  EndUpdateDisplay();       // deselect the display.
  while (1)
  {
      f_mmc_to_display_direct_transfer(&g_file, 40*1024, &bytesAvailable);
      PINC = 1;                   // Toggle PORTC PIN 0 (this is useful for fps measurements with the scope)
      //synchronizeTo20Hz();      // uncomment to synchronize to 20 Hz
      if (bytesAvailable == 0)
      {
        EndUpdateDisplay(); 
        return;
      }
                   
  }  
  EndUpdateDisplay();     
}
void synchronizeTo20Hz(void)
{
    while ( !(TIFR1 & (1 << OCF1A) ));
    TIFR1  |= (1 << OCF1A);  
}
