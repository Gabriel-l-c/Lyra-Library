#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <TFT.h>  // Arduino LCD library
#include <PWMServo.h>
#include <TimeLib.h>

uint8_t DAT[22] = { 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, 0xB0,
                    0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7,
                    0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF };  //array to store the address of 22 <8bit> calibration values
short _AC1, _AC2, _AC3, _B1, _B2, _MB, _MC, _MD;                       //<16bit> calibration values(8 signed and 3 unsigned)
short _AC4, _AC5, _AC6;
long B5, UT, UP, T, p;
double alt1 = 0.0;

#define sd_cs 4
#define lcd_cs 9
#define dc 8
#define rst 1


TFT TFTscreen = TFT(lcd_cs, dc, rst);




#define ALTITUDE 4.0      // Altitude of SparkFun's HQ in Boulder, CO. in meters
char sensorPrintout[10];  // Aumentei o tamanho do buffer
char status;
double P, p0, a;

const int pinRed = 11;    // Pino do LED Vermelho
const int pinGreen = 7;  // Pino do LED Verde
const int pinBlue = 3;   // Pino do LED Azul

int brilhoMaximo = 255;  // Intensidade do LED

// Definir os pinos onde o joystick está conectado
const int pinX = A0;     // Eixo X do joystick no pino A0
const int pinY = A1;     // Eixo Y do joystick no pino A1
const int pinBotao = 2;  // Botão do joystick no pino 2

// definindo o pino do servo
const int pinServo = 5;


float n = 0.4;
int anguloServo = 90;

// Debounce - contagem de tempo
unsigned long tempoBotao = 0;
unsigned long debounceDelay = 200;  // delay

void setup() {
  setTime(12, 30, 0, 22, 3, 2025); // Exemplo: 12:30:00 - 22 de março de 2025

  Wire.begin();

  Serial.begin(9600);               // Inicia a comunicação serial
  pinMode(pinBotao, INPUT_PULLUP);  // Configura o pino do botão com resistor pull-up
  pinMode(pinRed, OUTPUT);
  pinMode(pinGreen, OUTPUT);
  pinMode(pinBlue, OUTPUT);


  // Começa com branco
  analogWrite(pinRed, 255);
  analogWrite(pinGreen, 255);
  analogWrite(pinBlue, 255);
  Serial.println("LED Branco ligado!");

  // Exibe a lista de opções
  Serial.println("=== Controle do LED RGB ===");
  Serial.println("Digite um número para selecionar a cor:");
  Serial.println("1 - Vermelho");
  Serial.println("2 - Verde");
  Serial.println("3 - Azul");
  Serial.println("4 - Amarelo");
  Serial.println("5 - Ciano");
  Serial.println("6 - Magenta");
  Serial.println("7 - Branco");
  Serial.println("0 - Desligar LED");
  Serial.println("===========================");
  // Aguarda um tempo para estabilizar


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
  TFTscreen.stroke(0, 0, 0);
  TFTscreen.setTextSize(2);

  delay(100);  // estabilidade
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


  TFTscreen.setCursor(15, 8);
  TFTscreen.print("Temperatura: ");  // Use print em vez de text
  // Limpa a área onde a temperatura foi exibida
  TFTscreen.fillRect(15, 30, 100, 20, 0xFFFF);  // Limpa a área com fundo branco
  // Desenha o novo valor da temperatura
  TFTscreen.setCursor(15, 30);  // Define a posição do cursor
  TFTscreen.print(sensorPrintout);  // Exibe a temperatura

}
void print_p() {

  TFTscreen.setCursor(15, 50);
  TFTscreen.print("Pressao : ");                // Use print em vez de text
  TFTscreen.fillRect(15, 70, 100, 20, 0xFFFF);  // Limpa a área com fundo branco

  // Desenha o novo valor da temperatura
  TFTscreen.setCursor(15, 70);  // Define a posição do cursor
  TFTscreen.print(sensorPrintout);  // Exibe a temperatura
}

void fhora(){
    TFTscreen.fillRect(15, 8, 100, 20, 0xFFFF);  // Limpa a área com fundo branco
    TFTscreen.fillRect(15, 30, 100, 20, 0xFFFF);  // Limpa a área com fundo branco

    TFTscreen.fillRect(15, 50, 100, 20, 0xFFFF);  // Limpa a área com fundo branco
    TFTscreen.fillRect(15, 70, 100, 20, 0xFFFF);  // Limpa a área com fundo branco
    // Desenha o novo valor da temperatura
    TFTscreen.setCursor(15, 45);
    TFTscreen.print(hour());  // Exibe a temperatura
    TFTscreen.setCursor(35, 45);
    TFTscreen.print(":");  // Exibe a temperatura
    TFTscreen.setCursor(40, 45);
    TFTscreen.print(minute());  // Exibe a temperatura
    TFTscreen.setCursor(50, 45);
    TFTscreen.print(":");  // Exibe a temperatura
    TFTscreen.setCursor(55, 45);
    TFTscreen.print(second());  // Exibe a temperatura
    


}
void loop() {
  readUT();
  readUP(3);
  temp();
  dtostrf((float)T / 10.4, 6, 2, sensorPrintout);  // Converte o valor de T para uma string
  print_t();
  pressure(3);
  double p_atm = (double)p / 101325.0;
  dtostrf(p_atm, 6, 2, sensorPrintout);  // Converte o valor de p para uma string
  print_p();

  // Lê o estado do botão (0 = pressionado, 1 = não pressionado)
  int estadoBotao = digitalRead(pinBotao);
  // é para ciclar a intensidade pelo botão
  if (estadoBotao == 0 && (millis() - tempoBotao) > debounceDelay) {
    if (n == 0.4) {
      n = 0.8;
    } else if (n == 0.8) {
      n = 1;
    } else if (n == 1) {
      n = 0.4;
    }
  }

  //Esse if é sobre o LED RGB
  if (Serial.available()) {
    char comando = Serial.read();  // Lê o caractere digitado


    if (comando == '1') {
      analogWrite(pinRed, n * 255);
      analogWrite(pinGreen, 0);
      analogWrite(pinBlue, 0);
      Serial.println("LED Vermelho ligado!");
    } else if (comando == '2') {
      analogWrite(pinGreen, n * 255);
      analogWrite(pinRed, 0);
      analogWrite(pinBlue, 0);
      Serial.println("LED Verde ligado!");
    } else if (comando == '3') {
      analogWrite(pinBlue, n * 255);
      analogWrite(pinRed, 0);
      analogWrite(pinGreen, 0);
      Serial.println("LED Azul ligado!");
    } else if (comando == '4') {
      Serial.println("Amarelo ligado!");
      analogWrite(pinBlue, 0);
      analogWrite(pinRed, n * 255);
      analogWrite(pinGreen, n * 255);
    } else if (comando == '5') {
      Serial.println("Ciano ligado!");
      analogWrite(pinBlue, n * 255);
      analogWrite(pinRed, 0);
      analogWrite(pinGreen, n * 255);
    } else if (comando == '6') {
      Serial.println("Magenta ligado!");
      analogWrite(pinBlue, n * 255);
      analogWrite(pinRed, n * 255);
      analogWrite(pinGreen, 0);
    } else if (comando == '7') {
      Serial.println("Branco ligado!");
      analogWrite(pinBlue, n * 255);
      analogWrite(pinRed, n * 255);
      analogWrite(pinGreen, n * 255);
      
    } else if (comando == '0') {
      Serial.println("LED desligado!");
      analogWrite(pinBlue, 0);
      analogWrite(pinRed, 0);
      analogWrite(pinGreen, 0);
    } else {
      Serial.println("Digite um número válido (0 a 9):");
    }
  }

  // Aqui é sobre os valores dos eixos X e Y do joystick
  int valorX = analogRead(pinX);  // Lê o valor do eixo X
  int valorY = analogRead(pinY);  // Lê o valor do eixo Y



  if (valorX >= 0 & valorX < 100) {
    anguloServo = 100;
  } else if (valorX >= 100 & valorX < 200) {
    anguloServo = 145;
  } else if (valorX >= 200 & valorX < 300) {
    anguloServo = 90;
  } else if (valorX >= 300 & valorX < 400) {
    anguloServo = 80;
  } else if (valorX >= 400 & valorX < 500) {
    anguloServo = 70;
  } else if (valorX >= 500 & valorX < 600) {
    anguloServo = 60;
  } else if (valorX >= 600 & valorX < 700) {
    anguloServo = 50;
  } else if (valorX >= 700 & valorX < 800) {
    anguloServo = 40;
  } else if (valorX >= 800 & valorX < 900) {
    anguloServo = 30;
  } else if (valorX >= 900 & valorX < 1000) {
    anguloServo = 20;
  } else if (valorX >= 1000) {
    anguloServo = 10;
  }

  //anguloServo = 100;

  //servo1.write(90);

  // Exibe os valores no Monitor Serial
  Serial.print("X: ");
  Serial.print(valorX);  // Imprime o valor do eixo X
  Serial.print("\tY: ");
  Serial.print(valorY);  // Imprime o valor do eixo Y
  Serial.print("\tBotão: ");
  Serial.println(estadoBotao == LOW ? "Pressionado" : "Não pressionado");  // Imprime o estado do botão

  // Exibe o ângulo do servo para debugging
  Serial.print("Angulo do Servo: ");
  Serial.println(anguloServo);

  delay(200);  // Atraso para não sobrecarregar a serial, porém se for muito devagar, ou seja, o delay for alto, ele pode não perceber se o botão for pressionado
  
  fhora();
  delay(400);
}


