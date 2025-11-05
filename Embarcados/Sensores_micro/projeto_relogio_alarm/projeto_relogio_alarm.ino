#include <TFT.h>  // Arduino LCD library
#include <SPI.h>
#include <TimeLib.h>
#include <TimeAlarms.h>
const int buzzer = 7; //buzzer to arduino pin 9
// pin definition for the Uno
#define cs   10
#define dc   9
#define rst  8  

TFT TFTscreen = TFT(cs, dc, rst);

// buffer para exibir a hora
char timeBuffer[9]; // HH:MM:SS\0 = 8 + 1

void setup() {
  Serial.begin(9600);
  setTime(12,55,0,1,1,11); // set time to Saturday 8:29:00am Jan 1 2011
  pinMode(buzzer, OUTPUT); // Set buzzer - pin 9 as an output
  // create the alarms
  Alarm.alarmRepeat(8,30,0, MorningAlarm);  // 8:30am every day
  Alarm.alarmRepeat(12,55,3,EveningAlarm);  // 5:45pm every day
  Alarm.alarmRepeat(dowSaturday,8,30,30,WeeklyAlarm);  // 8:30:30 every Saturday
  // Alarm.timerRepeat(5, Repeats);            // timer for every 15 seconds   


  Alarm.timerOnce(10, OnceOnly);             // called once after 10 seconds

  TFTscreen.begin();
  TFTscreen.background(255, 255, 255);
  TFTscreen.stroke(0,0,0);
  TFTscreen.setTextSize(3);
}

void loop() {
  // Monta a string de hora: HH:MM:SS
  sprintf(timeBuffer, "%02d:%02d:%02d", hour(), minute(), second());
  Serial.println(timeBuffer);

  digitalClockDisplay();
  // Mostra a hora na tela
  TFTscreen.stroke(0,0,0);
  TFTscreen.text(timeBuffer, 10, 60);  // imprime

  Alarm.delay(1000); // wait one second between clock displ
  TFTscreen.stroke(255, 255, 255);
  TFTscreen.text(timeBuffer, 10, 60);  // apaga

  // Mostra também no monitor serial
 
 


}

// functions to be called when an a45larm triggers:
void MorningAlarm(){
 Serial.println("Alarm: - turn lights off");   
}


void EveningAlarm(){
 Serial.println("Alarm: - turn lights on");  
 tone(buzzer, 15); // Send 1KHz sound signal...
 Alarm.delay(1000);        // ...for 1 sec
 noTone(buzzer);     // Stop sound...
  
}


void WeeklyAlarm(){
 Serial.println("Alarm: - its Monday Morning");     
}


void ExplicitAlarm(){
 Serial.println("Alarm: - this triggers only at the given date and time");      
}


void Repeats(){
 Serial.println("55 second timer");    
 tone(buzzer, 15); // Send 1KHz sound signal...
 Alarm.delay(1000);        // ...for 1 sec
 noTone(buzzer);     // Stop sound...    
}


void OnceOnly(){
 Serial.println("This timer only triggers once"); 
 tone(buzzer, 15); // Send 1KHz sound signal...
 Alarm.delay(1000);        // ...for 1 sec
 noTone(buzzer);     // Stop sound...    
}


void digitalClockDisplay()
{
 // digital clock display of the time
 Serial.print(hour());
 printDigits(minute());
 printDigits(second());
 Serial.println();
}


void printDigits(int digits)
{
 Serial.print(":");
 if(digits < 10)
   Serial.print('0');
 Serial.print(digits);
}
