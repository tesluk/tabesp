void setupPWD() {
  pwm.begin();
  pwm.setPWMFreq(1600);
}

extern const uint8_t gamma8[];

uint16_t pwdState[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint16_t pwdTarget[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
bool relayPin[16] = {false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false};

// Main pwm function.
void setPinPWM(int pin, uint16_t value) {
  DEBUG("Setting PWM: "); DEBUG(pin); DEBUG(" to: "); DEBUG_LN(value);
  uint16_t tmpVal = value * 16;
  if (tmpVal > 4095) {
    tmpVal = 4095;
  }
  pwm.setPin(pin, tmpVal);
}

// pwm on single pin
void setPWMTarget(int pin, uint16_t value) {
  pwdTarget[pin] = value;
  relayPin[pin] = false;
}

// pwm on 3 pins
void setColorTarget(int pin, RGB rgb) {
  DEBUG("Setting color: ");
  DEBUG(" "); DEBUG(rgb.red); DEBUG(" - "); DEBUG(gamma8[rgb.red]);
  DEBUG(" "); DEBUG(rgb.green); DEBUG(" - "); DEBUG(gamma8[rgb.green]);
  DEBUG(" "); DEBUG(rgb.blue); DEBUG(" - "); DEBUG_LN(gamma8[rgb.blue]);

  setPWMTarget(pin, gamma8[rgb.red]);
  setPWMTarget((pin + 1) % 16,  gamma8[rgb.green]);
  setPWMTarget((pin + 2) % 16,  gamma8[rgb.blue]);
  // TODO upgrade mqtt
}

// on/off on single pin
void setRelayTarget(int pin, bool state) {
  relayPin[pin] = true;
  if (state) {
    pwdTarget[pin] = 256;
  } else {
    pwdTarget[pin] = 0;
  }
  // TODO upgrade mqtt
}



uint16_t pwdStep = 15;

void handlePWD() {
  for (int i = 0; i < 16; i++) {
    if (pwdState[i] == pwdTarget[i]) {
      continue;
    }

    if (relayPin[i]) {
      setPinPWM(i, pwdTarget[i]);
      pwdState[i] = pwdTarget[i];

    } else {

      uint16_t next = 0;
      if (pwdState[i] < pwdTarget[i]) {
        if (pwdTarget[i] - pwdState[i] <= pwdStep ) {
          next = pwdTarget[i];
        } else {
          next = pwdState[i] + pwdStep;
        }

      } else {
        if (pwdState[i] - pwdTarget[i] <= pwdStep ) {
          next = pwdTarget[i];
        } else {
          next = pwdState[i] - pwdStep;
        }
      }

      setPinPWM(i, next);
      pwdState[i] = next;

    }
  }
}
