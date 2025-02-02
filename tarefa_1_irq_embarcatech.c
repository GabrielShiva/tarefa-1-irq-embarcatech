#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/bootrom.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"

#include "ws2812b.pio.h"
#include "includes/leds_matrix.h"
#include "includes/convert_to_rgba.h"
#include "includes/sprites.h"

#define BTN_A_PIN 5 // define o pino do botão incrementador
#define BTN_B_PIN 6 // define o pino do botão decrementador
#define LED_RED_PIN 13 // define o pino do LED vermelho
#define LEDS_MATRIX_PIN 7 // define o pino de comunicação com a matriz de LEDS endereçáveis

#define MATRIX_ROWS 5
#define MATRIX_COLS 5
#define MATRIX_LEDS 25 // define o numero de LEDS da matriz

static volatile uint counter = 0; // define o contador como volátil para evitar otimizações sobre a variável (o compilador não armazena esse valor em um cache)
static volatile absolute_time_t last_time_btn_press = {0}; // define variável que armazena o tempo referente à última vez que um dos botões foi pressionado

// inicialização do LED e dos botões (com resistor de pull-up interno)

void setup()
{
    gpio_init(LED_RED_PIN);
    gpio_set_dir(LED_RED_PIN, GPIO_OUT);

    gpio_init(BTN_A_PIN);
    gpio_set_dir(BTN_A_PIN, GPIO_IN);
    gpio_pull_up(BTN_A_PIN);

    gpio_init(BTN_B_PIN);
    gpio_set_dir(BTN_B_PIN, GPIO_IN);
    gpio_pull_up(BTN_B_PIN);
}

void insert_sprite(int sprite_index) 
{
    npLED_t leds[LED_COUNT];
    int rgb_matrix[MATRIX_ROWS][MATRIX_COLS][3];

    convertARGBtoMatriz(matrix_sprites[sprite_index], rgb_matrix);
    spriteWrite(rgb_matrix, leds);
    matrizWrite(leds);
}

// definição da função que faz o LED piscar 5 vezes por segundo ()
void make_led_blink(uint led_pin, uint32_t sleep_time)
{
    // o LED deve piscar 5vezes/seg => período de 0.2seg ou 200ms => 100ms ligado/ 100ms desligado

    gpio_put(led_pin, 1);
    sleep_ms(sleep_time);
    gpio_put(led_pin, 0);
    sleep_ms(sleep_time);
}

// definição do método que é responsável por realizar o tratamento da interrupção gerada pelos botões
void gpio_irq_handler(uint gpio, uint32_t events)
{
    absolute_time_t current_time = get_absolute_time(); // obtem o tempo atual desde que o raspberry foi ligado (em microssegundos)

    if (current_time - last_time_btn_press > 250 * 1000) { // verifica se a diff entre o tempo atual e a ultima vez que o botão foi pressionado é maior que o tempo de debounce em microssegundos (por isso 260 * 1000)
        last_time_btn_press = current_time; // atualiza o tempo da última pressão no botão
        
        if (gpio == BTN_A_PIN) { // verifica se o botão A foi pressionado
            if (counter < 9) { // verifica se o valor no contador é menor que 9. Se sim, incrementa o valor. Utilizado para impedir que o valor no contador ultrapasse a faixa permitida (0 <= contador <= 9)
                counter++; // incrementa o valor do contador em uma unidade
            }

            insert_sprite(counter);
        } else if (gpio == BTN_B_PIN) {  // verifica se o botão B foi pressionado
            if (counter > 0) { // verifica se o valor no contador é maior que 0. Se sim, incrementa o valor. Utilizado para impedir que o valor no contador ultrapasse a faixa permitida (0 <= contador <= 9)
                counter--; // decrementa o valor do contador em uma unidade
            }
            
            insert_sprite(counter);
        }

        printf("VALOR ATUAL: %d\n", counter); // imprime o valor atual do contador
    }
}

int main()
{
    npLED_t leds[LED_COUNT];
    int rgb_matrix[MATRIX_ROWS][MATRIX_COLS][LED_COUNT];
    
    stdio_init_all(); 
    setup(); // inicializa as GPIOS
    
    // inicializa a matriz de leds
    matrizInit(LED_PIN, leds);
    
    // limpa a matriz de leds
    npClear(leds);
    matrizWrite(leds);

    insert_sprite(0);

    // confiuração das interrupções para os botões A e B para borda de descida
    gpio_set_irq_enabled_with_callback(BTN_A_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled(BTN_B_PIN, GPIO_IRQ_EDGE_FALL, true);

    while (true) {
        // chama a rotina para o LED vermelho piscar 5 vezes por segundo
        make_led_blink(LED_RED_PIN, 100);
    }

    return 0;
}
