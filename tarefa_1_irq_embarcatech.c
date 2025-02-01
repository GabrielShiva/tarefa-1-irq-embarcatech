#include <stdio.h>
#include "pico/stdlib.h"

#define BTN_A_PIN 5 
#define BTN_B_PIN 6
#define LED_RED_PIN 13
#define LEDS_MATRIX_PIN 7

void init_led_rgb_pin()
{
    gpio_init(LED_RED_PIN);
    gpio_set_dir(LED_RED_PIN, GPIO_OUT);
}

void init_btns_pins()
{
    gpio_init(BTN_A_PIN);
    gpio_set_dir(BTN_A_PIN, GPIO_IN);
    gpio_init(BTN_B_PIN);
    gpio_set_dir(BTN_B_PIN, GPIO_IN);
}

/**
 * O LED deve piscar 5vezes/seg => período de 0.2seg ou 200ms => 100ms ligado/ 100ms desligado
 */
void make_led_blink(uint led_pin, uint32_t sleep_time)
{
    gpio_put(led_pin, 1);
    sleep_ms(sleep_time);
    gpio_put(led_pin, 0);
    sleep_ms(sleep_time);
}

int main()
{
    stdio_init_all();

    init_led_rgb_pin();
    init_btns_pins();
    
    while (true) {
        make_led_blink(LED_RED_PIN, 100);
    }
}
