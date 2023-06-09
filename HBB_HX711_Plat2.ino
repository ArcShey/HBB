/*
Use two HX711 modules to measure and correct 
the errors generated by the sensor due to angle changes.
E+(red)    VCC
E-(black)  DAT 
A-(white)  CLK
A+(green)  GND

*/
#include <LiquidCrystal_PCF8574.h>
#include "HX711.h"

const byte SCK_PIN_m = 52; //blue
const byte DT_PIN_m = 50; // green
const byte PIN_CLK = 8;   // define CLK pin (any digital pin)
const byte PIN_DIO = 9; 
const byte PIN_BTN = 2; 
long weight_m; // measurement
int cycle = 0;
long diff = 0;
long total = 0;
long total_last = 0;
float toque = 0;
bool clear = false;
unsigned long wait;

unsigned long time_stamp = millis();
// unsigned long last_btn = millis();

// GyverTM1637 disp(PIN_CLK, PIN_DIO);

const int scale_factor_m = 221;
// const int scale_factor_c = -489;

LiquidCrystal_PCF8574 lcd(0x27);
HX711 scale_m;
// HX711 scale_c;

void setup() {
  Serial.begin(9600);
  pinMode(PIN_BTN, INPUT_PULLUP); // 
  lcd.begin(16, 2); // initialize the lcd
  lcd.setBacklight(100);
  lcd.home();
  lcd.clear();
  lcd.noCursor();
  lcd.setCursor(0, 0);
  lcd.print("  Initialing... ");
  
  scale_m.begin(DT_PIN_m, SCK_PIN_m);
  scale_m.set_scale(scale_factor_m);       // 設定比例參數
  scale_m.tare();				        // 歸零
  attachInterrupt(digitalPinToInterrupt(PIN_BTN), reset_value, FALLING);
  lcd.setCursor(0, 0);
  lcd.print("                ");
}

void resetDiff(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("     Reset      ");
  diff = weight_m;
  clear = false;
}
void reset_value(){
  clear = true;
}

// #include <string>

String padWithZerosInt(int number, int d) {
  String result = String(number);
  while (result.length() < d) {
    result = " " + result;
  }
  return result;
}

String padWithZerosFloat(float number, int d) {
  String result = String(number);
  while (result.length() < d) {
    result = " " + result;
  }
  return result;
}

void loop() {
  time_stamp = millis();
  weight_m = scale_m.get_units(3);
  // toque = int((weight_m - diff) * 26 / 100) / 10; // 無條件捨去至小數下一位
  toque = (weight_m - diff) * 26;
  toque /= 1000;

  lcd.setCursor(0, 0);
  lcd.print("W: " + padWithZerosInt(weight_m-diff, 4) + " / " + padWithZerosInt(weight_m, 4) + " g");
  lcd.setCursor(0, 1);
  lcd.print("T: " + padWithZerosFloat(toque, 6) + " Kg/cm");
  // lcd.print("Torque: " + String(toque) + "Kg/cm");

  Serial.println(toque);
  
  while(millis() < wait){}
  wait = millis() + 500;
  if (clear == true){
    resetDiff();
  }

}
