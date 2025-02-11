# Tradutor de Código Morse com BitDogLab

## Descrição

Este projeto implementa um tradutor de código Morse utilizando a placa BitDogLab, que é integrada com placa Raspberry Pi Pico W. O sistema permite que o usuário insira códigos Morse através de um botão e receba a tradução em caracteres alfanuméricos em um display OLED. O sistema também emite feedback sonoro com um buzzer para indicar quando o botão está sendo pressionado.

Desenvolvido em **linguagem C**, este projeto proporciona uma oportunidade para aprender sobre **sistemas embarcados** e o funcionamento do **código Morse**.

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

- **Botão de entrada de Morse**: GPIO 5
- **Botão de confirmação**: GPIO 6
- **Buzzer**: GPIO 21
- **I2C (Display OLED)**:
  - SDA: GPIO 14
  - SCL: GPIO 15

## Execução

### Dependências

- [Pico SDK](https://github.com/raspberrypi/pico-sdk)
- Biblioteca para o display OLED SSD1306

### Estrutura do Projeto

Para a execução correta do display OLED, é necessário ter a pasta `inc`, que está disponível no repositório do projeto. Dentro da pasta `inc`, devem estar os arquivos `ssd1306`, que já vêm incluídos nela.

Com a pasta inclusa no seu projeto, será necessário configurar no cmakelist.txt essas implementações: 

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

## 📜 Como funciona o código?

### 📌 Configuração do PWM para o Buzzer
```cpp
void setup_pwm(uint pin, uint freq)
```
Esta função inicializa e configura o **PWM** para controlar o buzzer. O **PWM** permite gerar frequências específicas para a reprodução do código Morse.

### 🔊 Ativar o Buzzer
```cpp
void buzzer_on()
```
Ativa o buzzer configurando o sinal PWM para emitir um som. A frequência do som pode ser ajustada para representar o código Morse.

### 🔇 Desativar o Buzzer
```cpp
void buzzer_off()
```
Desativa o buzzer interrompendo o sinal PWM, silenciando o som.

### ⏳ Morse para Caractere
```cpp
char morse_to_char(const char *morse)
```
Esta função converte código morse para caractere, ou seja, transforma em letra ou número.

### 📟 Exibir texto no Display OLED
```cpp
void display_text(const char *text)
```
Essa função exibe no display OLED a tradução do código Morse inserido pelo usuário.

### 📟 Exibir texto inicial no Display OLED
```cpp
void display_text_inicial()
```
Essa função exibe no display OLED a mensagem inicial para um breve tutorial.

### 🔄 Função Principal
```cpp
int main()
```
A função principal que mantém o sistema em execução, verificando os botões pressionados, tocando os sons do buzzer e atualizando o display OLED com a tradução correspondente.

## Uso

1. Pressione o botão de entrada (GPIO 5) para digitar um código Morse:
   - Pressões curtas representam `.` (ponto).
   - Pressões longas representam `-` (traço).
   - O buzzer emite um som enquanto o botão está pressionado.
2. Pressione o botão de confirmação (GPIO 6) para traduzir o código Morse inserido.
3. O caractere correspondente é exibido no display OLED.
4. Para adicionar um espaço entre palavras, mantenha o botão de confirmação pressionado por mais de 1 segundo.

No repositório há uma pasta images que contém uma **tabela do código morse** para pode praticar e testar

## 📜 Licença
Este projeto é de código aberto e pode ser utilizado livremente.

## Autor

Projeto desenvolvido por Maria Isabelly de Brito Rodrigues.
