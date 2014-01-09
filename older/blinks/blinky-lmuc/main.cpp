// Blinky demo sketch, controlled by Bencoded commands.
// 2012-10-03 <jc@wippler.nl> http://opensource.org/licenses/mit-license.php

// Sample data:
//  l4:ratei250eel7:triggeri4eel5:counti10ee
//  l4:ratei100eel7:triggeri10eel5:counti20ee

#include "libmanyuc.h"
#include "EmBencode.h"

#define VERSION 102

long now;
Serial serial (0, 9600);

char embuf [200];
EmBdecode decoder (embuf, sizeof embuf);
EmBencode encoder;

int rate;       // time between toggling the LED (ms)
int count;      // number of blinks still to go
int trigger;    // after how many blinks to trigger
long total;     // total number of blinks
long lastFlip;  // time when we last flipped the LED (ms)

static void initLed () {
	LPC_PINCON->PINSEL[0] &= ~ (3 << 12); // set general GPIO mode 0
	LPC_GPIO0->FIODIR |= 1 << 22; // P0.22 connected to LED2
}

static uint8_t toggleLed () {
	LPC_GPIO0->FIOPIN ^= 1 << 22; // Toggle P0.22
  return (LPC_GPIO0->FIOPIN >> 22) & 1;
}

void EmBencode::PushChar (char ch) {
	serial.write(&ch, 1); // the 1-arg version is not blocking!
}

static void sendGreeting () {
  encoder.startList();
  encoder.push("blinky");
  encoder.push(VERSION);
  encoder.endList();
}

static void sendErrorMsg (const char* msg) {
  encoder.startList();
  encoder.push(1); // error code
  encoder.push(msg);
  encoder.endList();
}

static void sendDoneMsg () {
  encoder.startList();
  encoder.push("done");
  encoder.endList();
}

void sendTriggerTime () {
  encoder.startList();
  encoder.push("time");
  encoder.push(now);
  encoder.endList();
}

static void setNumber (int& ivar) {
  if (decoder.nextToken() == EmBdecode::T_NUMBER)
    ivar = decoder.asNumber();
  else
    sendErrorMsg("number expected");
}

int main () {
  sendGreeting();
  initLed();
  lastFlip = now;
#if 1
  rate = 250;
  trigger = 4;
  count = 10;
#endif

  while (1) {
    // process incoming serial data
    if (serial.readable() &&
        decoder.process(serial.getByte()) > 0 &&
        decoder.nextToken() == EmBdecode::T_LIST &&
        decoder.nextToken() == EmBdecode::T_STRING) {
      // a complete message has been received
      const char* cmd = decoder.asString();
      if (strcmp(cmd, "rate") == 0)
        setNumber(rate);
      else if (strcmp(cmd, "count") == 0)
        setNumber(count);
      else if (strcmp(cmd, "trigger") == 0)
        setNumber(trigger);
      else
        sendErrorMsg("command expected");
      decoder.reset();
      total = 0; // synchronise on each new setting
    }

    // perform the requested blinky work
    Delay_us(1000);
    ++now;
    if (count > 0 && rate > 0 && now >= lastFlip + rate) {
      // the time has come to flip the LED state
      lastFlip = now;
      bool on = toggleLed();
      // check whether the LED just went off
      if (!on) {
        // has the time come to send out a trigger report?
        if (trigger > 0 && ++total % trigger == 0)
          sendTriggerTime();
        // have we completed our work?
        if (--count == 0)
          sendDoneMsg();
      }
    }
  }

	return 0;
}
