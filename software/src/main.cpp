#include <Arduino.h>

#include "SerialDebugger/SerialDebugger.hpp"

// Pin definitions
static const uint8_t LED_PIN = 21;
static const uint8_t BUTTON_PIN = 16;

// Delay before accepting button state change for debouncing purposes
static const unsigned long DEBOUNCE_DELAY_MILLIS = 20;

// The last time the button interrupt was triggered
unsigned long gLastButtonStateChangeTime = 0;
// The state the button was in when the interrupt occured
uint8_t gButtonState = LOW;
// The last button state we wrote to the LED
uint8_t gLastActionedButtonState = LOW;

/* Variables for testing purposes only */
// The number of button state changes triggered
uint16_t gButtonStateChangedTriggered = 0;
// The number of LED state changes after button debounce
uint16_t gLEDStateChangedTriggered = 0;

/********************
 * Interrupt routines
 ********************/
void buttonPressed() {
  gLastButtonStateChangeTime = millis();
  gButtonState = digitalReadFast(BUTTON_PIN);
  // For testing purposes:
  gButtonStateChangedTriggered++;
}

/*********************
 * Entry Point methods
 *********************/
// Setup function - executes once on startup
void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLDOWN);
  attachInterrupt(BUTTON_PIN, buttonPressed, CHANGE);
  digitalWrite(LED_PIN, LOW);
  // Setup serial interface to demonstrate that debounce is in fact working
  // (difficult to tell from an LED)
  Serial.begin(115200);
}

// Loop function - loops :)
void loop() {
  // Only do anything if the state has actually changed from what we last actioned
  if (gButtonState != gLastActionedButtonState) {
    // Check to see if it's been in this state for long enough (i.e. bouncing has
    // stopped)
    if (millis() - gLastButtonStateChangeTime > DEBOUNCE_DELAY_MILLIS) {
      gLEDStateChangedTriggered++;
      digitalWrite(LED_PIN, gButtonState);
      gLastActionedButtonState = gButtonState;
    }
  }

  SerialDebugger.updateValue("Number of button state changes",
                             gButtonStateChangedTriggered);
  SerialDebugger.updateValue("Number of LED state changes",
                             gLEDStateChangedTriggered);
}