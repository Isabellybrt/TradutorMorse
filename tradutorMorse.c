#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include "inc/ssd1306.h"
#include "hardware/i2c.h"
#include <string.h>

// Definições de pinos
#define BUTTON_PIN 5          // Pino do botão para entrada de código Morse
#define BUTTON_PRINT_PIN 6     // Pino do botão para imprimir a letra ou espaço
#define BUZZER_PIN 21          // Pino do buzzer para feedback sonoro
#define BUZZER_FREQUENCY 1000  // Frequência do buzzer em Hz

// Definições dos pinos I2C para o display OLED
const uint I2C_SDA = 14;       // Pino SDA do I2C
const uint I2C_SCL = 15;       // Pino SCL do I2C

// Definições de tempo para o código Morse
#define DURATION 299           // Duração de referência para distinguir ponto (.) de traço (-) em milissegundos
#define LETTER_SPACE 1500      // Tempo de espera entre letras em milissegundos
#define WORD_SPACE 3000        // Tempo de espera entre palavras em milissegundos

// Tabela de código Morse para letras (A-Z) e números (0-9)
const char *morse_table[36] = {
    ".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", ".---",  // A-J
    "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.", "...", "-",   // K-T
    "..-", "...-", ".--", "-..-", "-.--", "--..",                          // U-Z
    "-----", ".----", "..---", "...--", "....-", ".....", "-....", "--...", "---..", "----." // 0-9
};
const char letters[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";  // Letras e números correspondentes à tabela Morse

// Buffer para armazenar o código Morse digitado
char morse_buffer[20];  // Buffer para armazenar até 20 caracteres Morse
int buffer_index = 0;   // Índice atual no buffer

// Variáveis para controle de tempo e estado dos botões
absolute_time_t press_start_time;       // Tempo de início da pressão do botão 5
bool button_pressed = false;            // Estado do botão 5 (pressionado ou não)
bool button_print_pressed = false;      // Estado do botão 6 (pressionado ou não)
absolute_time_t press_start_time_6;     // Tempo de início da pressão do botão 6

// Configuração do PWM para o buzzer
void setup_pwm(uint pin, uint freq) {
    uint slice = pwm_gpio_to_slice_num(pin);  // Obtém o slice do PWM correspondente ao pino
    uint32_t clock_freq = clock_get_hz(clk_sys);  // Obtém a frequência do clock do sistema
    uint32_t divider = clock_freq / (freq * 256);  // Calcula o divisor para a frequência desejada

    pwm_set_clkdiv(slice, divider);  // Configura o divisor de clock
    pwm_set_wrap(slice, 256);        // Configura o valor máximo do contador PWM
    pwm_set_chan_level(slice, pwm_gpio_to_channel(pin), 0);  // Inicializa o PWM sem som
    pwm_set_enabled(slice, true);    // Habilita o PWM
}

// Liga o buzzer
void buzzer_on() {
    uint slice = pwm_gpio_to_slice_num(BUZZER_PIN);
    pwm_set_chan_level(slice, pwm_gpio_to_channel(BUZZER_PIN), 180);  // Define o nível do PWM para gerar som
}

// Desliga o buzzer
void buzzer_off() {
    uint slice = pwm_gpio_to_slice_num(BUZZER_PIN);
    pwm_set_chan_level(slice, pwm_gpio_to_channel(BUZZER_PIN), 0);  // Define o nível do PWM para silenciar
}

// Converte código Morse para caractere
char morse_to_char(const char *morse) {
    for (int i = 0; i < 36; i++) {
        if (strcmp(morse, morse_table[i]) == 0) {
            return letters[i];  // Retorna o caractere correspondente ao código Morse
        }
    }
    return '?';  // Retorna '?' se o código Morse não for encontrado
}

// Exibe texto no display OLED
void display_text(const char *text) {
    // Define a área de renderização no display
    struct render_area frame_area = {
        .start_column = 0,
        .end_column = ssd1306_width - 1,
        .start_page = 0,
        .end_page = ssd1306_n_pages - 1
    };
    calculate_render_area_buffer_length(&frame_area);

    // Limpa o buffer do display
    uint8_t ssd[ssd1306_buffer_length];
    memset(ssd, 0, ssd1306_buffer_length);

    // Desenha o texto no buffer do display
    ssd1306_draw_string(ssd, 5, 0, (char*)text);

    // Renderiza o buffer no display
    render_on_display(ssd, &frame_area);
}

// Exibe texto inicial no display OLED
void display_text_inicial() {
    struct render_area frame_area = {
        start_column : 0,
        end_column : ssd1306_width - 1,
        start_page : 0,
        end_page : ssd1306_n_pages - 1
    };

    calculate_render_area_buffer_length(&frame_area);

    // Limpa o display
    uint8_t ssd[ssd1306_buffer_length];
    memset(ssd, 0, ssd1306_buffer_length);
    render_on_display(ssd, &frame_area);

    // Texto inicial a ser exibido
    char *text[] = {
        "Aperte o botao ",
        "A para fazer ",
        "o codigo morse, ",
        "e aperte o ",
        "botao B para ",
        "enviar!"
    };

    // Desenha cada linha do texto no display
    int y = 0;
    for (uint i = 0; i < count_of(text); i++) {
        ssd1306_draw_string(ssd, 2, y, text[i]);
        y += 8;  // Move para a próxima linha
    }
    render_on_display(ssd, &frame_area);
}

void draw_dot(uint8_t *ssd, int x, int y) {
    // Desenha um pequeno ponto no display
    for (int dx = 0; dx < 3; dx++) {
        for (int dy = 0; dy < 3; dy++) {
            ssd1306_set_pixel(ssd, x + dx, y + dy, true);

        }
    }
}

void draw_dash(uint8_t *ssd, int x, int y) {
    // Desenha um traço (linha horizontal)
    for (int dx = 0; dx < 8; dx++) {
        ssd1306_set_pixel(ssd, x + dx, y, true);
        ssd1306_set_pixel(ssd, x + dx, y + 1, true);
    
    }
}

// Exibe o código morse sendo feito no display OLED
void display_morse_buffer() {
    struct render_area frame_area = {
        .start_column = 0,
        .end_column = ssd1306_width - 1,
        .start_page = 0,
        .end_page = ssd1306_n_pages - 1
    };
    calculate_render_area_buffer_length(&frame_area);

    uint8_t ssd[ssd1306_buffer_length];
    memset(ssd, 0, ssd1306_buffer_length);

    // Define a posição inicial
    int x = 10, y = 20;

    // Desenha cada caractere Morse na tela
    for (int i = 0; i < buffer_index; i++) {
        if (morse_buffer[i] == '.') {
            draw_dot(ssd, x, y);
            x += 6;  // Espaço após um ponto
        } else if (morse_buffer[i] == '-') {
            draw_dash(ssd, x, y);
            x += 12;  // Espaço maior após um traço
        }
    }

    render_on_display(ssd, &frame_area);
    sleep_ms(100);
}


// Função principal
int main() {
    stdio_init_all();  // Inicializa a comunicação serial

    // Inicializa o I2C para o display OLED
    i2c_init(i2c1, ssd1306_i2c_clock * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    // Inicializa o display OLED
    ssd1306_init();
    display_text_inicial();  // Exibe o texto inicial no display

    // Configura o botão 5 para entrada de código Morse
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);

    // Configura o botão 6 para imprimir letras ou espaços
    gpio_init(BUTTON_PRINT_PIN);
    gpio_set_dir(BUTTON_PRINT_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PRINT_PIN);

    // Configura o buzzer como saída PWM
    gpio_set_function(BUZZER_PIN, GPIO_FUNC_PWM);
    setup_pwm(BUZZER_PIN, BUZZER_FREQUENCY);

    absolute_time_t last_press_time = {0};  // Armazena o tempo da última pressão do botão
    char phrase[100] = "";  // Armazena a frase digitada

    // Trecho do código principal onde o morse_buffer é atualizado
    while (true) {
        // Verifica o estado do botão 5 (entrada de código Morse)
        if (gpio_get(BUTTON_PIN) == 0) {  // Botão pressionado
            if (!button_pressed) {
                press_start_time = get_absolute_time();  // Marca o início da pressão
                buzzer_on();  // Liga o buzzer
                button_pressed = true;  // Marca que o botão foi pressionado
            }
        } else {  // Botão solto
            if (button_pressed) {
                int press_duration = absolute_time_diff_us(press_start_time, get_absolute_time()) / 1000;

                buzzer_off();  // Desliga o buzzer

                // Determina se foi um ponto (.) ou traço (-) com base na duração da pressão
                if (press_duration < DURATION) {
                    morse_buffer[buffer_index++] = '.';
                } else if (press_duration > DURATION) {
                    morse_buffer[buffer_index++] = '-';
                }

                morse_buffer[buffer_index] = '\0';  // Finaliza a string no buffer

                // Exibe o código Morse digitado no display
                display_morse_buffer();  // Atualiza o display com o novo conteúdo do buffer

                button_pressed = false;
            }
        }

        // Verifica o estado do botão 6 (imprimir letra ou espaço)
        if (gpio_get(BUTTON_PRINT_PIN) == 0) {  // Botão pressionado
            if (!button_print_pressed) {
                press_start_time_6 = get_absolute_time();  // Marca o início da pressão
                // Converte o código Morse no buffer para uma letra
                if (buffer_index > 0) {
                    char decoded_char = morse_to_char(morse_buffer);

                    // Adiciona a letra à frase
                    char letter_str[2] = {decoded_char, '\0'};
                    strcat(phrase, letter_str);

                    // Exibe a frase no display
                    display_text(phrase);

                    // Limpa o buffer para a próxima letra
                    buffer_index = 0;
                    morse_buffer[0] = '\0';
                }

                button_print_pressed = true;  // Marca que o botão foi pressionado
            }

            // Verifica se o botão foi pressionado por mais de 1 segundo para adicionar um espaço
            int press_duration_6 = absolute_time_diff_us(press_start_time_6, get_absolute_time()) / 1000;
            if (press_duration_6 > 1000) {
                strcat(phrase, " ");  // Adiciona um espaço à frase
                button_print_pressed = false;  // Permite pressionar novamente
            }
        }

        // Verifica se o botão 6 foi solto
        if (gpio_get(BUTTON_PRINT_PIN) == 1) {
            button_print_pressed = false;
        }

        sleep_ms(100);  // Aguarda 100ms para reduzir o uso da CPU
    }

    return 0;
}