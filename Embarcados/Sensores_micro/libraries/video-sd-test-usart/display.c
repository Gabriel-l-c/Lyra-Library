#include <arduino.h>

/*
	This file contains display-specific functions.
	
	Display:
	These functions support a 160x128 SPI display.
*/

#include "TFT_ILI9163C_registers.h"
#include <avr/io.h>
#include <avr/pgmspace.h>
#define ILI9163_INIT_DELAY 0x80
#include "display.h"
#define nop() asm volatile("nop");

// MODIFY THESE DEFINES ACCORDING TO YOUR REQUIREMENTS
#define DISPLAY_NCS_PORT PORTC
#define DISPLAY_DC_PORT PORTC
#define DISPLAY_NCS_DDR DDRC
#define DISPLAY_DC_DDR DDRC
#define DISPLAY_NCS_PIN 5
#define DISPLAY_DC_PIN 4
// Useful macros
#define DISPLAY_DC_HIGH()  DISPLAY_DC_PORT |= (1 << DISPLAY_DC_PIN)		
#define DISPLAY_DC_LOW() DISPLAY_DC_PORT &= ~(1 << DISPLAY_DC_PIN)
#define DISPLAY_NCS_HIGH()  DISPLAY_NCS_PORT |= (1 << DISPLAY_NCS_PIN)    
#define DISPLAY_NCS_LOW() DISPLAY_NCS_PORT &= ~(1 << DISPLAY_NCS_PIN)


void DisplayWriteCommand(uint8_t value)
{
  // Usart in SPI Mode
  volatile uint8_t dummy;
	DISPLAY_DC_LOW();
	DISPLAY_NCS_LOW();
  nop();
  nop();
  nop();
  nop();
  nop();

//  SPDR = value;
  UDR0 = value;
  loop_until_bit_is_set(UCSR0A, TXC0);
  UCSR0A |= TXC0;
  nop();
  nop();
  nop();
  nop();

  dummy = UDR0;     // is this required?
  
	DISPLAY_NCS_HIGH();
 #warning dummy required?
}
void DisplayWriteData(uint8_t value)
{
  volatile uint8_t dummy;
	DISPLAY_DC_HIGH();
	DISPLAY_NCS_LOW();
  nop();
  nop();
  nop();
  nop();

//  SPDR = value;
//  loop_until_bit_is_set(SPSR, SPIF);
  UDR0 = value;
  loop_until_bit_is_set(UCSR0A, TXC0);
  UCSR0A |= TXC0;
  nop();
  nop();
  nop();
  nop();

  //
  dummy = UDR0;     // is this required?

  DISPLAY_NCS_HIGH();
 #warning dummy required?
}

void InitDisplaySPI(void)
{
  // usart version
  
  PORTD |= 0b00010010;
  DDRD |=  0b00010010;
  /* Enable USART module in SPI mode 0 */
  
  UCSR0B = 0b00011000;
  UCSR0C = 0b11000000;
  UBRR0  = 0;  
}
void InitDisplayGPIO(void)
{
  DISPLAY_NCS_HIGH();
  DISPLAY_DC_HIGH();
  DISPLAY_NCS_DDR |= (1 << DISPLAY_NCS_PIN);
  DISPLAY_DC_DDR |= (1 << DISPLAY_DC_PIN);   
}

void DisplayInit(void)
{
  // this function initializes the SPI (even though it might be already initialized by other devices, such as SD), the I/O ports for D/C and nCS and 
  // sends the initialization commands to the display 
  /* The following array has the following structure:
   *  number of commands
   *  Command Code, number of data bytes, data bytes (if any), delay in ms (if number of bytes is in OR with 0x80)
   *  ...
   */
  static const uint8_t ILI9163_cmds[] PROGMEM =
  {
    17,             // 17 commands follow
    CMD_SWRESET,  0 | ILI9163_INIT_DELAY, 100,       // Software reset
    CMD_SLPOUT,  0 | ILI9163_INIT_DELAY, 100,       // Exit sleep mode
    CMD_PIXFMT,  1, 0x05, // Set pixel format
    CMD_GAMMASET,  1, 0x04, // Set Gamma curve
    CMD_GAMRSEL,  1, 0x01, // Gamma adjustment enabled
    CMD_PGAMMAC, 15, 0x3F, 0x25, 0x1C, 0x1E, 0x20, 0x12, 0x2A, 0x90,
              0x24, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, // Positive Gamma
    CMD_NGAMMAC, 15, 0x20, 0x20, 0x20, 0x20, 0x05, 0x00, 0x15,0xA7,
              0x3D, 0x18, 0x25, 0x2A, 0x2B, 0x2B, 0x3A, // Negative Gamma
    CMD_FRMCTR1,  2, 0xA, 0x14,  // Frame rate control 1  //0x0A, 0x14
    CMD_DINVCTR,  1, 0x07,       // Display inversion
    CMD_PWCTR1,  2, 0x0A, 0x02, // Power control 1
    CMD_PWCTR2,  1, 0x02,       // Power control 2
    CMD_VCOMCTR1,  2, 0x4F, 0x5A, // Vcom control 1 (0x4F 0x5A)
    CMD_VCOMOFFS,  1, 0x40,       // Vcom offset 
    CMD_CLMADRS,  4 | ILI9163_INIT_DELAY, 0x00, 0x00, 0x00, 0x9F, 250, // Set column address  0x00  0x00 0x00 0x9F, 0 (no delay)
    CMD_PGEADRS,  4, 0x00, 0x00, 0x00, 0x7F,            // Set page address 0x00 0x00 0x00 0x7F
    CMD_MADCTL,  1, 0x60,       // Set address mode 0xC8 
    CMD_DISPON,  0,             // Set display on
  };
  uint8_t *addr = ILI9163_cmds;
	uint8_t ms, numArgs,numCommands;
	//
  
	InitDisplayGPIO();
	InitDisplaySPI();
  // Send initialization commands
  numCommands = pgm_read_byte(addr++);            // Number of commands to follow
  while (numCommands--)                           // For each command...
  {
    DisplayWriteCommand(pgm_read_byte(addr++));    // Read, issue command
    numArgs = pgm_read_byte(addr++);        // Number of args to follow
    ms = numArgs & ILI9163_INIT_DELAY;      // If hibit set, delay follows args
    numArgs &= ~ILI9163_INIT_DELAY;         // Mask out delay bit
    while (numArgs--)                       // For each argument...
    {
      DisplayWriteData(pgm_read_byte(addr++)); // Read, issue argument
    }

    if (ms)
    {
      ms = pgm_read_byte(addr++);     // Read post-command delay time (ms)
      delay( (ms==255 ? 500 : ms) );
    }
  }
}

void BeginUpdateDisplay()
{
	DisplayWriteCommand(CMD_CLMADRS);
	DisplayWriteData(0);
	DisplayWriteData(0);
	DisplayWriteData(0);
	DisplayWriteData(0x9F);
	DisplayWriteCommand(CMD_PGEADRS);
	DisplayWriteData(0);
	DisplayWriteData(0);
	DisplayWriteData(0);
	DisplayWriteData(127);
	DisplayWriteCommand(CMD_RAMWR);
	DISPLAY_DC_HIGH();
	DISPLAY_NCS_LOW();														
  nop();

}
void SelectDisplay(void)
{
	DISPLAY_NCS_LOW();
  nop();
}
void EndUpdateDisplay()
{
	DISPLAY_NCS_HIGH();															
  nop();
}


