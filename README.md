# Tradutor de Código Morse com BitDogLab

## Descrição

Este projeto implementa um tradutor de código Morse utilizando a placa BitDogLab, que é integrada com placa Raspberry Pi Pico W. O sistema permite que o usuário insira códigos Morse através de um botão e receba a tradução em caracteres alfanuméricos em um display OLED. O sistema também emite feedback sonoro com um buzzer para indicar quando o botão está sendo pressionado.

Desenvolvido em **linguagem C**, este projeto proporciona uma oportunidade para aprender sobre **sistemas embarcados** e o funcionamento do **código Morse**.

## Link do vídeo da aparesentação
https://drive.google.com/file/d/18RTwHbdvNjdn3vchJ3-gSbTGpBu8hIWH/view?usp=drivesdk

## 📌 Funcionalidades

- Entrada de código Morse via botão.
- Feedback sonoro durante a pressão do botão.
- Tradução do código Morse para caracteres alfanuméricos.
- Exibição do texto traduzido em um display OLED.
- Suporte a letras (A-Z) e números (0-9).
- Utilização de **PWM** (Modulação por Largura de Pulso) para controle preciso do buzzer.

## 🛠️ Hardware Utilizado
- **Placa BitDogLab** (com botões, buzzer e display OLED integrados)
- Raspberry Pi Pico (processamento e controle)

## 🚀 GPIOs utilizados:

| Componente            | GPIO |
|----------------------|------|
| Botão de entrada de Morse | 5  |
| Botão de confirmação      | 6  |
| Buzzer                   | 21 |
| SDA                      | 14 |
| SCL                      | 15 |

# 📂 Estrutura do Projeto

## Pastas do Projeto

**Inc:** Contém os arquivos fundamentais para rodar o display OLED.

**Images:** Contém a imagem do código Morse para testar a tradução.

------------------------------------------------------------------------

Para a execução correta do display OLED, é necessário ter a pasta inc, que está disponível no repositório do projeto. Dentro dessa pasta, devem estar os arquivos ssd1306, já incluídos.

Com as pastas inclusa no seu projeto, será necessário configurar o arquivo cmakelist.txt

### Configuração do CMakeLists.txt

```cmake
add_executable(tradutorMorse // Nome da sua pasta
    tradutorMorse.c //Lembre-se de trocar esse nome pelo o nome do seu arquivo .c, caso esteja fazendo do 0 sem importar do meu.
    inc/ssd1306_i2c.c
)

# Adiciona as bibliotecas padrão da Pico
target_link_libraries(tradutorMorse // Nome da sua pasta
    pico_stdlib 
    hardware_i2c 
    hardware_pwm 
    hardware_gpio 
    hardware_clocks
)
```

### Bibliotecas Utilizadas

```cpp
#include <stdio.h>              // Biblioteca padrão para entrada e saída
#include "pico/stdlib.h"         // Biblioteca para funcionalidades básicas da Raspberry Pi Pico
#include "hardware/gpio.h"      // Biblioteca para controle de GPIOs
#include "hardware/pwm.h"       // Biblioteca para controle de PWM (Pulse Width Modulation)
#include "hardware/clocks.h"    // Biblioteca para manipulação de clocks
#include "inc/ssd1306.h"        // Biblioteca para controle do display OLED
#include "hardware/i2c.h"       // Biblioteca para controle de comunicação I2C
```

Essas bibliotecas são responsáveis por fornecer as funcionalidades essenciais para a operação do projeto, incluindo controle de entrada/saída, controle do buzzer, manipulação do display OLED, e comunicação via I2C para o display.

## 📜 Como funciona o código?

### 📌 Configuração do PWM para o Buzzer
```cpp
void setup_pwm(uint pin, uint freq) {
    uint slice = pwm_gpio_to_slice_num(pin);  // Obtém o slice do PWM correspondente ao pino
    uint32_t clock_freq = clock_get_hz(clk_sys);  // Obtém a frequência do clock do sistema
    uint32_t divider = clock_freq / (freq * 256);  // Calcula o divisor para a frequência desejada

    pwm_set_clkdiv(slice, divider);  // Configura o divisor de clock
    pwm_set_wrap(slice, 256);        // Configura o valor máximo do contador PWM
    pwm_set_chan_level(slice, pwm_gpio_to_channel(pin), 0);  // Inicializa o PWM sem som
    pwm_set_enabled(slice, true);    // Habilita o PWM
}
```
Esta função inicializa e configura o **PWM** para controlar o buzzer. O **PWM** permite gerar frequências específicas para a reprodução do código Morse.

### 🔊 Ativar o Buzzer
```cpp
void buzzer_on() {
    uint slice = pwm_gpio_to_slice_num(BUZZER_PIN);
    pwm_set_chan_level(slice, pwm_gpio_to_channel(BUZZER_PIN), 180);  // Define o nível do PWM para gerar som
}
```
Ativa o buzzer configurando o sinal PWM para emitir um som. A frequência do som pode ser ajustada para representar o código Morse.

### 🔇 Desativar o Buzzer
```cpp
void buzzer_off() {
    uint slice = pwm_gpio_to_slice_num(BUZZER_PIN);
    pwm_set_chan_level(slice, pwm_gpio_to_channel(BUZZER_PIN), 0);  // Define o nível do PWM para silenciar
}
```
Desativa o buzzer interrompendo o sinal PWM, silenciando o som.

### ⏳ Morse para Caractere
```cpp
char morse_to_char(const char *morse) {
    for (int i = 0; i < 36; i++) {
        if (strcmp(morse, morse_table[i]) == 0) {
            return letters[i];  // Retorna o caractere correspondente ao código Morse
        }
    }
    return '?';  // Retorna '?' se o código Morse não for encontrado
}
```
Esta função converte código morse para caractere, ou seja, transforma em letra ou número.

### 📟 Exibir texto no Display OLED
```cpp
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
```
Essa função exibe no display OLED a tradução do código Morse inserido pelo usuário.

### 📟 Exibir texto inicial no Display OLED
```cpp
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
```
Essa função exibe no display OLED a mensagem inicial para um breve tutorial.

### 🔄 Função Principal
```cpp
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
```
A função principal que mantém o sistema em execução, verificando os botões pressionados, tocando os sons do buzzer e atualizando o display OLED com a tradução correspondente. Se o botão do pino 5 for pressionado por menos de 299ms ele vai gerar um ".", e se for pressionado por mais de 299ms ele vai gerar um "-". Já o botão do pino 6 envia o código morse gerado para ser transformada em caractere, se esse botão do pino 6 for pressionada por mais de 1 segundo, ele vai gerar um espaço no display oled, assim sendo possível criar frases utilizando o código morse.

## Uso

1. Pressione o botão de entrada (GPIO 5) para digitar um código Morse:
   - Pressões curtas representam `.` (ponto).
   - Pressões longas representam `-` (traço).
   - O buzzer emite um som enquanto o botão está pressionado.
2. Pressione o botão de confirmação (GPIO 6) para traduzir o código Morse inserido.
3. O caractere correspondente é exibido no display OLED.
4. Para adicionar um espaço entre palavras, mantenha o botão de confirmação pressionado por mais de 1 segundo.

No repositório há uma pasta images que contém uma **tabela do código morse** para poder praticar e testar

## 📜 Licença
Este projeto é de código aberto e pode ser utilizado livremente.

## Autor

Projeto desenvolvido por Maria Isabelly de Brito Rodrigues.


