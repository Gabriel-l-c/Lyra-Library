#include <SFE_BMP180.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <TFT.h>  // Arduino LCD library

// pin definitions
#define sd_cs  4
#define lcd_cs 10
#define dc     9
#define rst    8  

TFT TFTscreen = TFT(lcd_cs, dc, rst);
SFE_BMP180 pressure;

#define ALTITUDE 4.0 // Altitude of SparkFun's HQ in Boulder, CO. in meters
char sensorPrintout[10]; // Aumentei o tamanho do buffer

void setup() {
  Serial.begin(9600);
  Serial.println("REBOOT");

  if (pressure.begin())
    Serial.println("BMP180 init success");
  else {
    Serial.println("BMP180 init fail\n\n");
    while(1); // Pause forever.
  }
  TFTscreen.begin();
  TFTscreen.background(255, 255, 255); // Fundo branco
  TFTscreen.stroke(255, 0, 0);
  TFTscreen.setTextSize(2);
  TFTscreen.setCursor(15, 8);
  TFTscreen.print("Temperatura: "); // Use print em vez de text
}

void loop() {
  char status; 
  double T, P, p0, a;

  status = pressure.startTemperature();
  if (status != 0) {
    delay(status);

    status = pressure.getTemperature(T);
    if (status != 0) {
      // Converta T para String e armazene em sensorPrintout
      // dtostrf(T, 6, 2, sensorPrintout); // Converte o valor de T para uma string

      // Limpa a área onde a temperatura foi exibida
      // TFTscreen.fillRect(15, 30, 100, 20, 0xFFFF); // Limpa a área com fundo branco

      // // Desenha o novo valor da temperatura
      // TFTscreen.setCursor(15, 30); // Define a posição do cursor
      // TFTscreen.setTextSize(2);
      // TFTscreen.print(sensorPrintout); // Exibe a temperatura
      // Serial.print("temperature: ");
      // Serial.print(T);
      // Serial.print("\n");
      
      dtostrf(T, 6, 2, sensorPrintout); // Converte o valor de T para uma string
      print_t();
      // TFTscreen.setCursor(15, 50);
      // TFTscreen.print("Pressao : "); // Use print em vez de text
      status = pressure.startPressure(3);
      if (status != 0) {
        delay(status);
        status = pressure.getPressure(P, T);
        if (status != 0) {
           dtostrf(P, 6, 2, sensorPrintout); // Converte o valor de T para uma string
            print_p();
      // Limpa a área onde a temperatura foi exibida
          // TFTscreen.fillRect(15, 70, 100, 20, 0xFFFF); // Limpa a área com fundo branco

      // Desenha o novo valor da temperatura
          // TFTscreen.setCursor(15, 70); // Define a posição do cursor
          // TFTscreen.setTextSize(2);
          // TFTscreen.print(sensorPrintout); // Exibe a temperatura
          
        } else {
          Serial.println("error retrieving pressure measurement\n");
        }
      } else {
        Serial.println("error starting pressure measurement\n");
      }
    } else {
      Serial.println("error retrieving temperature measurement\n");
    }
  } else {
    Serial.println("error starting temperature measurement\n");
  }

  delay(2000);  // Pause for 2 seconds.
}

void print_t(){

      // Limpa a área onde a temperatura foi exibida
      TFTscreen.fillRect(15, 30, 100, 20, 0xFFFF); // Limpa a área com fundo branco

      // Desenha o novo valor da temperatura
      TFTscreen.setCursor(15, 30); // Define a posição do cursor
      TFTscreen.setTextSize(2);
      TFTscreen.print(sensorPrintout); // Exibe a temperatura
}
void print_p(){
  TFTscreen.setCursor(15, 50);
  TFTscreen.print("Pressao : "); // Use print em vez de text
    TFTscreen.fillRect(15, 70, 100, 20, 0xFFFF); // Limpa a área com fundo branco

      // Desenha o novo valor da temperatura
          TFTscreen.setCursor(15, 70); // Define a posição do cursor
          TFTscreen.setTextSize(2);
          TFTscreen.print(sensorPrintout); // Exibe a temperatura
}