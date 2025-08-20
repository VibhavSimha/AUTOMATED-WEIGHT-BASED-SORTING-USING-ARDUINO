#define BLYNK_TEMPLATE_ID "TMPL3OaFAguGL"
#define BLYNK_TEMPLATE_NAME "Weight Based Seperator"
#define BLYNK_AUTH_TOKEN "tNXkTffEKDgm5tHqzY8bkhnEnCD0iV6-"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include "HX711.h"
#include <ESP32Servo.h>





const int LOADCELL_DOUT_PIN = 18;
const int LOADCELL_SCK_PIN = 19;

HX711 scale;

int blue = 26, green = 4, red = 2;
int buzz = 27;
Servo tservo, fservo;
int thresh = 20;


float avgVal;

bool autoMode = true;
char auth[] = BLYNK_AUTH_TOKEN;
const char ssid[] = "motorola edge 20 fusion_8533";
const char pass[] = "f09f58hi92";

void setup() {
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass);
  Blynk.virtualWrite(V4, thresh); 
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(435);
  scale.tare();

  Serial.println("Readings:");

  tservo.attach(13);
  fservo.attach(12);
  tservo.write(0);
  fservo.write(0);
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(blue, OUTPUT);
  pinMode(buzz, OUTPUT);

  digitalWrite(buzz, LOW);
}

void led(boolean r, boolean g, boolean b) {
  digitalWrite(red, r);
  digitalWrite(green, g);
  digitalWrite(blue, b);
}

void fcond() {
  Blynk.virtualWrite(V2, 1); 
  Blynk.virtualWrite(V1, 0); 
  tservo.write(0);
  led(1, 0, 0);
  digitalWrite(buzz, HIGH);
  for (int i = 0; i <= 98; i += 5) {
    fservo.write(i);
    delay(15);
  }
  for (int i = 98; i >= 0; i -= 5) {
    fservo.write(i);
    delay(15);
  }
  fservo.write(0);
  delay(1000);
  digitalWrite(buzz, LOW);
}

void tcond() {
  Blynk.virtualWrite(V1, 1); 
  Blynk.virtualWrite(V2, 0); 
  fservo.write(0);
  led(0, 1, 0);
  digitalWrite(buzz, LOW);
  for (int i = 0; i <= 90; i += 5) {
    tservo.write(i);
    delay(15);
  }
  for (int i = 90; i >= 0; i -= 5) {
    tservo.write(i);
    delay(15);
  }
  tservo.write(0);
  delay(1000);
}

void wcond() {
  led(0, 0, 1);
  digitalWrite(buzz, LOW);
  fservo.write(0);
  tservo.write(0);
  for (int i = 1; i < 3; i++) {
    digitalWrite(buzz, HIGH);
    delay(500);
    digitalWrite(buzz, LOW);
    delay(500);
  }
}

void hcond() {
  Serial.println("OBJECT TOO HEAVY!");

  for (int i = 0; i < 3; i++) {
    led(0, 0, 1);
    digitalWrite(buzz, HIGH);
    delay(100);
    digitalWrite(buzz, LOW);
    delay(100);
    digitalWrite(buzz, HIGH);
    delay(100);
    digitalWrite(buzz, LOW);
    delay(200);
    led(0, 0, 0);
    digitalWrite(buzz, HIGH);
    delay(100);
    digitalWrite(buzz, LOW);
    delay(100);
    digitalWrite(buzz, HIGH);
    delay(100);
    digitalWrite(buzz, LOW);
    delay(200);
  }
}

BLYNK_WRITE(V0) {
  autoMode = param.asInt();  // 1 = automatic, 0 = manual
  Serial.print("Mode: ");
  Serial.println(autoMode ? "Automatic" : "Manual");
}

BLYNK_WRITE(V1) {
  if (!autoMode && param.asInt()) {
    tcond();
    delay(3000);
  }
}

BLYNK_WRITE(V2) {
  if (!autoMode && param.asInt()) {
    fcond();
    delay(3000);
  }
}
BLYNK_WRITE(V4)
{
  thresh=param.asInt();
}

void loop() {
  Blynk.run();
  
  wcond();
  delay(1000);

  avgVal = scale.get_units(10);
  Blynk.virtualWrite(V3, avgVal); 
if(autoMode){
  if (avgVal < 3) {
    wcond();
    Serial.println("0");
  } else {
    Serial.println(avgVal);
    if ((avgVal > (thresh + 5) || avgVal < (thresh - 5)) && avgVal < 150) {
      fcond();
    } else if (avgVal > 150) {
      hcond();
    } else {
      tcond();
    }
    delay(3000);
  }
}
}
