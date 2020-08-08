void setup() {
  // put your setup code here, to run once:

}

void loop() 
  // put your main code here, to run repeatedly:

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
