#define pin 13



int Humidity    = 0; 
int Temperature = 0; 
uint8_t sum = 0;
void setup() {
Serial.begin(9600);
}

void loop() {
readdht();
delay(3000);
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