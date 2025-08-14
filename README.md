# Smart Glasses

This project is the firmware for a pair of custom-built smart glasses using an Arduino Nano. The glasses display real-time information from a smartphone, including time, date, notifications, media, and weather, via a Bluetooth connection to an OLED display.

---

## Hardware Required

* Arduino Nano
* HC-05 or HC-06 Bluetooth Module
* SSD1306 0.96" I2C OLED Display (128x64)
* DS3231 I2C Real-Time Clock (RTC) Module
* Connecting wires

---

## Wiring

Connect the components as follows. The I2C bus (SDA/SCL) can be shared between the OLED and RTC modules.

| Component           | Arduino Pin |
| ------------------- | ----------- |
| **OLED Display** |             |
| VCC                 | 5V          |
| GND                 | GND         |
| SDA                 | A4          |
| SCL                 | A5          |
| **RTC Module** |             |
| VCC                 | 5V          |
| GND                 | GND         |
| SDA                 | A4          |
| SCL                 | A5          |
| **Bluetooth Module**|             |
| VCC                 | 5V          |
| GND                 | GND         |
| TX                  | D2 (Arduino RX) |
| RX                  | D3 (Arduino TX) |

**Note:** The Bluetooth module's RX pin may require a voltage divider if it is not 5V tolerant.

---

## Setup & Installation

1.  **Install Arduino Libraries:**
    Open the Arduino IDE and navigate to **Sketch > Include Library > Manage Libraries...**. Search for and install the following:
    * `Adafruit GFX Library`
    * `Adafruit SSD1306`
    * `RTClib`

2.  **Set the Time:**
    The first time you upload the code, you need to set the Real-Time Clock. In the `setup()` function of the `.ino` sketch, uncomment this line:
    ```cpp
    // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    ```
    Upload the sketch. This sets the RTC to your computer's time at the moment of compilation. After this initial upload, **comment the line out again** and re-upload the sketch. This prevents the time from being reset every time the glasses turn on.

3.  **Upload the Sketch:**
    Connect your Arduino Nano and upload the final sketch.

---

## Companion App Protocol

A companion smartphone application is required to send data to the glasses. The app must connect to the HC-05/HC-06 Bluetooth module and send newline-terminated strings with the following prefixes to display information:

* **Notification:** `N:Your notification text`
* **Media:** `M:Song Title - Artist`
* **Weather:** `W:72F Sunny`
* **Set Time (Optional):** `T:YYYY,MM,DD,hh,mm,ss` (e.g., `T:2025,08,14,01,09,30`)

Notifications, media, and weather information will be displayed for 5 seconds before the screen reverts to showing the time and date.
