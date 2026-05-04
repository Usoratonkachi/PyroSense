#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 10, 9, 7, 6);

// ===== PINOK =====
int tempPin = A0;
int windPin = A1;
int moisturePin = A2;

int redPin = 3;
int greenPin = 5;

int screen = 0;
unsigned long lastSwitch = 0;

bool ledState = false;

// ===== LED =====
void setColor(bool red, bool green) {
  digitalWrite(redPin, red);
  digitalWrite(greenPin, green);
}

void setup() {
  lcd.begin(16, 2);

  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
}

void loop() {

  // ===== MÉRÉS =====
  int tempRaw = analogRead(tempPin);
  float voltage = tempRaw * (5.0 / 1023.0);
  float temp = (voltage - 0.5) * 100;

  int wind = analogRead(windPin);
  int moist = analogRead(moisturePin);

  // ===== SKÁLÁZÁS =====
  int humidity = map(moist, 0, 1023, 100, 0); // fordított!
  int windKmh = map(wind, 0, 1023, 0, 60);

  // ===== SZÖVEGES BESOROLÁS =====
  String windText;
  if (windKmh < 10) windText = "Light";
  else if (windKmh < 20) windText = "Moderate";
  else windText = "Strong";

  String humText;
  if (humidity > 40) humText = "Moist";
  else if (humidity > 30) humText = "Normal";
  else if (humidity > 20) humText = "Dry";
  else humText = "Very Dry";

  // ===== DÖNTÉSI LOGIKA =====
  int state = 0;

  // ❗ FAIL-SAFE
  if (temp >= 60) {
    state = 3;
  }
  // 🔴 EXTREM
  else if (temp >= 35 && humidity <= 15 && windKmh >= 30) {
    state = 3;
  }
  // 🟠 MAGAS
  else if (temp >= 32 && humidity <= 25 && windKmh >= 20) {
    state = 2;
  }
  // 🟡 KEZDETI KOCKÁZAT
  else if (temp >= 30 && humidity <= 30 && windKmh >= 15) {
    state = 1;
  }
  else {
    state = 0;
  }

  // ===== LED =====
  if (state == 0) {
    setColor(0, 1); // zöld
  }
  else if (state == 1) {
    setColor(1, 1); // sárga
  }
  else if (state == 2) {
    setColor(1, 0); // piros
  }
  else {
    ledState = !ledState;
    setColor(ledState, 0); // villogó piros
  }

  // ===== KIJELZŐ VÁLTÁS =====
  if (millis() - lastSwitch > 2000) {
    screen++;
    if (screen > 2) screen = 0;
    lastSwitch = millis();
    lcd.clear();
  }

  // ===== LCD =====
  lcd.setCursor(0, 0);

  if (screen == 0) {
    lcd.print("Temp:");
    lcd.print(temp, 1);
    lcd.print("C");
  }
  else if (screen == 1) {
    lcd.print("Wind:");
    lcd.print(windText);
  }
  else {
    lcd.print("Hum:");
    lcd.print(humText);
  }

  lcd.setCursor(0, 1);

  if (state == 0) {
    lcd.print("Alacsony");
  }
  else if (state == 1) {
    lcd.print("Kockazat");
  }
  else if (state == 2) {
    lcd.print("MAGAS");
  }
  else {
    lcd.print("!!! EXTREM !!!");
  }

  delay(300);
}