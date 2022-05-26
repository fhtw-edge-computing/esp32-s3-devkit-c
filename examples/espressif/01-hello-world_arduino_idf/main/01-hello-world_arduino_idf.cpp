#include <Arduino.h>

extern "C" void app_main(void) {
  initArduino();

  // Arduino-like setup()
  Serial.begin(115200);
  while (!Serial) {
    ;  // wait for serial port to connect
  }
  Serial.println();
  Serial.println("Hello World!");

  // Arduino-like loop()
  while (true) {
    delay(1000);
  }

  // WARNING: if program reaches end of function app_main() the MCU will restart.
}
