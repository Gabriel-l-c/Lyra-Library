#include <PWMServo.h>  // Biblioteca PWMServo

const int pinGreen = 6;  // Pino do LED Verde
const int pinRed = 5;    // Pino do LED Vermelho
const int pinBlue = 3;   // Pino do LED Azul

const int pinX = A0;     // Eixo X do joystick
const int pinY = A1;     // Eixo Y do joystick
const int pinServo = 9;  // Pino do servo motor (usei 9 para evitar conflitos de PWM)
const int pinTouch = 4;  // Pino do botão touch

PWMServo servo1;  // Criando o objeto do servo

int brilhoMaximo = 255;
float n = 0.8;
int anguloServo = 90;

unsigned long tempoBotao = 0;
unsigned long debounceDelay = 200;

char comando = '7';
int estadotouch = 0;
int cont_luminosidade = 0;

void setup() {
  Serial.begin(9600);

  pinMode(pinRed, OUTPUT);
  pinMode(pinGreen, OUTPUT);
  pinMode(pinBlue, OUTPUT);
  pinMode(pinTouch, INPUT);

  servo1.attach(pinServo);  // Anexa o servo
  servo1.write(anguloServo); // Inicializa no meio

  // Inicia LED branco
  analogWrite(pinRed, 255);
  analogWrite(pinGreen, 255);
  analogWrite(pinBlue, 255);

  Serial.println("LED Branco ligado!");
  delay(100);
}

void loop() {
  if (Serial.available()) {
    char novoComando = Serial.read();
    if (novoComando != '\n' && novoComando != '\r') {  
      comando = novoComando;
    }
    led(comando);
  }

  estadotouch = digitalRead(pinTouch);
  if (estadotouch == 1){
    cont_luminosidade = (cont_luminosidade + 1) % 4;

    switch(cont_luminosidade){
      case 0: n = 0; break;
      case 1: n = 0.1; break;
      case 2: n = 0.5; break;
      case 3: n = 1; break;
    }
    led(comando);
  }

  int valorX = analogRead(pinX);

  // Mapeando o valor do joystick para ângulo do servo
  anguloServo = map(valorX, 0, 1023, 0, 180);
  servo1.write(anguloServo);

  Serial.print("Valor Joystick: ");
  Serial.println(valorX);
  Serial.print("Ângulo Servo: ");
  Serial.println(anguloServo);
  
  delay(200);
}

void led(char comando){
  if (comando == '1') {
    analogWrite(pinRed, n * 255);
    analogWrite(pinGreen, 0);
    analogWrite(pinBlue, 0);
  } else if (comando == '2') {
    analogWrite(pinGreen, n * 255);
    analogWrite(pinRed, 0);
    analogWrite(pinBlue, 0);
  } else if (comando == '3') {
    analogWrite(pinBlue, n * 255);
    analogWrite(pinRed, 0);
    analogWrite(pinGreen, 0);
  } else if (comando == '4') {
    analogWrite(pinBlue, 0);
    analogWrite(pinRed, n * 255);
    analogWrite(pinGreen, n * 255);
  } else if (comando == '5') {
    analogWrite(pinBlue, n * 255);
    analogWrite(pinRed, 0);
    analogWrite(pinGreen, n * 255);
  } else if (comando == '6') {
    analogWrite(pinBlue, n * 255);
    analogWrite(pinRed, n * 255);
    analogWrite(pinGreen, 0);
  } else if (comando == '7') {
    analogWrite(pinBlue, n * 255);
    analogWrite(pinRed, n * 255);
    analogWrite(pinGreen, n * 255);
  } else if (comando == '0') {
    analogWrite(pinBlue, 0);
    analogWrite(pinRed, 0);
    analogWrite(pinGreen, 0);
  } else {
    Serial.println("Digite um número válido (0 a 7):");
  }
}


















// // #include servo.h

// const int pinGreen = 6;    // Pino do LED Vermelho
// const int pinRed= 5;  // Pino do LED Verde
// const int pinBlue = 3;    // Pino do LED Azu
// // Definir os pinos onde o joystick está conectado
// const int pinX = A0;  // Eixo X do joystick no pino A0
// const int pinY = A1;  // Eixo Y do joystick no pino A1
// // definindo o pino do servo
// const int pinServo = 5;
// const int pinTouch = 4;


// int brilhoMaximo = 255;  // Intensidade do LED
// // Objeto do servo
// // Servo servo1;

// float n = 0.8;
// int anguloServo = 90;

// // Debounce - contagem de tempo
// unsigned long tempoBotao = 0;
// unsigned long debounceDelay = 200;  // delay

// char comando = '7';  // Variável para armazenar o comando recebido
// int comando2 = 9;
// int estadotouch = 0; 
// int cont_luminosidade = 0;


// void setup() {
//   Serial.begin(9600);  // Inicia a comunicação serial
//   // pinMode(pinBotao, INPUT_PULLUP);  // Configura o pino do botão com resistor pull-up
//   pinMode(pinRed, OUTPUT);
//   pinMode(pinGreen, OUTPUT);
//   pinMode(pinBlue, OUTPUT);
//   pinMode(pinTouch, INPUT);

//   // servo1.attach(pinServo);
//   // servo1.write(anguloServo);  // Posiciona o servo no meio
//   // Começa com branco
//   analogWrite(pinRed, 255);
//   analogWrite(pinGreen, 255);
//   analogWrite(pinBlue, 255);

//   Serial.println("LED Branco ligado!");

//   // Exibe a lista de opções
//   Serial.println("=== Controle do LED RGB ===");
//   Serial.println("Digite um número para selecionar a cor:");
//   Serial.println("1 - Vermelho");
//   Serial.println("2 - Verde");
//   Serial.println("3 - Azul");
//   Serial.println("4 - Amarelo");
//   Serial.println("5 - Ciano");
//   Serial.println("6 - Magenta");
//   Serial.println("7 - Branco");
//   Serial.println("0 - Desligar LED");
//   Serial.println("===========================");
//   // Aguarda um tempo para estabilizar


//   delay(100); // estabilidade
// }







// void loop() {

//   if (Serial.available()) {
//     char novoComando = Serial.read();  // Lê o novo comando
//     if (novoComando != '\n' && novoComando != '\r') {  
//       comando = novoComando;  // Atualiza apenas se for um caractere válido
//     }
//     // Serial.print("Comando atualizado para: ");
//     // Serial.println(comando);
//   }
//   // // Chama a função para ler a string
//   // if (Serial.available()) {
//   //   comando = Serial.read();  
//   //   Serial.print("-----------\n");
//   //   Serial.print(comando);
//   //   Serial.print("-----------\n");
//      led(comando);

    
//   // }


//   estadotouch = digitalRead(pinTouch);

//   if (estadotouch == 1){
    
//     if (cont_luminosidade <= 2){
     
//       cont_luminosidade += 1;
//     }else{
//       cont_luminosidade = 0;
//     }

//     switch(cont_luminosidade){
//       case 0:
//         n = 0;
//         led(comando);
//       break;
//       case 1: 
//          led(comando);
        
//           n = 0.1;
//       break;
//       case 2:
//           led(comando);
//           n = 0.5;
//       break;
//       case 3:
//           led(comando);
//            n = 1;
//       break;
      
//     }
    
//   }



//   // // Lê o estado do botão (0 = pressionado, 1 = não pressionado)
//   // int estadoBotao = digitalRead(pinBotao);
//   // tempoBotao = millis();
//   // // é para ciclar a intensidade pelo botão
//   // if (estadoBotao == 0 && (millis() - tempoBotao) > debounceDelay){
//   //   if (n == 0.4){
//   //       n = 0.8;
//   //   } else if (n == 0.8){
//   //       n = 1;
//   //   } else if (n == 1){
//   //       n = 0.4;
//   //   }
//   // }





//   // Aqui é sobre os valores dos eixos X e Y do joystick
//   // int valorX = analogRead(pinX);  // Lê o valor do eixo X
//   // int valorY = analogRead(pinY);  // Lê o valor do eixo Y
//   int valorX = analogRead(pinX);
//   // anguloServo = map(valorX, 0, 1023, 0, 180);
//   // servo1.write(anguloServo);
//   Serial.print(valorX);
//   Serial.println("\n");
//   Serial.print(valorX);
//   Serial.println("\n");


//   if (valorX >= 0 & valorX<100){
//       anguloServo = 100;
//   } else if (valorX >= 100 & valorX<200){
//       anguloServo = 145;
//   } else if (valorX >= 200 & valorX<300){
//       anguloServo = 90;
//   } else if (valorX >= 300 & valorX<400){
//       anguloServo = 80;
//   } else if (valorX >= 400 & valorX<500){
//       anguloServo = 70;
//   } else if (valorX >= 500 & valorX<600){
//       anguloServo = 60;
//   } else if (valorX >= 600 & valorX<700){
//       anguloServo = 50;
//   } else if (valorX >= 700 & valorX<800){
//       anguloServo = 40;
//   } else if (valorX >= 800 & valorX<900){
//       anguloServo = 30;
//   } else if (valorX >= 900 & valorX<1000){
//       anguloServo = 20;
//   } else if (valorX >= 1000){
//       anguloServo = 10;
//   }

//   //anguloServo = 100;
 
//   //servo1.write(90);
//   // estadotouch = digitalRead(pinTouch);
//   // if (estadotouch == 1 ){
//   //    analogWrite(pinBlue, 0);  
//   //     analogWrite(pinRed, 0);  
//   //     analogWrite(pinGreen, 0); 
//   // }
//   // // Exibe os valores no Monitor Serial
//   // Serial.print("X: ");
//   // Serial.print(valorX);  // Imprime o valor do eixo X
//   // Serial.print("\tY: ");
//   // Serial.print(valorY);  // Imprime o valor do eixo Y
//   // Serial.print("\tBotão: ");
//   // Serial.println(estadoBotao == LOW ? "Pressionado" : "Não pressionado"); // Imprime o estado do botão

//   // Exibe o ângulo do servo para debugging
//   // Serial.print("Angulo do Servo: ");
//   // Serial.println(anguloServo);

//   // Serial.println("");
//   // Serial.print(n);
//   //  Serial.println("");
//   // Serial.print(comando);
//   // Serial.print(cont_luminosidade);
//   // Serial.println("\n");

//   delay(200);  // Atraso para não sobrecarregar a serial, porém se for muito devagar, ou seja, o delay for alto, ele pode não perceber se o botão for pressionado
// }

// void led(char comando){

//   if (comando == '1') {
//       analogWrite(pinRed, n * 255);  
//       analogWrite(pinGreen, 0);     
//       analogWrite(pinBlue, 0);       
//       Serial.println("LED Vermelho ligado!");
//   } else if (comando =='2') {
//       analogWrite(pinGreen, n * 255);  
//       analogWrite(pinRed, 0);       
//       analogWrite(pinBlue, 0);      
//       Serial.println("LED Verde ligado!");
//   } else if (comando == '3') {
//       analogWrite(pinBlue, n * 255);  
//       analogWrite(pinRed, 0);        
//       analogWrite(pinGreen, 0);    
//       Serial.println("LED Azul ligado!");
//   } else if (comando == '4') {
//       Serial.println("Amarelo ligado!");
//       analogWrite(pinBlue, 0);     
//       analogWrite(pinRed, n * 255);   
//       analogWrite(pinGreen, n * 255);  
//   } else if (comando == '5') {
//       Serial.println("Ciano ligado!");
//       analogWrite(pinBlue, n * 255);  
//       analogWrite(pinRed, 0);          
//       analogWrite(pinGreen, n * 255);  
//   } else if (comando == '6') {
//       Serial.println("Magenta ligado!");
//       analogWrite(pinBlue, n * 255); 
//       analogWrite(pinRed, n * 255);   
//       analogWrite(pinGreen, 0);     
//   } else if (comando == '7') {
//       Serial.println("Branco ligado!");
//       analogWrite(pinBlue, n * 255); 
//       analogWrite(pinRed, n * 255);   
//       analogWrite(pinGreen, n * 255); 
//   } else if (comando == '0') {
//       Serial.println("LED desligado!");
//       analogWrite(pinBlue, 0);  
//       analogWrite(pinRed, 0);  
//       analogWrite(pinGreen, 0); 
//   } else {
//       Serial.println("Digite um número válido (0 a 9):");
//   }
      
// } 


