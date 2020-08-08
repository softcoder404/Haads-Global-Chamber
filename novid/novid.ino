#include <LiquidCrystal.h>
#include <EEPROM.h>
LiquidCrystal lcd(13, 12, 11, 10, 9, 8);
#define trigPin 2
#define echoPin 3
#define timer 5
#define buzzer 6
#define pump 7


long frameDistance = 0;
long passCount = 0;
bool derisPassFlg = false;
bool detectMove = false;

long current, previous = 0;
bool displayTitle = true;
long getLastCount();
void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  pinMode(buzzer, OUTPUT);
  pinMode(pump, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(timer, INPUT_PULLUP);
  digitalWrite(buzzer, LOW);
  digitalWrite(pump, LOW);
  lcd.clear();
  lcd.print("NOVID CONTROLLER");
  lcd.setCursor(0, 1);
  lcd.print("Starting Up.....");

  passCount = getLastCount();
  if (passCount == 25000) {
    // formatEEPROM();
  }
  //format eeprom
  int c = 0;
  while (c < 5) {
    lcd.setCursor(0, 1);
    lcd.print("Init Frame Dist.");
    frameDistance = takeDistance();
    c++;
    delay(500);
    lcd.setCursor(0, 1);
    lcd.print("                ");
    delay(500);
  }
  lcd.clear();
  lcd.print("Frame Dist Taken");
  lcd.setCursor(0, 1);
  lcd.print("Frame Dist:" + String(frameDistance));
  delay(1000);
  lcd.clear();

}

void loop() {
  performMajorTask();
}


void performMajorTask() {
  current = millis();
  if (current - previous >= 2000) {
    displayTitle = !displayTitle;
    lcd.clear();
    previous = current;
  }

  long _distance = takeDistance();
  String firstLine = "HAADS-GLOBAL NIG";
  String title = "NOVID CONTROLLER";
  String secondLine = "Pass Count:" + String(passCount);
  if (displayTitle) {
    lcd.setCursor(0, 0);
    lcd.print(title);
    lcd.setCursor(0, 1);
    lcd.print(secondLine);
  } else {
    lcd.setCursor(0, 0);
    lcd.print(firstLine);
    lcd.setCursor(0, 1);
    lcd.print(secondLine);
  }
  monitorMovement(_distance);
  if (derisPassFlg) {
    passCount++;
    digitalWrite(buzzer, HIGH);
    digitalWrite(pump, HIGH);
    delay(digitalRead(timer) == LOW ? 3000 : 2000);
    digitalWrite(pump, LOW );
    digitalWrite(buzzer, LOW);
    saveData(passCount);
    derisPassFlg = false;
    detectMove = false;
  }
}

long takeDistance() {
  long duration, distance;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration / 2) / 29.1;
  return distance;
}


void monitorMovement(long dist) {
  if (dist + 8 < frameDistance && detectMove == false) {
    detectMove = true;
  }
  if (dist >= frameDistance - 8 && detectMove == true) {
    derisPassFlg = true;
  }
}

long getLastCount() {
  long num = 0;
  for (int i = 0; i <= 100; i++) {
    num += EEPROM.read(i);
    delay(50);
  }
  return num;
}

void saveData(long dataVal) {
  if (dataVal <= 255) {
    EEPROM.write(0, dataVal);

    delay(200);
    Serial.println("data saved successfully!");
  } else {
    if (dataVal % 255 > 0) {
      //if deris remainder
      int r = dataVal % 255;
      int addr = floor( dataVal / 255);
      EEPROM.write(addr, r);

      delay(100);
      for (int i = 0; i < addr; i++) {
        EEPROM.write(i, 255);

        Serial.println("data saved successfully!");
      }
    } else {
      //if remainder is zero
      for (int i = 0; i < dataVal / 255; i++ ) {
        EEPROM.write(i, 255);

        delay(100);

      }
      Serial.println("data saved successfully!");
    }
  }
}
