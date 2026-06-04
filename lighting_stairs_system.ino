#include <Wire.h>
#include <LiquidCrystal_I2C.h>#include <Keypad.h>
#include <Servo.h>
// LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);
// Servo
Servo gateServo;
int servoPin = 14;
unsigned long gateOpenTime = 0;
bool gateOpen = false;
bool gateByStarKey = false;
// Keypad
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
{'1','2','3','A'},
{'4','5','6','B'},
{'7','8','9','C'},
{'*','0','#','D'}
};
byte rowPins[ROWS] = {2, 3, 4, 5};
byte colPins[COLS] = {6, 7, 8, 9};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
// IR Sensors and LEDs
int irPins[5] = {A0, A1, A2, A3, A4};
int bulbPins[4] = {10, 11, 12, 13};
unsigned long ledTimers[4] = {0, 0, 0, 0};
bool ledStates[4] = {false, false, false, false};
const unsigned long ledDelay = 2000; // 2 seconds
// Password
String password = "1234";
String input = "";
bool accessGranted = false;
bool stairsActive = false;
bool systemResetRequested = false;
void setup() {
Serial.begin(9600);
lcd.init();
lcd.backlight();
lcd.setCursor(0, 0);
lcd.print("Enter Password:");
gateServo.attach(servoPin);
gateServo.write(120); // Closed
for (int i = 0; i < 5; i++) pinMode(irPins[i], INPUT);
for (int i = 0; i < 4; i++) {
pinMode(bulbPins[i], OUTPUT);
digitalWrite(bulbPins[i], LOW);
}
}
void loop() {
char key = keypad.getKey();
if (key) {
if (!accessGranted) {
handlePassword(key);
handlePassword(key);} 
else {
if (key == '*') {
stairsActive = false;
lcd.clear();
lcd.print("Stairs Deactivated");
turnOffAllLEDs();
gateServo.write(20); // Open gate
gateOpenTime = millis();
gateByStarKey = true;
gateOpen = true;
systemResetRequested = true;
}
}
}
// Handle auto-close gate after 10s (normal) or 5s (* key)
if (gateOpen) {
if ((gateByStarKey && millis() - gateOpenTime >= 5000) || (!gateByStarKey && millis()
- gateOpenTime >= 10000)) {
gateServo.write(120); // Close gate
gateOpen = false;
gateByStarKey = false;
if (systemResetRequested) {
lcd.clear();
lcd.print("Enter Password:");
accessGranted = false;
input = "";
stairsActive = false;
systemResetRequested = false;
}
}
}
if (stairsActive) {
handleIRSensors();
updateLEDStates();
}
}
void handlePassword(char key) {
if (key == '#') {
if (input == password) {
lcd.clear();
lcd.print("Access Granted");
gateServo.write(20);
accessGranted = true;
stairsActive = true;
gateOpen = true;
gateOpenTime = millis();
delay(1000);
lcd.clear();
lcd.print("Stairs Activated");
} else {
lcd.clear();
lcd.print("Wrong Password");
delay(1500);
lcd.clear();
lcd.print("Enter Password:");
}
input = "";
} else if (key == '*') {input = "";
lcd.setCursor(0, 1);
lcd.print("
");
} else {
if (input.length() < 16) {
input += key;
lcd.setCursor(0, 1);
lcd.print(input);
}
}
}
void handleIRSensors() {
if (digitalRead(irPins[0]) == LOW) {
lightLED(0);
} else if (digitalRead(irPins[1]) == LOW) {
lightLED(0);
lightLED(1);
} else if (digitalRead(irPins[2]) == LOW) {
lightLED(0);
lightLED(1);
lightLED(2);
} else if (digitalRead(irPins[3]) == LOW) {
lightLED(2);
lightLED(3);
lightLED(1);
} else if (digitalRead(irPins[4]) == LOW) {
lightLED(3);
lightLED(2);
}
}
void lightLED(int index) {
digitalWrite(bulbPins[index], HIGH);
ledTimers[index] = millis();
ledStates[index] = true;
}
void updateLEDStates() {
for (int i = 0; i < 4; i++) {
if (ledStates[i] && (millis() - ledTimers[i] > ledDelay)) {
digitalWrite(bulbPins[i], LOW);
ledStates[i] = false;
}
}
}
void turnOffAllLEDs() {
for (int i = 0; i < 4; i++) {
digitalWrite(bulbPins[i], LOW);
ledStates[i] = false;
}}

