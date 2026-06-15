#include <Wire.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define PIR_PIN 13
#define LED_PIN 12
#define BUZZER_PIN 27

LiquidCrystal_I2C lcd(0x27, 16, 2);

// PWM settings
#define BUZZER_FREQ 1000
#define BUZZER_RESOLUTION 8

// Time tracking
unsigned long ledOnStartTime = 0;
unsigned long ledOnDuration = 0;
bool ledState = false;

//  Beep interval tracking
unsigned long lastBeepTime = 0;

void setup() {
  pinMode(PIR_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);

  Serial.begin(115200);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("System Ready");

  // ESP32 PWM setup
  ledcAttach(BUZZER_PIN, BUZZER_FREQ, BUZZER_RESOLUTION);
}

void loop() {
  int sensorValue = digitalRead(PIR_PIN);

  if (sensorValue == HIGH) {
    //  Motion detected → LED ON, buzzer OFF
    digitalWrite(LED_PIN, HIGH);
    ledcWrite(BUZZER_PIN, 0);

    // Start timer when LED just turned ON
    if (!ledState) {
      ledOnStartTime = millis();
      ledState = true;
      lastBeepTime = millis(); // reset beep timer
    }

    //   Beep every 30 seconds
    unsigned long currentTime = millis();
    if (currentTime - lastBeepTime >= 30000) {
      ledcWrite(BUZZER_PIN, 128); 
      // ON
      delay(200);
      ledcWrite(BUZZER_PIN, 0);  
      // OFF

      lastBeepTime = currentTime;
    }

    lcd.setCursor(0, 0);
    lcd.print("StudyModeOn....");
  } 
  else {
    //  No motion → LED OFF, buzzer ON
    digitalWrite(LED_PIN, LOW);
    ledcWrite(BUZZER_PIN, 128);

    // Store ON duration
    if (ledState) {
      ledOnDuration = millis() - ledOnStartTime;
      ledState = false;
    }

    lcd.setCursor(0, 0);
    lcd.print("PowerSavingMode      ");
  }

  //  Display time
  lcd.setCursor(0, 1);

  if (ledState) {
    unsigned long seconds = (millis() - ledOnStartTime) / 1000;
    lcd.print("ON Time: ");
    lcd.print(seconds);
    lcd.print("s   ");
  } else {
    lcd.print("Last Goal: ");
    lcd.print(ledOnDuration / 1000);
    lcd.print("s   ");
  }

  delay(200);
}
