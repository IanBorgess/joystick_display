#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "hardware/i2c.h"
#include "ssd1306.h"
#include "font.h"

#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define OLED_ADDRESS 0x3C
#define JOYSTICK_X_PIN 27
#define JOYSTICK_Y_PIN 26
#define JOYSTICK_PB 22
#define BUTTON_A 5
#define LED_PIN_GREEN 11
#define LED_PIN_BLUE 12
#define LED_PIN_RED 13

static volatile uint32_t last_time = 0;
static volatile uint a = 1;
uint pwm_red = 0;
uint pwm_blue = 0;
ssd1306_t ssd;
bool cor = true;
bool leds_on = true; // Variável para controlar se os LEDs estão ligados ou desligados
bool leds_toggle = true; // Variável para controlar se os LEDs devem ser alternados
uint borda_atual = 90; // Variável para controlar a borda


// Função de setup
void setup();

// Funções de manipulação de interrupção
void gpio_irq_handler(uint gpio, uint32_t events);

// Funções de manipulação de LED
void toggle_green_led();
void toggle_pwm_leds();

// Funções de manipulação de display
void update_display(uint16_t adc_value_x, uint16_t adc_value_y);

// Funções auxiliares
uint pwm_init_gpio(uint gpio, uint wrap);

int main() {
    stdio_init_all();
    setup(); // Configura tudo

    uint16_t adc_value_x = 0;
    uint16_t adc_value_y = 0;

    while (true) {
        adc_select_input(0);
        adc_value_y = adc_read();
        adc_select_input(1);
        adc_value_x = adc_read();

        update_display(adc_value_x, adc_value_y);

        sleep_ms(100);
    }
}

void setup() {
    // Inicializa GPIOs
    gpio_init(JOYSTICK_PB);
    gpio_set_dir(JOYSTICK_PB, GPIO_IN);
    gpio_pull_up(JOYSTICK_PB);
    gpio_set_irq_enabled_with_callback(JOYSTICK_PB, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

    gpio_init(BUTTON_A);
    gpio_set_dir(BUTTON_A, GPIO_IN);
    gpio_pull_up(BUTTON_A);
    gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

    gpio_init(LED_PIN_RED);
    gpio_set_dir(LED_PIN_RED, GPIO_OUT);
    gpio_init(LED_PIN_GREEN);
    gpio_set_dir(LED_PIN_GREEN, GPIO_OUT);
    gpio_init(LED_PIN_BLUE);
    gpio_set_dir(LED_PIN_BLUE, GPIO_OUT);

    // Inicializa I2C
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    // Inicializa ADC
    adc_init();
    adc_gpio_init(JOYSTICK_X_PIN);
    adc_gpio_init(JOYSTICK_Y_PIN);

    // Inicializa PWM
    uint pwm_wrap = 4096;
    pwm_red = pwm_init_gpio(LED_PIN_RED, pwm_wrap);
    pwm_blue = pwm_init_gpio(LED_PIN_BLUE, pwm_wrap);

    // Inicializa o display SSD1306
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, OLED_ADDRESS, I2C_PORT);
    ssd1306_config(&ssd);
    ssd1306_send_data(&ssd);
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);
}

void gpio_irq_handler(uint gpio, uint32_t events)
{

  // Obtém o tempo atual em microssegundos
  uint32_t current_time = to_us_since_boot(get_absolute_time());
  
  // Verifica se passou tempo suficiente desde o último evento
  if (current_time - last_time > 200000) // 200 ms de debouncing
  {
    last_time = current_time; // Atualiza o tempo do último evento
    if (gpio == JOYSTICK_PB)
    {
      gpio_put(LED_PIN_GREEN, !gpio_get(LED_PIN_GREEN));
      printf("LED verde alternado!\n");
      if(borda_atual < 94) {
        borda_atual++; 
      } else {
        borda_atual = 90;
      }
    }
    if (gpio == BUTTON_A)
    {
      leds_on = !leds_on;
      leds_toggle = true;
    }  
    a++;
  }
}

void toggle_green_led() {
    gpio_put(LED_PIN_GREEN, !gpio_get(LED_PIN_GREEN));
    printf("LED verde alternado!\n");
    if (borda_atual < 94) {
        borda_atual++;
    } else {
        borda_atual = 90;
    }
}

void toggle_pwm_leds() {
    leds_on = !leds_on;
    leds_toggle = true;
}

void update_display(uint16_t adc_value_x, uint16_t adc_value_y) {
    ssd1306_fill(&ssd, !cor);
    if (borda_atual == 90) {
        ssd1306_rect(&ssd, 6, 6, 115, 51, cor, !cor);
    } else {
        ssd1306_border_character(&ssd, 0, 0, 128, 64, cor, (char)borda_atual);
    }

    if (leds_toggle) {
        if (leds_on) {
            pwm_set_enabled(pwm_red, 1);
            pwm_set_enabled(pwm_blue, 1);
            printf("LEDs PWM ativados!\n");
            ssd1306_draw_string(&ssd, "LEDs PWM", 32, 8);
            ssd1306_draw_string(&ssd, "ativados", 32, 18);
            ssd1306_send_data(&ssd);
            sleep_ms(2000);
        } else {
            pwm_set_enabled(pwm_red, 0);
            pwm_set_enabled(pwm_blue, 0);
            printf("LEDs PWM desativados!\n");
            ssd1306_draw_string(&ssd, "LEDs PWM", 32, 8);
            ssd1306_draw_string(&ssd, "desativados", 20, 18);
            ssd1306_send_data(&ssd);
            sleep_ms(2000);
        }
        leds_toggle = false;
    }

    double screen_x16 = ((((double)adc_value_x) / 4095) * 106) + 7;
    double screen_y16 = (((4095 - ((double)adc_value_y)) / 4095) * 42) + 7;
    uint8_t screen_x = (uint8_t)screen_x16;
    uint8_t screen_y = (uint8_t)screen_y16;

    if (screen_x > 54 && screen_x < 64) {
        pwm_set_gpio_level(LED_PIN_RED, 0);
    } else {
        pwm_set_gpio_level(LED_PIN_RED, adc_value_x);
    }

    if (screen_y > 22 && screen_y < 32) {
        pwm_set_gpio_level(LED_PIN_BLUE, 0);
    } else {
        pwm_set_gpio_level(LED_PIN_BLUE, 4095 - adc_value_y);
    }

    ssd1306_rect(&ssd, screen_y, screen_x, 8, 8, cor, 1);
    ssd1306_send_data(&ssd);
}

uint pwm_init_gpio(uint gpio, uint wrap) {
    gpio_set_function(gpio, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(gpio);
    pwm_set_wrap(slice_num, wrap);
    pwm_set_enabled(slice_num, true);
    return slice_num;
}