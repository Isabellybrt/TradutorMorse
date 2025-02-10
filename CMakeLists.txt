# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 1.5.1)
set(toolchainVersion 13_3_Rel1)
set(picotoolVersion 2.0.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Definição da placa Pico W
set(PICO_BOARD pico_w CACHE STRING "Board type")

# Importa o SDK do Raspberry Pi Pico
include(pico_sdk_import.cmake)

# Define o nome do projeto
project(tradutorMorse C CXX ASM)

# Inicializa o SDK da Pico
pico_sdk_init()

# Adiciona o executável com os arquivos do projeto
add_executable(tradutorMorse 
    tradutorMorse.c 
    inc/ssd1306_i2c.c
)

# Define nome e versão do programa
pico_set_program_name(tradutorMorse "tradutorMorse")
pico_set_program_version(tradutorMorse "0.1")

# Habilita a saída padrão via USB (desativando UART)
pico_enable_stdio_uart(tradutorMorse 0)
pico_enable_stdio_usb(tradutorMorse 1)

# Adiciona as bibliotecas padrão da Pico
target_link_libraries(tradutorMorse 
    pico_stdlib 
    hardware_i2c 
    hardware_pwm 
    hardware_gpio 
    hardware_clocks
)

# Inclui diretórios de cabeçalhos necessários
target_include_directories(tradutorMorse PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/inc
)

# Gera arquivos binários adicionais
pico_add_extra_outputs(tradutorMorse)
