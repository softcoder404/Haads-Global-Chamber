void setup() {
  // put your setup code here, to run once:

}

void loop() 
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
  String firstLine = "HAADS-GLOBAL NIG"
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
    digitalWrite(buzzer,LOW);
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
    num += EEPROM.read(0x0F + i);
    delay(50);
  }
  return num;
}

void saveData(long dataVal) {
  if (dataVal <= 255) {
    EEPROM.write(0x0F + 0, dataVal);
    EEPROM.commit();
    delay(200);
    Serial.println("data saved successfully!");
  } else {
    if (dataVal % 255 > 0) {
      //if deris remainder
      int r = dataVal % 255;
      int addr = floor( dataVal / 255);
      EEPROM.write(0x0F + addr, r);
      EEPROM.commit();
      delay(100);
      for (int i = 0; i < addr; i++) {
        EEPROM.write(0x0F + i, 255);
        EEPROM.commit();
        Serial.println("data saved successfully!");
      }
    } else {
      //if remainder is zero
      for (int i = 0; i < dataVal / 255; i++ ) {
        EEPROM.write(0x0F + i, 255);
        EEPROM.commit();
        delay(100);

      }
      Serial.println("data saved successfully!");
    }
  }
}
