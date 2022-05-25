#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "Arduino.h"

#include "esp32-hal.h"

#define RGBLED_PIN   47

#define NR_OF_COLORS 3
#define NR_OF_LEDS   8
#define NR_OF_RGB_BITS 24
#define NR_OF_ALL_BITS NR_OF_RGB_BITS*NR_OF_LEDS

//
// Note: This example uses a LED strip, 8 LEDs chained one
//      after another, each RGB LED has its 24 bit value 
//      for color configuration (8b for each color)
//
//      Bits encoded as pulses as follows:
//
//      "1":
//         +-------+              +--
//         |       |              |
//         |       |              |
//         |       |              |
//      ---|       |--------------|
//         +       +              +
//         | 0.4us |   0.85 0us   |
//
//      "0":
//         +-------------+       +--
//         |             |       |
//         |             |       |
//         |             |       |
//         |             |       |
//      ---+             +-------+
//         |    0.8us    | 0.4us |

rmt_data_t led_data[NR_OF_ALL_BITS];
rmt_obj_t* rmt_send = NULL;

void setup() 
{
  Serial.begin(115200);
    
  if ((rmt_send = rmtInit(RGBLED_PIN, RMT_TX_MODE, RMT_MEM_64)) == NULL)
  {
    Serial.println("init sender failed\n");
  }

  float realTick = rmtSetTick(rmt_send, 100);
  Serial.printf("real tick set to: %fns\n", realTick);

}

int colors[][NR_OF_COLORS] = {
  { 0x8b, 0xb3, 0x1d }, // fhtw-green
  { 0x72, 0x77, 0x7a }, // fhtw-gray
  { 0x00, 0x64, 0x9c }, // fhtw-blue
};

void load_bit(int bit, int status) {
  led_data[bit].level0 = 1;
  led_data[bit].duration0 = status == 0 ? 8 : 4;
  led_data[bit].level1 = 0;
  led_data[bit].duration1 = status == 0 ? 4 : 8;
  Serial.printf(status == 0 ? "0" : "1");
}

void load_led(int led, int i) {  
  Serial.printf("LED %d: ", led);
  
  for (int col = 0; col < 3; col++ ) {
    for (int bit = 0; bit < 8; bit++) {
      load_bit(led * NR_OF_RGB_BITS + col * 8 + bit, (colors[i][col] & (1<<(7-bit))));
    }
    Serial.printf(" ");
  }
  Serial.printf("\r\n");
}

void clear_led(int led) {
  for (int col = 0; col < 3; col++ ) {
    for (int bit = 0; bit < 8; bit++) {
      led_data[led * NR_OF_RGB_BITS + col * 8 + bit].level0 = 1;
      led_data[led * NR_OF_RGB_BITS + col * 8 + bit].duration0 = 4;
      led_data[led * NR_OF_RGB_BITS + col * 8 + bit].level1 = 0;
      led_data[led * NR_OF_RGB_BITS + col * 8 + bit].duration1 = 8;
      Serial.printf("0");
    }
    Serial.printf(" ");
  }
  Serial.printf("\r\n");
}

int cnt = 0;

void loop() 
{ 
  for (int led = 0; led < NR_OF_LEDS; led++) {
    load_led(led, (led + cnt) % NR_OF_COLORS);
    // clear_led(led);
  }
  Serial.printf("\r\n");
   
  // Send the data
  rmtWrite(rmt_send, led_data, NR_OF_ALL_BITS);

  // Move colors
  cnt++;
  cnt%=8;

  delay(300);
}
