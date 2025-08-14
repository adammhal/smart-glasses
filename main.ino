#include <Wire.h>
#include <SoftwareSerial.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "RTClib.h"

SoftwareSerial bluetooth(2, 3);

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

RTC_DS3231 rtc;

String incomingData = "";
String currentNotification = "";
String currentMedia = "";
String currentWeather = "";
unsigned long lastDisplayTime = 0;
const int displayTimeout = 5000;

enum DisplayState {
  SHOW_TIME,
  SHOW_NOTIFICATION,
  SHOW_MEDIA,
  SHOW_WEATHER
};
DisplayState currentState = SHOW_TIME;

void setup() {
  Serial.begin(9600);
  
  bluetooth.begin(9600);
  Serial.println("Bluetooth ready.");

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.println("Smart Glasses ON");
  display.display();
  delay(2000);

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    display.clearDisplay();
    display.println("RTC Module Error");
    display.display();
    while (1);
  }

  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
}

void loop() {
  checkBluetooth();
  updateDisplay();
}

void checkBluetooth() {
  if (bluetooth.available()) {
    char receivedChar = bluetooth.read();
    if (receivedChar == '\n') {
      parseData(incomingData);
      incomingData = "";
    } else {
      incomingData += receivedChar;
    }
  }
}

void parseData(String data) {
  Serial.print("Received: ");
  Serial.println(data);

  if (data.startsWith("N:")) {
    currentNotification = data.substring(2);
    currentState = SHOW_NOTIFICATION;
    lastDisplayTime = millis();
  } else if (data.startsWith("M:")) {
    currentMedia = data.substring(2);
    currentState = SHOW_MEDIA;
    lastDisplayTime = millis();
  } else if (data.startsWith("W:")) {
    currentWeather = data.substring(2);
    currentState = SHOW_WEATHER;
    lastDisplayTime = millis();
  } else if (data.startsWith("T:")) {
    String timeData = data.substring(2);
    int year = timeData.substring(0, 4).toInt();
    int month = timeData.substring(5, 7).toInt();
    int day = timeData.substring(8, 10).toInt();
    int hour = timeData.substring(11, 13).toInt();
    int minute = timeData.substring(14, 16).toInt();
    int second = timeData.substring(17, 19).toInt();
    rtc.adjust(DateTime(year, month, day, hour, minute, second));
    Serial.println("Time updated via Bluetooth.");
  }
}

void updateDisplay() {
  if (currentState != SHOW_TIME && (millis() - lastDisplayTime > displayTimeout)) {
    currentState = SHOW_TIME;
  }

  switch (currentState) {
    case SHOW_TIME:
      displayTimeAndDate();
      break;
    case SHOW_NOTIFICATION:
      displayMessage("Notification", currentNotification);
      break;
    case SHOW_MEDIA:
      displayMessage("Media Playing", currentMedia);
      break;
    case SHOW_WEATHER:
      displayMessage("Weather", currentWeather);
      break;
  }
}

void displayTimeAndDate() {
  DateTime now = rtc.now();
  
  display.clearDisplay();
  
  display.setTextSize(2);
  display.setCursor(15, 10);
  if(now.hour() < 10) display.print("0");
  display.print(now.hour());
  display.print(":");
  if(now.minute() < 10) display.print("0");
  display.print(now.minute());
  
  display.setTextSize(1);
  display.setCursor(25, 40);
  display.print(now.month(), DEC);
  display.print('/');
  display.print(now.day(), DEC);
  display.print('/');
  display.print(now.year(), DEC);
  
  display.display();
  delay(1000);
}

void displayMessage(String title, String message) {
  display.clearDisplay();
  
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println(title);
  display.drawLine(0, 10, SCREEN_WIDTH, 10, SSD1306_WHITE);

  display.setCursor(0, 15);
  display.setTextWrap(true);
  display.println(message);
  display.setTextWrap(false);

  display.display();
}
