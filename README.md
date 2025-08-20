# Weight Based Sorting IoT Embedded System
The **Weight-Based Quality Control Using IoT and Real-Time Database Monitoring** system is an automated system designed to separate products based on weight criteria with minimal human intervention.

This system uses:

* **IoT sensors (Load Cells + HX711 Amplifier)** to measure weight in real-time.
* **ESP32** to process sensor data and control actuators.
* **Servo motors** to separate products based on their weight.
* **Blynk IoT Platform** to provide remote monitoring, control, and real-time visualization.

By integrating IoT with real-time databases, the system ensures precision, reduces human error, minimizes waste, and increases productivity across industries like **manufacturing, food & beverage, pharmaceuticals, logistics, and agriculture**.

---

## System Architecture

* **ESP32** (central controller – Wi-Fi enabled).
* **HX711 Amplifier + Load Cell (5kg)** for weight sensing.
* **Servo Motors (9g x 2)** to sort items.
* **Buzzer + RGB LEDs** for feedback alerts.
* **Blynk Cloud Dashboard** for real-time monitoring and manual/automatic control.
## Prototype System
<img width="1512" height="1134" alt="image" src="https://github.com/user-attachments/assets/46249325-d383-4196-ab17-182c5d3b9dd8" />

## Blynk Dashboard
<img width="1280" height="581" alt="image" src="https://github.com/user-attachments/assets/0392e1d8-1f34-4973-9952-58d3f261531d" />

**Process Flow:**

1. Product is placed on load cell → weight measured.
2. ESP32 reads data → compares against threshold.
3. Based on criteria:

   * Acceptable → Green LED → Pass Servo.
   * Reject → Red LED + Buzzer → Reject Servo.
   * Too Heavy → Blue LED + Warning.
4. Data pushed to **Blynk Dashboard** for monitoring.

---

## Hardware Requirements

* ESP32 Dev Module
* Load Cell (5kg)
* HX711 Amplifier
* Servo Motors (2x 9g)
* RGB LED + Buzzer
* Breadboard + Jumper wires + Power supply

---

## ESP32 Hardware Wiring (from code)

| Component | ESP32 Pin |
| --- | --- |
| HX711 DOUT | 18 |
| HX711 SCK | 19 |
| Top Servo (`tservo`) | 13 |
| Front Servo (`fservo`) | 12 |
| RGB LED Red | 2 |
| RGB LED Green | 4 |
| RGB LED Blue | 26 |
| Buzzer | 27 |

Notes:
- The load cell scale factor is set in code with `scale.set_scale(435);`. Calibrate for your hardware (see Calibration below).
- Initial threshold is `20` (adjustable via Blynk slider).

---

## Software Requirements

* **Arduino IDE** (with ESP32 board support)
* **Blynk IoT App** (Android/iOS)
* Required Libraries:

  ```cpp
  #include <WiFi.h>
  #include <BlynkSimpleEsp32.h>
  #include "HX711.h"
  #include <ESP32Servo.h>
  ```

---

## Setup Instructions

### 1. ESP32 Setup

1. Install **Arduino IDE**.
2. Add ESP32 board support:

   * Go to **File → Preferences → Additional Board URLs**
   * Add: `https://dl.espressif.com/dl/package_esp32_index.json`
3. Install libraries:

   * **Blynk** (`BlynkSimpleEsp32.h`)
   * **HX711**
   * **ESP32Servo**
4. Connect ESP32 via USB and select correct **Port** + **Board = ESP32 Dev Module**.

### 2. Blynk Account Setup

1. Download **Blynk IoT App** (Android/iOS).
2. Create a new project → Select **ESP32** → Generate **Auth Token**.
3. Replace in code:

   ```cpp
   #define BLYNK_TEMPLATE_ID   "Your_Template_ID"
   #define BLYNK_TEMPLATE_NAME "Your_Project_Name"
   #define BLYNK_AUTH_TOKEN    "Your_Auth_Token"
   ```
4. Add Widgets:

   * **Gauge/Value Display** for weight.
   * **Buttons** for manual control (V1, V2).
   * **Switch** for Auto/Manual (V0).
   * **Slider** for threshold adjustment (V4).

### Blynk Virtual Pin Mapping (from code)

- **V0**: Auto/Manual mode switch (`1 = Automatic`, `0 = Manual`)
- **V1**: Pass action (manual mode → triggers `tcond()`)
- **V2**: Reject action (manual mode → triggers `fcond()`)
- **V3**: Current weight value (`avgVal`)
- **V4**: Threshold slider (integer; default `20`)

### 3. Upload & Run

* Upload code from `Program.ino` to ESP32.
* Open **Blynk App** → connect device.
* Place objects on load cell → observe weight + sorting mechanism.

Wi‑Fi/Blynk:
- Update your Wi‑Fi SSID and password in code (`ssid`, `pass`).
- Insert your Blynk credentials (`BLYNK_TEMPLATE_ID`, `BLYNK_TEMPLATE_NAME`, `BLYNK_AUTH_TOKEN`).

---

## Operating Modes and Logic (from code)

- **Default mode**: Automatic (`autoMode = true`). Toggle via Blynk `V0`.
- **Idle/Ready**: When measured weight `< 3`, the system stays in ready state (`wcond()`), sets Blue LED and beeps briefly.
- **Measurement**: `avgVal = scale.get_units(10)` is sent to Blynk `V3`.
- **Decision thresholds**:
  - Target threshold: `thresh` (default `20`, adjustable via `V4`).
  - Tolerance: ±`5` around `thresh`.
  - Too heavy cutoff: `> 150`.

Behavior in Automatic mode:
- If `avgVal > 150` → Too Heavy: Blue LED + warning beeps (`hcond()`).
- Else if `|avgVal - thresh| <= 5` → Pass: Green LED, top servo actuates (`tcond()`).
- Else (`|avgVal - thresh| > 5` and `< 150`) → Reject: Red LED + buzzer, front servo actuates (`fcond()`).
- A post-action delay of ~3 seconds is applied.

Behavior in Manual mode:
- Use Blynk buttons: `V1` to trigger Pass (`tcond()`), `V2` to trigger Reject (`fcond()`).

Note: Output units depend on calibration; after proper calibration they are typically grams.

---

## Calibration

Use `Calibration.ino` (ESP32 version at project root) to determine the correct scale factor:
1. Upload `Calibration.ino`.
2. Follow serial prompts to tare and place a known weight.
3. Compute calibration factor = reading / known weight.
4. Update `scale.set_scale(<your_factor>);` in `Progam.ino`.

For Arduino Uno, see `Arduino Uno Version/Calibration.ino` and `Arduino Uno Version/Main.ino`.

---

## 📊 Applications

* **Manufacturing** – Product quality assurance.
* **Food & Beverage** – Package portion control.
* **Pharmaceuticals** – Accurate dosage checking.
* **Agriculture** – Sorting produce.
* **Logistics** – Verifying shipping weights.
* **Recycling** – Sorting materials by weight.
