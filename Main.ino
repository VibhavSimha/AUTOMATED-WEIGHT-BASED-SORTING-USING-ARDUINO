#include <Arduino.h>
#include "HX711.h"
#include <Servo.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x3F,16,2);
// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 4;
const int LOADCELL_SCK_PIN = 5;

HX711 scale;

int blue=6,green=7,red=8;
int buzz=9;
Servo tservo,fservo;
int thresh=20;
float avgVal;
void setup() {
  Serial.begin(57600);
  
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("READY");
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);

  
            
  scale.set_scale(404);
  
  scale.tare();              

  
  Serial.println("Readings:");


  tservo.attach(10);
  fservo.attach(2);
  tservo.write(0);
  fservo.write(8);
  pinMode(red,OUTPUT);
  pinMode(green,OUTPUT);
  pinMode(blue,OUTPUT);
  pinMode(buzz,OUTPUT);
}

void led(boolean r,boolean g,boolean b)
{
  digitalWrite(red,r);
  digitalWrite(green,g);
  digitalWrite(blue,b);
}
void fcond()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Object REJECTED");
  lcd.setCursor(0,1);
  lcd.print("Wt: ");
  lcd.print(avgVal);
  tservo.write(0);
  led(1,0,0);
  digitalWrite(buzz,HIGH);
  for(int i=8;i<=98;i+=5)
  {
    fservo.write(i);
    delay(15);
  }
  for(int i=98;i>=8;i-=5)
  {
    fservo.write(i);
    delay(15);
  }
  fservo.write(8);
  delay(1000);
  digitalWrite(buzz,LOW);

}
void tcond()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Object PASSED");
  lcd.setCursor(0,1);
  lcd.print("Wt: ");
  lcd.print(avgVal);
  fservo.write(8);
  led(0,1,0);
  digitalWrite(buzz,LOW);
  for(int i=0;i<=90;i+=5)
  {
    tservo.write(i);
    delay(15);
  }
  for(int i=90;i>=0;i-=5)
  {
    tservo.write(i);
    delay(15);
  }
  tservo.write(0);
  delay(1000);
}
void wcond()
{
  lcd.clear();
  lcd.print("READY");
  led(0,0,1);
  digitalWrite(buzz,LOW);
  fservo.write(8);
  tservo.write(0);
  for(int i=1;i<3;i++){
    digitalWrite(buzz,HIGH);
    delay(500);
    digitalWrite(buzz,LOW);
    delay(500);
  }
}
void hcond()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Object too heavy!!");
  Serial.println("OBJECT TOO HEAVY!");
  lcd.setCursor(0,1);
  lcd.print("Wt: ");
  lcd.print(avgVal);
  for(int i=0;i<3;i++)
  {
    led(0,0,1);
    digitalWrite(buzz,HIGH);
    delay(100);
    digitalWrite(buzz,LOW);
    delay(100);
    digitalWrite(buzz,HIGH);
    delay(100);
    digitalWrite(buzz,LOW);
    delay(200);
    led(0,0,0);
    digitalWrite(buzz,HIGH);
    delay(100);
    digitalWrite(buzz,LOW);
    delay(100);
    digitalWrite(buzz,HIGH);
    delay(100);
    digitalWrite(buzz,LOW);
    delay(200);
  }
}
void loop() {
  wcond();
  delay(1000);
  
  avgVal=scale.get_units(10);
  
  if(avgVal<3){wcond();
  Serial.println("0");}
  else{
    Serial.println(avgVal);
  if((avgVal>(thresh+5) || avgVal<(thresh-5))&& avgVal<150)fcond();
  else if (avgVal>150)hcond();
  else tcond();
  delay(3000);
  }
  
}