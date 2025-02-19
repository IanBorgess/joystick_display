# 🕹️ Projeto de Controle de Joystick com Display OLED e LEDs PWM 🎮

Este projeto utiliza um joystick analógico para controlar a posição de um retângulo em um display OLED, além de alternar o estado de LEDs PWM com botões. O código foi desenvolvido para a plataforma **Raspberry Pi Pico** usando o SDK da Raspberry Pi.

---

## 🛠️ Componentes Utilizados
- **Raspberry Pi Pico** 🍓
- **Display OLED SSD1306** (128x64 pixels) 📟
- **Joystick Analógico** 🕹️
- **LEDs RGB** (Vermelho, Verde e Azul) 💡
- **Botões** para interação 🔘

---

## 📋 Funcionalidades
- **Controle de Joystick**: O eixo X e Y do joystick controlam a posição de um retângulo no display OLED.
- **Botões**:
  - **Botão do Joystick**: Alterna o estado do LED verde e muda o estilo da borda no display.
  - **Botão A**: Liga/desliga os LEDs PWM (Vermelho e Azul).
- **Display OLED**:
  - Exibe um retângulo que se move com o joystick.
  - Mostra mensagens quando os LEDs PWM são ativados/desativados.
- **LEDs PWM**:
  - O LED Vermelho é controlado pelo eixo X do joystick.
  - O LED Azul é controlado pelo eixo Y do joystick.

---

## 🚀 Como Configurar

### 📦 Dependências
- **Bibliotecas**:
  - `hardware/adc.h`
  - `hardware/pwm.h`
  - `hardware/i2c.h`
  - `ssd1306.h`
  - `font.h`

### 🔌 Conexões
| Componente       | GPIO do Pico |
|------------------|--------------|
| **OLED SDA**     | GPIO 14      |
| **OLED SCL**     | GPIO 15      |
| **Joystick X**   | GPIO 27      |
| **Joystick Y**   | GPIO 26      |
| **Joystick Botão** | GPIO 22    |
| **Botão A**      | GPIO 5       |
| **LED Vermelho** | GPIO 13      |
| **LED Verde**    | GPIO 11      |
| **LED Azul**     | GPIO 12      |

---

# 🎮 Como Usar

## Joystick
- **Mova o joystick**: Controla a posição do retângulo no display.
- **Pressione o botão do joystick**: Alterna o estado do LED verde e muda o estilo da borda no display.

## Botão A
- **Pressione**: Liga/desliga os LEDs PWM (Vermelho e Azul).

## Display OLED
- **Mostra o retângulo**: Controlado pelo joystick.
- **Exibe mensagens**: Quando os LEDs PWM são ativados/desativados.

---

# 📝 Código

O código está organizado em funções modulares:

- **`setup()`**: Configura GPIOs, I2C, ADC, PWM e o display OLED.
- **`gpio_irq_handler()`**: Lida com as interrupções dos botões.
- **`update_display()`**: Atualiza o display com base nos valores do joystick.
- **`toggle_green_led()`**: Alterna o estado do LED verde.
- **`toggle_pwm_leds()`**: Alterna o estado dos LEDs PWM.