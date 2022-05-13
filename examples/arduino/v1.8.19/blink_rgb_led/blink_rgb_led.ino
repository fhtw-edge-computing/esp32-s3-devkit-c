#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "Arduino.h"

#include "esp32-hal.h"

#define CONFIG_IDF_TARGET_ESP32S3_LEDSTRIP 1

// The effect seen in ESP32C3, ESP32S2 and ESP32S3 is like a Blink of RGB LED
#if CONFIG_IDF_TARGET_ESP32S3_LEDSTRIP
#define BUILTIN_RGBLED_PIN   47
#elif CONFIG_IDF_TARGET_ESP32S2
#define BUILTIN_RGBLED_PIN   18
#elif CONFIG_IDF_TARGET_ESP32S3
#define BUILTIN_RGBLED_PIN   48
#elif CONFIG_IDF_TARGET_ESP32C3
#define BUILTIN_RGBLED_PIN   8
#else
#define BUILTIN_RGBLED_PIN   21   // ESP32 has no builtin RGB LED
#endif

#define NR_OF_LEDS   8
#define NR_OF_ALL_BITS 24*NR_OF_LEDS

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
    
    if ((rmt_send = rmtInit(BUILTIN_RGBLED_PIN, RMT_TX_MODE, RMT_MEM_64)) == NULL)
    {
        Serial.println("init sender failed\n");
    }

    float realTick = rmtSetTick(rmt_send, 100);
    Serial.printf("real tick set to: %fns\n", realTick);

}

int color[] =  { 0x8b, 0xb3, 0x1d };  // RGB value
//int color[] =  { 0x00, 0x64, 0x9c };  // RGB value
int led_index = 0;

void loop() 
{
    // Init data with only one led ON
    int led, col, bit;
    int i=0;
    for (led=0; led<NR_OF_LEDS; led++) {
        for (col=0; col<3; col++ ) {
            for (bit=0; bit<8; bit++){
                if ( (color[col] & (1<<(7-bit))) && (led == led_index) ) {
                    led_data[i].level0 = 1;
                    led_data[i].duration0 = 4;
                    led_data[i].level1 = 0;
                    led_data[i].duration1 = 8;
                } else {
                    led_data[i].level0 = 1;
                    led_data[i].duration0 = 8;
                    led_data[i].level1 = 0;
                    led_data[i].duration1 = 4;
                }
                i++;
            }
        }
    }
    // make the led travel in the pannel
    if ((++led_index)>=NR_OF_LEDS) {
        led_index = 0;
    }

    // Send the data
    rmtWrite(rmt_send, led_data, NR_OF_ALL_BITS);

    delay(100);
}
