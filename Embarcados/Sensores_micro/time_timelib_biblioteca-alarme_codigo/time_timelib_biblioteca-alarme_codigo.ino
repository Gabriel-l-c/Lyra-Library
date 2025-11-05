#include <Time.h>
#include <TimeAlarms.h>

const int buzzer = 8; // buzzer to arduino pin 8

void setup() {
  Serial.begin(9600);
  setTime(12, 55, 0, 1, 1, 11); // set time to Saturday 8:29:00am Jan 1 2011
  pinMode(buzzer, OUTPUT); // Set buzzer - pin 8 as an output
  digitalWrite(buzzer, HIGH);
  // Create the alarms
  Alarm.alarmRepeat(8, 30, 0, MorningAlarm);  // 8:30am every day
  Alarm.alarmRepeat(12, 55, 3, EveningAlarm);  // 12:55:03pm every day
  Alarm.alarmRepeat(dowSaturday, 8, 30, 30, WeeklyAlarm);  // 8:30:30 every Saturday
  Alarm.timerRepeat(5, Repeats); // timer for every 5 seconds
  Alarm.timerOnce(10, OnceOnly); // called once after 10 seconds
}

void loop() {
  digitalClockDisplay();
  Alarm.delay(1000); // wait one second between clock display
}

// Functions to be called when an alarm triggers:
void MorningAlarm() {
  Serial.println("Alarm: - turn lights off");
}

void EveningAlarm() {
  Serial.println("Alarm: - turn lights on");  
  tone(buzzer, 1000); // Send 1KHz sound signal...
  delay(1000); // play sound for 1 second
  noTone(buzzer); // Stop sound...
}

void WeeklyAlarm() {
  Serial.println("Alarm: - it's Saturday Morning");
}

void ExplicitAlarm() {
  Serial.println("Alarm: - this triggers only at the given date and time");      
}

void Repeats() {
  Serial.println("55 second timer");
  tone(buzzer, 1000); // Send 1KHz sound signal...
  delay(1000);        // play sound for 1 second
  noTone(buzzer);     // Stop sound...
}

void OnceOnly() {
  Serial.println("This timer only triggers once");
}

void digitalClockDisplay() {
  // digital clock display of the time
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.println();
}

void printDigits(int digits) {
  Serial.print(":");
  if (digits < 10)
    Serial.print('0');
  Serial.print(digits);
}
