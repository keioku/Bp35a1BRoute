#include "Bp35c0-broute.h"

#define ID    "YOUR_B_ROUTE_ID_HERE"
#define PASS  "YOUR_B_ROUTE_PASSWORD_HERE"

#define LED 2

HardwareSerial uart(2);
Bp35c0_broute b(uart);

void blink(int sec)
{
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);
  delay(sec);
  digitalWrite(LED, LOW);
  delay(sec);
}

void error()
{
  pinMode(LED, OUTPUT);
  while (true)
    blink(500);
}

void initLed()
{
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
}

void setup()
{
  Serial.begin(115200);
  uart.begin(115200);
  //b.setDebugSerial(Serial);

  initLed();

  if (!b.available())
    error();
  if (!b.open(ID, PASS))
    error();
}

void loop()
{
  blink(100);

  char u[32] = {'\0'}, d[32] = {'\0'};
  float uu, dd;

  b.waitForRecieve(d, &dd, u, &uu);
  Serial.println(u"正方向:" + String(d) + " : " + String(dd));
  Serial.println(u"逆方向:" + String(u) + " : " + String(uu));
}
