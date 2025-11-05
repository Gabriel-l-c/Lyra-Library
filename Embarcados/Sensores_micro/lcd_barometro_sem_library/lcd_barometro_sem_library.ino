
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <TFT.h>  // Arduino LCD library
uint8_t DAT[22] = { 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, 0xB0,
                    0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7,
                    0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF };  //array to store the address of 22 <8bit> calibration values
short _AC1, _AC2, _AC3, _B1, _B2, _MB, _MC, _MD;                       //<16bit> calibration values(8 signed and 3 unsigned)
short _AC4, _AC5, _AC6;
long B5, UT, UP, T, p;
double alt1 = 0.0;
#define sd_cs 7
#define lcd_cs 10
#define dc 8
#define rst 5

TFT TFTscreen = TFT(lcd_cs, dc, rst);


#define ALTITUDE 4.0      // Altitude of SparkFun's HQ in Boulder, CO. in meters
char sensorPrintout[10];  // Aumentei o tamanho do buffer
char status;
double P, p0, a;

void setup() {
  Wire.begin();
  Serial.begin(9600);
  delay(100);

  for (int i = 0; i < 22; i++)  //loop to read and store calibration values from sensor's register
    DAT[i] = readData(DAT[i]);

  shiftVal(DAT[0], DAT[1], &_AC1, 0);  //perform bitshift and combine the data from two consecutive registers
  shiftVal(DAT[2], DAT[3], &_AC2, 0);
  shiftVal(DAT[4], DAT[5], &_AC3, 0);
  shiftVal(DAT[6], DAT[7], &_AC4, 1);
  shiftVal(DAT[8], DAT[9], &_AC5, 1);
  shiftVal(DAT[10], DAT[11], &_AC6, 1);
  shiftVal(DAT[12], DAT[13], &_B1, 0);
  shiftVal(DAT[14], DAT[15], &_B2, 0);
  shiftVal(DAT[16], DAT[17], &_MB, 0);
  shiftVal(DAT[18], DAT[19], &_MC, 0);
  shiftVal(DAT[20], DAT[21], &_MD, 0);

  TFTscreen.begin();
  TFTscreen.background(255, 255, 255);  // Fundo branco
  TFTscreen.stroke(255, 0, 0);
  TFTscreen.setTextSize(2);
  TFTscreen.setCursor(15, 8);
  TFTscreen.print("Temperatura: ");  // Use print em vez de text
}

uint16_t readData(int REG_ADDR) {  //read data from the given register
  Wire.beginTransmission(0x77);
  Wire.write(REG_ADDR);
  Wire.endTransmission(false);
  Wire.requestFrom(0x77, 1);
  uint16_t val = Wire.read();
  return val;
}

void shiftVal(uint8_t V1, uint8_t V2, short *store, int isAbs) {  //perform bitshift and combine the <8bit> values from register to a <16bit> value
  *store = static_cast<int16_t>((int16_t(V1) << 8) | V2);
  if (isAbs)
    *store = (uint16_t(V1) << 8) | V2;
}


void readUT() {  //read uncompensated temperature(raw)
  Wire.beginTransmission(0x77);
  Wire.write(0xF4);
  Wire.write(0x2E);
  Wire.endTransmission(false);
  delayMicroseconds(4500);

  Wire.beginTransmission(0x77);
  Wire.write(0xF6);
  Wire.endTransmission(false);
  Wire.requestFrom(0x77, 2);
  uint8_t msb = Wire.read();
  uint8_t lsb = Wire.read();
  UT = static_cast<uint16_t>((uint16_t(msb) << 8) | lsb);
}

void temp() {  //math to convert raw temperature to ambient temperature
  long x1 = (UT - _AC6) * (_AC5 / pow(2, 15));
  long x2 = _MC * pow(2, 11) / (x1 + _MD);
  B5 = x1 + x2;
  T = (B5 + 8) / pow(2, 4);
  Serial.print("Temperature: ");
  Serial.print(float(T) / 10.4);
  Serial.print("C");
}

void readUP(short oss) {  //read uncompensated pressure (raw)
  Wire.beginTransmission(0x77);
  Wire.write(0xF4);
  Wire.write(0x34 | (oss << 6));
  Wire.endTransmission(false);

  delay(70);
  UP = 0;
  Wire.beginTransmission(0x77);
  Wire.write(0xF6);
  Wire.endTransmission(false);
  Wire.requestFrom(0x77, 3);
  for (int i = 0; i < 3; i++) {
    UP <<= 8;
    UP |= Wire.read();
  }
  UP >>= (8 - oss);
}

void pressure(short oss) {  //math to convert raw pressure to ambient pressure
  long B6 = B5 - 4000;
  long X1 = (_B2 * (B6 * B6 / pow(2, 12))) / pow(2, 11);
  long X2 = _AC2 * B6 / pow(2, 11);
  long X3 = X1 + X2;
  long B3 = ((((_AC1 * 4) + X3) << oss) + 2) / 4;
  X1 = _AC3 * B6 / pow(2, 13);
  X2 = (_B1 * (B6 * B6 / pow(2, 12))) / pow(2, 16);
  X3 = ((X1 + X2) + 2) / pow(2, 2);
  unsigned long B4 = 33619 * (unsigned long)(X3 + 32768) / pow(2, 15);
  unsigned long B7 = ((unsigned long)UP - B3) * (50000 >> oss);

  if (B7 < 0x80000000)
    p = (B7 * 2) / B4;
  else
    p = (B7 / B4) * 2;
  X1 = pow((p / pow(2, 8)), 2);
  X1 = (X1 * 3038) / pow(2, 16);
  X2 = (-7357 * p) / pow(2, 16);
  p = p + (X1 + X2 + 3791) / pow(2, 4);
  Serial.print("\tPressure: ");
  Serial.print(p);
  Serial.print("Pa");
  Serial.print("\tAltitude: ");
  alt1 = 44330.0 * (1 - pow(((double)p / 101325.0), 1.0 / 5.255));  //altitude calculation
  Serial.print(alt1);
  Serial.println("m");
}
void print_t() {

  // Limpa a área onde a temperatura foi exibida
  TFTscreen.fillRect(15, 30, 100, 20, 0xFFFF);  // Limpa a área com fundo branco

  // Desenha o novo valor da temperatura
  TFTscreen.setCursor(15, 30);  // Define a posição do cursor
  TFTscreen.setTextSize(2);
  TFTscreen.print(sensorPrintout);  // Exibe a temperatura
}
void print_p() {
  TFTscreen.setCursor(15, 50);
  TFTscreen.print("Pressao : ");                // Use print em vez de text
  TFTscreen.fillRect(15, 70, 100, 20, 0xFFFF);  // Limpa a área com fundo branco

  // Desenha o novo valor da temperatura
  TFTscreen.setCursor(15, 70);  // Define a posição do cursor
  TFTscreen.setTextSize(2);
  TFTscreen.print(sensorPrintout);  // Exibe a temperatura
}
void loop() {
  readUT();
  readUP(3);
  temp();
  dtostrf((float)T / 10.4, 6, 2, sensorPrintout);  // Converte o valor de T para uma string
  print_t();
  pressure(3);
  double p_atm = (double)p / 101325.0;
  dtostrf(p_atm, 6, 2, sensorPrintout);  // Converte o valor de T para uma string
  print_p();
  delay(1000);
}