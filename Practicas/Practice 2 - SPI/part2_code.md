#include <MKL25Z4.H>
#include <stdbool.h>

void SPI0_init(void);
void max7219_write(unsigned char command, unsigned char data);
void display_number(uint16_t number);
void GPIO_buttons_init(void);
void delay_ms(uint32_t ms);

#define DECODE     9
#define INTENSITY 10
#define SCANLIMIT 11
#define SHUTDOWN  12
#define TEST      15

// Push button definitions (using Port A pins with internal pull-ups)
// PTA1: Play/Pause
// PTA2: Increment/Decrement Mode Toggle
// PTA4: Manual Step / Reset (Short press = Step when paused, Long press/Dedicated = Reset)
#define BTN_PLAY_PAUSE (1 << 1)
#define BTN_MODE       (1 << 2)
#define BTN_ACTION     (1 << 4)

int main(void) {
    SPI0_init(); // Initialize SPI module and GPIO pins
    GPIO_buttons_init(); // Initialize push buttons

    // MAX7219 Configuration
    max7219_write(DECODE, 0x0F);
    max7219_write(SCANLIMIT, 3);    // Scan 4 digits (0, 1, 2, 3)
    max7219_write(INTENSITY, 4);    // Set brightness level (0 to 15)
    max7219_write(TEST, 0);         // Turn off display test mode
    max7219_write(SHUTDOWN, 1);     // Power on display from sleep

    uint16_t count = 0;
    bool running = false;
    bool increment_mode = true; // true = increment (up), false = decrement (down)

    display_number(count);

    while(1) {
        // 1. Play/Pause Button Check (PTA1)
        if (!(GPIOA->PDIR & BTN_PLAY_PAUSE)) {
            delay_ms(50); // Debounce delay
            while (!(GPIOA->PDIR & BTN_PLAY_PAUSE)); // Wait for release
            running = !running;
        }

        // 2. Increment/Decrement Mode Button Check (PTA2)
        if (!(GPIOA->PDIR & BTN_MODE)) {
            delay_ms(50);
            while (!(GPIOA->PDIR & BTN_MODE));
            increment_mode = !increment_mode;
        }

        // 3. Action Button Check (PTA4) - Manual adjustment or Reset
        if (!(GPIOA->PDIR & BTN_ACTION)) {
            delay_ms(50);
            if (!(GPIOA->PDIR & BTN_ACTION)) {
                // Check for Reset condition or Manual step
                // If running, action button can act as a direct Reset
                // If paused, action button performs manual step (+1 or -1)
                if (!running) {
                    if (increment_mode) {
                        if (count < 9999) count++;
                    } else {
                        if (count > 0) count--;
                    }
                } else {
                    // Reset when running or explicit reset trigger
                    count = increment_mode ? 0 : 9999;
                }
                display_number(count);
                while (!(GPIOA->PDIR & BTN_ACTION)); // Wait for release
            }
        }

        // Automatic counting when running
        if (running) {
            if (increment_mode) {
                if (count < 9999) count++;
                else count = 0; // Wrap around or stop at limit
            } else {
                if (count > 0) count--;
                else count = 9999;
            }
            display_number(count);
            delay_ms(300); // Speed of automatic counter
        }
    }
}

void SPI0_init(void) {
    SIM->SCGC5 |= 0x1000;    // Enable clock for Port D

    PORTD->PCR[1] = 0x200;   // Set PTD1 as SPI0_SCK
    PORTD->PCR[2] = 0x200;   // Set PTD2 as SPI0_MOSI
    PORTD->PCR[0] = 0x100;   // Set PTD0 as GPIO
    PTD->PDDR |= 0x01;       // Set PTD0 direction as Output for CS
    PTD->PSOR = 0x01;        // CS idle High

    SIM->SCGC4 |= 0x400000;  // Enable clock for SPI0

    SPI0->C1 = 0x10;         // Disable SPI module and set KL25Z as Master
    SPI0->BR = 0x60;         // Set Baud rate divider (Baud Rate = 1 MHz)
    SPI0->C1 |= 0x40;        // Enable SPI module
}

void GPIO_buttons_init(void) {
    SIM->SCGC5 |= 0x200;     // Enable clock for Port A

    // Configure PTA1, PTA2, PTA4 as GPIO with internal pull-up resistors enabled
    PORTA->PCR[1] = 0x103;   // GPIO, Pull-select enabled, Pull-up
    PORTA->PCR[2] = 0x103;   // GPIO, Pull-select enabled, Pull-up
    PORTA->PCR[4] = 0x103;   // GPIO, Pull-select enabled, Pull-up

    // Set directions as input
    GPIOA->PDDR &= ~(BTN_PLAY_PAUSE | BTN_MODE | BTN_ACTION);
}

void max7219_write(unsigned char command, unsigned char data) {
    volatile char dummy;
    PTD->PCOR = 1;               // Drop CS low to select MAX7219

    while(!(SPI0->S & 0x20)) { } // Wait until SPTEF bit (Transmit Buffer Empty)
    SPI0->D = command;           // Send command byte first
    while(!(SPI0->S & 0x80)) { } // Wait until SPRF bit (Receive Complete)
    dummy = SPI0->D;             // Read SPI0->D to clear flag

    while(!(SPI0->S & 0x20)) { } // Wait until SPTEF is 1
    SPI0->D = data;              // Send data byte
    while(!(SPI0->S & 0x80)) { } // Wait until SPRF is 1
    dummy = SPI0->D;             // Read SPI0->D to clear flag

    PTD->PSOR = 1;               // Drive CS high to latch command into MAX7219
}

void display_number(uint16_t number) {
    if (number > 9999) number = 9999;
    max7219_write(1, number % 10);          // Ones place -> DIG 0 (Rightmost)
    max7219_write(2, (number / 10) % 10);   // Tens place -> DIG 1
    max7219_write(3, (number / 100) % 10);  // Hundreds place -> DIG 2
    max7219_write(4, (number / 1000) % 10); // Thousands place -> DIG 3 (Leftmost)
}

void delay_ms(uint32_t ms) {
    for (volatile uint32_t i = 0; i < ms * 4000; i++) {
        // Simple software delay loop adjusted for core clock
    }
}
