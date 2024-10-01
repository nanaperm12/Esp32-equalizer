KODE ESP32 EQUALIZER TDA8425 SILAHKAN COPY KODE INI UNTUK MEMBUAT EQUALIZER DIGITAL

#include <Wire.h>
#include <LiquidCrystal_I2C.h> 

LiquidCrystal_I2C lcd(0x27, 16, 2); 

// Alamat TDA8425
const byte tda8425Address = 0x40; 

// Pin rotary encoder
#define CLK 26
#define DT 27
#define SW 14

int volume = 50;  
int bass = 7;     
int treble = 7;   
int inputSource = 2;  
int mode = 0;     

unsigned long lastEncoderDebounceTime = 0; 
unsigned long lastButtonDebounceTime = 0;  
unsigned long debounceDelayEncoder = 10;   
unsigned long debounceDelayButton = 100;   

int lastClk = HIGH;
int lastButtonState = HIGH;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  lcd.init();
  lcd.backlight();

  pinMode(CLK, INPUT);
  pinMode(DT, INPUT);
  pinMode(SW, INPUT_PULLUP);
  
  // Tampilkan pengaturan awal di LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Vol:");
  lcd.print(volume);

  lcd.setCursor(9, 0);
  lcd.print("Input:");
  lcd.print(inputSource);
  
  lcd.setCursor(0, 1);
  lcd.print("Bass:");
  lcd.print(bass);
  
  lcd.setCursor(9, 1);
  lcd.print("High:");
  lcd.print(treble);
  
  // Pilih input awal
  writeTDA8425(0x02, inputSource); 
}

void loop() {
  // Baca rotary encoder untuk perubahan dengan debounce
  int currentClk = digitalRead(CLK);
  if ((millis() - lastEncoderDebounceTime) > debounceDelayEncoder) {
    if (currentClk != lastClk) {
      if (currentClk == LOW) {
        if (digitalRead(DT) != currentClk) {
          adjustValue(1);  
        } else {
          adjustValue(-1); 
        }
        updateLCD();
        lastEncoderDebounceTime = millis(); 
      }
    }
    lastClk = currentClk;
  }
  
  // Baca tombol rotary encoder dengan debounce
  int buttonState = digitalRead(SW);
  if ((millis() - lastButtonDebounceTime) > debounceDelayButton) {
    if (buttonState == LOW && lastButtonState == HIGH) {
      mode = (mode + 1) % 4; 
      updateLCD();
      lastButtonDebounceTime = millis(); 
    }
    lastButtonState = buttonState;
  }
}

void adjustValue(int change) {
  switch (mode) {
    case 0: 
      volume = constrain(volume + change, 0, 63);
      writeTDA8425(0x00, volume); 
      writeTDA8425(0x01, volume); 
      break;
    case 1: 
      bass = constrain(bass + change, 0, 15);
      writeTDA8425(0x02, bass); 
      break;
    case 2: 
      treble = constrain(treble + change, 0, 15);
      writeTDA8425(0x03, treble); 
      break;
    case 3: 
       inputSource = constrain(inputSource + change, 0, 1); 
       writeTDA8425(0x04, inputSource); 
       break;
  }
}

void updateLCD() {
  lcd.clear();
  if (mode == 0) {
    lcd.setCursor(0, 0);
    lcd.print("Volume: ");
    lcd.print(volume);
  } else if (mode == 1) {
    lcd.setCursor(0, 0);
    lcd.print("Bass: ");
    lcd.print(bass);
  } else if (mode == 2) {
    lcd.setCursor(0, 0);
    lcd.print("Treble: ");
    lcd.print(treble);
  } else if (mode == 3) {
    lcd.setCursor(0, 0);
    lcd.print("Input: ");
    lcd.print(inputSource == 0 ? "Input 1" : "Input 2");
  }
}

void writeTDA8425(byte controlByte, byte value) {
  Serial.print("Writing to TDA8425: Control byte = ");
  Serial.print(controlByte, HEX);
  Serial.print(", Value = ");
  Serial.println(value);

  Wire.beginTransmission(tda8425Address);
  Wire.write(controlByte); // Register control
  Wire.write(value);       // Nilai parameter
  byte error = Wire.endTransmission();

  if (error == 0) {
    Serial.println("Success!");
  } else {
    Serial.print("Error occurred: ");
    Serial.println(error);
    
    if (error == 2) {
      Serial.println("Error 2: Address not found.");
    } else if (error == 3) {
      Serial.println("Error 3: NACK received on data.");
    } else if (error == 4) {
      Serial.println("Error 4: Other error (timeout, etc.).");
    }
  }
}
