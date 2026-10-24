

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(
SCREEN_WIDTH,
SCREEN_HEIGHT,
&Wire,
OLED_RESET
);

// Rotary encoder pins
#define ENCODER_A 18
#define ENCODER_B 19
#define ENCODER_SW 23

// Spin coater settings
int rpm = 3000;
int spinTime = 30;

// 0 = RPM, 1 = TIME
int selectedSetting = 0;

// Spin coater status
bool running = false;

// Timer
unsigned long startTime = 0;

// Encoder variables
int lastA;
unsigned long lastEncoderTime = 0;

// Button variables
bool lastButtonState = HIGH;
unsigned long buttonPressedTime = 0;
bool longPressHandled = false;

void readEncoder();
void readButton();
void updateScreen();

void setup() {

Serial.begin(115200);

// Encoder setup
pinMode(ENCODER_A, INPUT_PULLUP);
pinMode(ENCODER_B, INPUT_PULLUP);
pinMode(ENCODER_SW, INPUT_PULLUP);

lastA = digitalRead(ENCODER_A);

// OLED setup
Wire.begin(21, 22);

if (!display.begin(
SSD1306_SWITCHCAPVCC, 0x3C)) {

  Serial.println("OLED not found");

while (true) {
  delay(100);
}
}


display.clearDisplay();
display.setTextColor(SSD1306_WHITE);

display.setTextSize(1);
display.setCursor(15, 20);
display.println("SPIN COATER");

display.setCursor(25, 35);
display.println("STARTING...");

display.display();

delay(1500);
}

void loop() {

  readEncoder();
  readButton();
  updateScreen();

}

// READ ROTARY ENCODER
void readEncoder() {

int currentA = digitalRead(ENCODER_A);

if (currentA != lastA &&
currentA == LOW &&
millis() - lastEncoderTime > 3) {

lastEncoderTime = millis();

if (!running) {

int direction;

if (digitalRead(ENCODER_B) == HIGH) {
direction = 1;
} else {
direction = -1;
}

if (selectedSetting == 0) {

rpm += direction * 100;
rpm = constrain(rpm, 500, 6000);

} else {

spinTime += direction * 5;
spinTime = constrain(spinTime, 5, 180);

}

}
}

lastA = currentA;
}

// READ BUTTON
void readButton() {

bool buttonState = digitalRead(ENCODER_SW);

if (buttonState == LOW &&
lastButtonState == HIGH) {

buttonPressedTime = millis();
longPressHandled = false;

}

// Long press
if (buttonState == LOW &&
!longPressHandled &&
millis() - buttonPressedTime >= 900) {

longPressHandled = true;

running = !running;

if (running) {
startTime = millis();
}

}

// Short press
if (buttonState == HIGH &&
lastButtonState == LOW) {

unsigned long pressDuration =
millis() - buttonPressedTime;

if (pressDuration > 30 &&
pressDuration < 900 &&
!running) {

    selectedSetting = 1 - selectedSetting;

    }
  }

  lastButtonState = buttonState;
}

// UPDATE OLED
void updateScreen() {

display.clearDisplay();
display.setTextSize(1);

display.setCursor(20, 0);
display.println("SPIN COATER");

display.drawLine(0, 11, 128, 11, SSD1306_WHITE);

display.setCursor(0, 17);

if (selectedSetting == 0 && !running) {
  display.print("> ");
  } else {
  display.print(" ");
}

display.print("RPM: ");
display.println(rpm);

display.setCursor(0, 30);

if (selectedSetting == 1 && !running) {
  display.print("> ");
  } else {
  display.print(" ");
}

display.print("TIME: ");

if (running) {

  unsigned long elapsed =
  (millis() - startTime) / 1000;

  int remaining = spinTime - elapsed;

  if (remaining <= 0) {
    remaining = 0;
    running = false;
  }

  display.print(remaining);

  } else {

  display.print(spinTime);

  }

  display.println(" sec");

  display.drawLine(0, 45, 128, 45, SSD1306_WHITE);

  display.setCursor(0, 52);

  if (running) {

  display.println("RUNNING - HOLD STOP");

  } else {

  display.println("READY - HOLD START");

  }

  display.display();

  delay(20);
}