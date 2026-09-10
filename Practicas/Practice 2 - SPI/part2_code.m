#include <MKL25Z4.H>
#include <stdbool.h>

void SPI0_init(void);
void max7219_write(unsigned char command, unsigned char data);
void display_number(uint16_t number);
void GPIO_buttons_init(void);
void SysTick_init(void);

#define DECODE    9
#define INTENSITY 10
#define SCANLIMIT 11
#define SHUTDOWN  12
#define TEST      15
#define BTN_PLAY_PAUSE (1 << 1)
#define BTN_MODE       (1 << 2)
#define BTN_STEP       (1 << 4)
#define BTN_RESET      (1 << 5)

// Tick Counter (milisegundos)
volatile uint32_t ms_ticks = 0;
// Interrupción que se ejecuta automáticamente cada 1 ms
void SysTick_Handler(void) {
    ms_ticks++;
}

int main(void) {
    SPI0_init();
    GPIO_buttons_init();
    SysTick_init(); // Inicia el reloj de ticks a 1 ms

    max7219_write(DECODE, 0x0F);
    max7219_write(SCANLIMIT, 3);
    max7219_write(INTENSITY, 4);
    max7219_write(TEST, 0);
    max7219_write(SHUTDOWN, 1);

    uint16_t count = 0;
    bool running = false;
    bool increment_mode = true;
    // Control de tiempo (no bloqueante)
    uint32_t last_count_time = 0;
    const uint32_t COUNT_INTERVAL = 1000; // ms por número
    display_number(count);

    while(1) {
        // 1. Botón Play/Pause (PTA1)
        if (!(GPIOA->PDIR & BTN_PLAY_PAUSE)) {
            // Breve espera para debounce usando ticks
            uint32_t start_debounce = ms_ticks;
            while(ms_ticks - start_debounce < 20);

            if (!(GPIOA->PDIR & BTN_PLAY_PAUSE)) {
                running = !running;
                while (!(GPIOA->PDIR & BTN_PLAY_PAUSE));
            }
        }

        // 2. Botón Cambiar Modo (PTA2)
        if (!(GPIOA->PDIR & BTN_MODE)) {
            uint32_t start_debounce = ms_ticks;
            while(ms_ticks - start_debounce < 20);

            if (!(GPIOA->PDIR & BTN_MODE)) {
                increment_mode = !increment_mode;
                while (!(GPIOA->PDIR & BTN_MODE));
            }
        }

        // 3. Botón Paso Manual (PTA4) - Solo en Pausa
        if (!(GPIOA->PDIR & BTN_STEP)) {
            uint32_t start_debounce = ms_ticks;
            while(ms_ticks - start_debounce < 20);

            if (!(GPIOA->PDIR & BTN_STEP)) {
                if (!running) {
                    if (increment_mode) {
                        if (count < 9999) count++;
                        else count = 0;
                    } else {
                        if (count > 0) count--;
                        else count = 9999;
                    }
                    display_number(count);
                }
                while (!(GPIOA->PDIR & BTN_STEP));
            }
        }

        // 4. Botón Reset (PTA5)
        if (!(GPIOA->PDIR & BTN_RESET)) {
            uint32_t start_debounce = ms_ticks;
            while(ms_ticks - start_debounce < 20);

            if (!(GPIOA->PDIR & BTN_RESET)) {
                count = increment_mode ? 0 : 9999;
                display_number(count);
                while (!(GPIOA->PDIR & BTN_RESET));
            }
        }

        // 5. Conteo Automático mediante Ticks (No Bloqueante)
        if (running) {
            if ((ms_ticks - last_count_time) >= COUNT_INTERVAL) {
                last_count_time = ms_ticks; // Resetea el temporizador relativo

                if (increment_mode) {
                    if (count < 9999) count++;
                    else count = 0;
                } else {
                    if (count > 0) count--;
                    else count = 9999;
                }
                display_number(count);
            }
        }
    }
}

void SysTick_init(void) {
    SysTick->LOAD = (20971520u / 1000u) - 1u; // Configurado a 1 ms
    SysTick->VAL  = 0u;
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |
                    SysTick_CTRL_TICKINT_Msk   |
                    SysTick_CTRL_ENABLE_Msk;
}

void SPI0_init(void) {
    SIM->SCGC5 |= 0x1000;
    PORTD->PCR[1] = 0x200;
    PORTD->PCR[2] = 0x200;
    PORTD->PCR[0] = 0x100;
    PTD->PDDR |= 0x01;
    PTD->PSOR = 0x01;

    SIM->SCGC4 |= 0x400000;
    SPI0->C1 = 0x10;
    SPI0->BR = 0x60;
    SPI0->C1 |= 0x40;
}

void GPIO_buttons_init(void) {
    SIM->SCGC5 |= 0x200;
    PORTA->PCR[1] = 0x103;
    PORTA->PCR[2] = 0x103;
    PORTA->PCR[4] = 0x103;
    PORTA->PCR[5] = 0x103;
    GPIOA->PDDR &= ~(BTN_PLAY_PAUSE | BTN_MODE | BTN_STEP | BTN_RESET);
}

void max7219_write(unsigned char command, unsigned char data) {
    volatile char dummy;
    PTD->PCOR = 1;
    while(!(SPI0->S & 0x20)) { }
    SPI0->D = command;
    while(!(SPI0->S & 0x80)) { }
    dummy = SPI0->D;

    while(!(SPI0->S & 0x20)) { }
    SPI0->D = data;
    while(!(SPI0->S & 0x80)) { }
    dummy = SPI0->D;
    PTD->PSOR = 1;
}

void display_number(uint16_t number) {
    if (number > 9999) number = 9999;
    max7219_write(1, number % 10);
    max7219_write(2, (number / 10) % 10);
    max7219_write(3, (number / 100) % 10);
    max7219_write(4, (number / 1000) % 10);
}
