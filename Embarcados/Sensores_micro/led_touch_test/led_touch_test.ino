
#include <Servo.h>

const int pinGreen = 6;    // Pino do LED Vermelho
const int pinRed= 11;  // Pino do LED Verde
const int pinBlue = 3;    // Pino do LED Azul

int brilhoMaximo = 255;  // Intensidade do LED
int touchState =0;
// Definir os pinos onde o joystick está conectado
const int pinX = A0;  // Eixo X do joystick no pino A0
const int pinY = A1;  // Eixo Y do joystick no pino A1
const int pinBotao = 2; // Botão do joystick no pino 2

// definindo o pino do servo
const int pinServo = 5;

// Objeto do servo
Servo servo1;

float n = 1;
int anguloServo = 90;

// Debounce - contagem de tempo
unsigned long tempoBotao = 0;
unsigned long debounceDelay = 200;  // delay

char comando = '8';  // Variável para armazenar o comando recebido

void setup() {
  Serial.begin(9600);  // Inicia a comunicação serial
  pinMode(pinBotao, INPUT_PULLUP);  // Configura o pino do botão com resistor pull-up
  pinMode(pinRed, OUTPUT);
  pinMode(pinGreen, OUTPUT);
  pinMode(pinBlue, OUTPUT);
  pinMode(2, INPUT);

  servo1.attach(pinServo);
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


  delay(1000); // estabilidade
}







void loop() {
  touch();
  // Chama a função para ler a string
  if (Serial.available()) {
    char comando = Serial.read();  
    Serial.println(comando);
    led_rgb(comando);
    
  }
 
  // Lê o estado do botão (0 = pressionado, 1 = não pressionado)
  int estadoBotao = digitalRead(pinBotao);
  // é para ciclar a intensidade pelo botão
  if (estadoBotao == 0 && (millis() - tempoBotao) > debounceDelay){
    if (n == 0.4){
        n = 0.8;
    } else if (n == 0.8){
        n = 1;
    } else if (n == 1){
        n = 0.4;
    }
  }





  // Aqui é sobre os valores dos eixos X e Y do joystick
  int valorX = analogRead(pinX);  // Lê o valor do eixo X
  int valorY = analogRead(pinY);  // Lê o valor do eixo Y



  if (valorX >= 0 & valorX<100){
      anguloServo = 100;
  } else if (valorX >= 100 & valorX<200){
      anguloServo = 145;
  } else if (valorX >= 200 & valorX<300){
      anguloServo = 90;
  } else if (valorX >= 300 & valorX<400){
      anguloServo = 80;
  } else if (valorX >= 400 & valorX<500){
      anguloServo = 70;
  } else if (valorX >= 500 & valorX<600){
      anguloServo = 60;
  } else if (valorX >= 600 & valorX<700){
      anguloServo = 50;
  } else if (valorX >= 700 & valorX<800){
      anguloServo = 40;
  } else if (valorX >= 800 & valorX<900){
      anguloServo = 30;
  } else if (valorX >= 900 & valorX<1000){
      anguloServo = 20;
  } else if (valorX >= 1000){
      anguloServo = 10;
  }

  //anguloServo = 100;
  servo1.write(anguloServo);
  //servo1.write(90);

  // Exibe os valores no Monitor Serial
  Serial.print("X: ");
  Serial.print(valorX);  // Imprime o valor do eixo X
  Serial.print("\tY: ");
  Serial.print(valorY);  // Imprime o valor do eixo Y
  Serial.print("\tBotão: ");
  Serial.println(estadoBotao == LOW ? "Pressionado" : "Não pressionado"); // Imprime o estado do botão

  // Exibe o ângulo do servo para debugging
  Serial.print("Angulo do Servo: ");
  Serial.println(anguloServo);

  delay(200);  // Atraso para não sobrecarregar a serial, porém se for muito devagar, ou seja, o delay for alto, ele pode não perceber se o botão for pressionado
}

void led_rgb(char comando){
  // switch(comando){
  //   case '1':
  //   analogWrite(pinGreen,n * 0);  
  //   analogWrite(pinRed, n * 255);  
  //   analogWrite(pinBlue, n *0);     
        
  //   break;
  //   case'2':
  //   analogWrite(pinGreen, n * 255);  
  //   analogWrite(pinRed, n *0);       
  //   analogWrite(pinBlue, n *0); 
  //   break;

  //   case'3':
  //    analogWrite(pinGreen, n *0);  
  //   analogWrite(pinRed,n * 0);    
  //     analogWrite(pinBlue, n * 255);    
   
  //   break;
  //    case'4':
  //     analogWrite(pinBlue, 0);     
  //     analogWrite(pinRed, n * 255);   
  //     analogWrite(pinGreen, n * 255);      
   
  //   break;
  //    case'5':
  //    analogWrite(pinBlue, n * 255);  
  //     analogWrite(pinRed, 0);          
  //     analogWrite(pinGreen, n * 255);    
   
  //   break;
  //    case'6':
  //    analogWrite(pinBlue, n * 255); 
  //     analogWrite(pinRed, n * 255);   
  //     analogWrite(pinGreen, 0);      
   
  //   break;
  //   case'7':
  //    analogWrite(pinBlue, n * 255); 
  //     analogWrite(pinRed, n * 255);   
  //     analogWrite(pinGreen, n * 255);      
   
  //   break;
  //   case'0':
  //    analogWrite(pinGreen, n *0);  
  //   analogWrite(pinRed,n * 0);    
  //     analogWrite(pinBlue, n * 0);    
   
  //   break;

  // }

    
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
void touch(){
  touchState = digitalRead(2); // Lê o sensor de toque
  
  if (touchState == HIGH){
    analogWrite(pinBlue, 0);  
    analogWrite(pinRed, 0);  
    analogWrite(pinGreen, 0); 
  }
    if (touchState >= 3){
      touchState == 0; 
      n == 0.1;
    }
  if (touchState == 1) {
         n = 0.3;
  }
   
   if (touchState ==2){
    n = 0.6;
   }
     if (touchState ==3){
    n = 1;
   }

    

}

