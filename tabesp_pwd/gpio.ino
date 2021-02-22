// 0 D5  GPIO14
// 1 D6  GPIO12
// 2 D7  GPIO13

extern const uint8_t gamma8[];

uint16_t gpioState[3] = {0, 0, 0};
uint16_t gpioTarget[3] = {0, 0, 0};
bool relayGpio[3] = {false, false, false};

uint16_t gpioStep = 15;

void setGpioPWM(int pin, uint16_t value) {
  uint16_t tmpVal = value * 4;
  if (tmpVal > 1023) {
    tmpVal = 1023;
  }
  if (pin == 0) {
    analogWrite(14, tmpVal);
  }
  if (pin == 1) {
    analogWrite(12, tmpVal);
  }
  if (pin == 2) {
    analogWrite(13, tmpVal);
  }
}

void handleGpio() {
  for (int i = 0; i <= 2; i++) {
    if (gpioState[i] == gpioTarget[i]) {
      continue;
    }

    if (relayGpio[i]) {
      setGpioPWM(i, gpioTarget[i]);
      gpioState[i] = gpioTarget[i];

    } else {

      uint16_t next = 0;
      if (gpioState[i] < gpioTarget[i]) {
        if (gpioTarget[i] - gpioState[i] <= gpioStep ) {
          next = gpioTarget[i];
        } else {
          next = gpioState[i] + gpioStep;
        }

      } else {
        if (gpioState[i] - gpioTarget[i] <= gpioStep ) {
          next = gpioTarget[i];
        } else {
          next = gpioState[i] - gpioStep;
        }
      }

      setGpioPWM(i, next);
      gpioState[i] = next;

    }
  }
}

void setGpioTarget(int pin, uint16_t value) {
  gpioTarget[pin] = value;
  relayGpio[pin] = false;
}


void setGColorTarget(CRGB rgb) {
  DEBUG("Setting color: ");
  DEBUG(" "); DEBUG(rgb.red); DEBUG(" - "); DEBUG(gamma8[rgb.red]);
  DEBUG(" "); DEBUG(rgb.green); DEBUG(" - "); DEBUG(gamma8[rgb.green]);
  DEBUG(" "); DEBUG(rgb.blue); DEBUG(" - "); DEBUG_LN(gamma8[rgb.blue]);

  setGpioTarget(0, gamma8[rgb.red]);
  setGpioTarget(1, gamma8[rgb.green]);
  setGpioTarget(2, gamma8[rgb.blue]);
  // TODO upgrade mqtt
}

void setGRelayTarget(int pin, bool value) {
  if (value) {
    gpioTarget[pin] = 256;
  } else {
    gpioTarget[pin] = 0;
  }
  relayGpio[pin] = true;
}
