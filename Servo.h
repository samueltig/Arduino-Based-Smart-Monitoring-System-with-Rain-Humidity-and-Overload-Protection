#include <Servo.h>
#include <LiquidCrystal.h>
#include <DHT.h>

// -------------------- SERVO --------------------
Servo myServo;

// -------------------- LCD --------------------
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

// -------------------- DHT SENSOR --------------------
#define DHTPIN 12
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

// -------------------- PINS --------------------
const int SENSOR_PIN = 8;     // Rain sensor
const int SERVO_PIN = 9;      // Servo motor
const int FAN_PIN = 10;       // Fan motor
const int BUZZER_PIN = 11;    // Speaker
const int WEIGHT_PIN = A0;    // Potentiometer
const int OVERRIDE_PIN = 13;  // Emergency button

void setup() {

  pinMode(SENSOR_PIN, INPUT);

  pinMode(FAN_PIN, OUTPUT);

  pinMode(BUZZER_PIN, OUTPUT);

  pinMode(OVERRIDE_PIN, INPUT_PULLUP);

  myServo.attach(SERVO_PIN);

  myServo.write(0);

  dht.begin();

  lcd.begin(16, 2);

  lcd.setCursor(0, 0);
  lcd.print("System Starting");

  delay(2000);

  lcd.clear();
}

void loop() {

  // =================================================
  // MANUAL OVERRIDE BUTTON
  // =================================================

  int buttonState = digitalRead(OVERRIDE_PIN);

  // =================================================
  // MANUAL OVERRIDE MODE
  // =================================================

  if (buttonState == LOW) {

    myServo.write(90);

    digitalWrite(FAN_PIN, LOW);

    noTone(BUZZER_PIN);

    lcd.setCursor(0, 0);
    lcd.print("MANUAL OVERRIDE");

    lcd.setCursor(0, 1);
    lcd.print("System Paused ");

  }

  // =================================================
  // AUTOMATIC MODE
  // =================================================

  else {

    int rainState = digitalRead(SENSOR_PIN);

    int weightValue = analogRead(WEIGHT_PIN);

    // Read humidity
    float humidity = dht.readHumidity();

    // Safety if DHT fails
    if (isnan(humidity)) {

      humidity = 0;
    }

    // Convert potentiometer value to weight
    int weight = map(weightValue, 0, 1023, 0, 50);

    // =================================================
    // OVERLOAD PROTECTION
    // =================================================

    if (weight > 30) {

      myServo.write(90);

      digitalWrite(FAN_PIN, LOW);

      // Continuous alarm sound
      tone(BUZZER_PIN, 1000);

      lcd.setCursor(0, 0);
      lcd.print("!!! OVERLOAD !!!");

      lcd.setCursor(0, 1);
      lcd.print("Reduce Weight  ");
    }

    // =================================================
    // NORMAL SYSTEM OPERATION
    // =================================================

    else {

      noTone(BUZZER_PIN);

      // Rain OR High Humidity
      if (rainState == LOW || humidity > 70.0) {

        myServo.write(90);

        digitalWrite(FAN_PIN, HIGH);

        lcd.setCursor(0, 0);

        if (rainState == LOW) {

          lcd.print("Rain Detected  ");
        }

        else {

          lcd.print("High Humidity  ");
        }
      }

      // Normal weather condition
      else {

        myServo.write(0);

        digitalWrite(FAN_PIN, LOW);

        lcd.setCursor(0, 0);

        lcd.print("Status: Normal ");
      }

      // =================================================
      // DISPLAY WEIGHT & HUMIDITY
      // =================================================

      if ((millis() / 2000) % 2 == 0) {

        lcd.setCursor(0, 1);

        lcd.print("Weight: ");

        lcd.print(weight);

        lcd.print(" kg   ");
      }

      else {

        lcd.setCursor(0, 1);

        lcd.print("Humidity: ");

        lcd.print((int)humidity);

        lcd.print("%   ");
      }
    }
  }

  delay(300);
}
