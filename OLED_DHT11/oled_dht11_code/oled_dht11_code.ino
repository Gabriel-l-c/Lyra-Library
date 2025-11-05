

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define pin 13


int Humidity    =0; 
int Temperature = 0; 
uint8_t sum = 0;
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define NUMFLAKES     10 // Number of snowflakes in the animation example

#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16
static const unsigned char PROGMEM logo_bmp[] =
{ 0b00000000, 0b11000000,
  0b00000001, 0b11000000,
  0b00000001, 0b11000000,
  0b00000011, 0b11100000,
  0b11110011, 0b11100000,
  0b11111110, 0b11111000,
  0b01111110, 0b11111111,
  0b00110011, 0b10011111,
  0b00011111, 0b11111100,
  0b00001101, 0b01110000,
  0b00011011, 0b10100000,
  0b00111111, 0b11100000,
  0b00111111, 0b11110000,
  0b01111100, 0b11110000,
  0b01110000, 0b01110000,
  0b00000000, 0b00110000 };

void setup() {
  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.display();

}

void loop() {
   display.clearDisplay();

    // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
 
  readdht();
  delay(100); // Pause for 2 seconds
 
  displayhumidade();    // Draw scrolling text
  delay(1000);
  displaytemperatura();    // Draw scrolling text
  delay(1000);
}

void displayhumidade(void) {
  display.clearDisplay();

  display.setTextSize(1); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(20, 0);
  display.println(F("HUMIDADE"));
  display.setTextSize(2); // Draw 2X-scale text
  display.setCursor(50, 16);
  display.print(Humidity);
  
  
  display.display();      // Show initial text
  delay(100);
 
  display.startscrollleft(0x00, 0x0F);
  delay(2000);
 
  display.stopscroll();
}

void displaytemperatura(void) {
  display.clearDisplay();

  display.setTextSize(1); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(20, 0);
  display.println(F("TEMPERATURA"));
  display.setTextSize(2); // Draw 2X-scale text
  display.setCursor(50, 16);
  display.print(Temperature);
  

  display.display();      // Show initial text
  delay(100);

  display.startscrollleft(0x00, 0x0F);
  delay(2000);

  display.stopscroll();

}


float readdht(){
 
uint8_t bits[5];
uint8_t cnt = 7;
uint8_t idx = 0;
for (int i=0; i< 5; i++) bits[i] = 0;
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
  delay(18);
  digitalWrite(pin, HIGH);
  delayMicroseconds(40);
  pinMode(pin, INPUT);
  unsigned int loopCnt = 10000;
  
  while(digitalRead(pin) == LOW)
    if (loopCnt-- == 0) return -2;

  loopCnt = 10000;
  while(digitalRead(pin) == HIGH)
    if (loopCnt-- == 0) return -2;
  for (int i=0; i<40; i++)
  {
    loopCnt = 10000;
    while(digitalRead(pin) == LOW)
      if (loopCnt-- == 0) return -2;

    unsigned long t = micros();

    loopCnt = 10000;
    while(digitalRead(pin) == HIGH)
      if (loopCnt-- == 0) return -2;

    if ((micros() - t) > 40) bits[idx] |= (1 << cnt);
    if (cnt == 0)
    {
      cnt = 7; 
      idx++;
    }
    else cnt--;
  }
 Serial.print("humidity is ");  
 Humidity    = bits[0]; 
 Serial.print(Humidity); 
 Serial.println("%");
 Serial.print("temperature is "); 
 Temperature = bits[2]; 
 Serial.print(Temperature); 
 Serial.println("C");
 sum = bits[0] + bits[2];
  if (bits[4] != sum) return -1;
  return 0;

  }