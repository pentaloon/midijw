#include <Arduino.h>
#include <RotaryEncoder.h>
#include "MIDIUSB.h"

#define PIN_IN1 2
#define PIN_IN2 3
#define ROTARYMIN 0
#define ROTARYCENTRE 64
#define ROTARYMAX 127
#define MIDI_CHANNEL 8
#define MIDI_CC_GPCTRL1 0x10

RotaryEncoder *encoder = nullptr;
int lastPos = -1;

void checkPosition()
{
  encoder->tick(); // just call tick() to check the state.
}

void controlChange(byte channel, byte control, byte value) {
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
  MidiUSB.sendMIDI(event);
}

void setup()
{
  encoder->setPosition(ROTARYCENTRE);
  encoder = new RotaryEncoder(PIN_IN1, PIN_IN2, RotaryEncoder::LatchMode::TWO03);

  attachInterrupt(digitalPinToInterrupt(PIN_IN1), checkPosition, CHANGE);
  attachInterrupt(digitalPinToInterrupt(PIN_IN2), checkPosition, CHANGE);
}

void loop()
{
  encoder->tick();

  int pVal = encoder->getPosition();
  int newPos = constrain(pVal,ROTARYMIN,ROTARYMAX);

  if (newPos != ROTARYCENTRE) {
    controlChange(MIDI_CHANNEL, MIDI_CC_GPCTRL1, newPos);
    MidiUSB.flush();
    encoder->setPosition(ROTARYCENTRE);
    lastPos, newPos = ROTARYCENTRE;
  }
  delay(10);
}
