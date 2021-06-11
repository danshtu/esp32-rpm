#include <Arduino.h>
#include "BluetoothSerial.h"
#include "ELMduino.h"
#include <FastLED.h>

ELM327 myELM327;
BluetoothSerial SerialBT;
#define ELM_PORT    SerialBT
#define DEBUG_PORT  Serial
#define NUM_LEDS    16
#define DATA_PIN    23

CRGB leds[NUM_LEDS] = {0};
uint32_t rpm = 0;

void setAllLeds(CRGB color) {
  for (size_t i = 0; i < NUM_LEDS; i++)
    leds[i] = color;
}

void setLedPair(int l, int r, CRGB color) {
  leds[l] = color;
  leds[r] = color;
}

void ledInit() {
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(30);
  setAllLeds(0);
}

void ledSetRPM(uint32_t rpm) {
  // Clear all the leds to off
  setAllLeds(0);

  switch (rpm)
  {
  case 6500 ... 15000:
    setAllLeds(CRGB::White);
    break;
  case 6000 ... 6499:
    setLedPair(7,8,CRGB::Red);
  case 5500 ... 5999:
    setLedPair(6,9,CRGB::Yellow);
  case 5000 ... 5499:
    setLedPair(5,10,CRGB::Yellow);
  case 4000 ... 4999:
    setLedPair(4,11,CRGB::Yellow);
  case 3000 ... 3999:
    setLedPair(3,12,CRGB::Green);
  case 2000 ... 2999:
    setLedPair(2,13,CRGB::Green);
  case 1000 ... 1999:
    setLedPair(1,14,CRGB::Green);
  case 500 ... 999:
    setLedPair(0,15,CRGB::Green);
    break;
  default:
    setLedPair(0,15,CRGB::Blue);
    break;
  }

  FastLED.show();
}

void setup() {
  // put your setup code here, to run once:
  #if LED_BUILTIN
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
  #endif

  ledInit();
  DEBUG_PORT.begin(115200);
  ELM_PORT.begin("ArduHUD", true);
  
  if (!ELM_PORT.connect("OBDII")) {
    DEBUG_PORT.println(("Couldn't connect to OBD scanner - Phase 1"));
    while(1);
  }

  if (!myELM327.begin(ELM_PORT, true, 2000)) {
    DEBUG_PORT.println("Couldn't connect to OBD scanner - Phase 2");
    while(1);
  }

  DEBUG_PORT.println("Connected to ELM327");
}

void loop() {
  // put your main code here, to run repeatedly:
  // leds[0] = CRGB::Red;
  // FastLED.show();
  // delay(500);

  float tempRPM = myELM327.rpm();

  if (myELM327.status == ELM_SUCCESS) {
    rpm = (uint32_t) tempRPM;
    DEBUG_PORT.print("RPM: ");
    DEBUG_PORT.println(rpm);
  } else {
    myELM327.printError();
  }

  ledSetRPM(rpm);
}