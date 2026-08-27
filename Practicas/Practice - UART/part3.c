#include <MKL25Z4.H>

/* Mapeo de pines: */
/* LEDs RGB: PTB18 (Rojo), PTB19 (Verde), PTD1 (Azul) */
/* ADC: Potenciómetro en PTE20 (Canal 0 / ADC0_DP0) */
/* Keypad: Filas PTC0-PTC3 (Salidas), Columnas PTC4-PTC7 (Entradas con Pull-Up) */
/* Botones: PTD4 (Botón 1) y PTD5 (Botón 2) con Pull-Up interno */

void UART0_init(void);
void UART0_sendChar(char c);
void UART0_sendString(const char *str);
int  UART0_hasChar(void);
char UART0_getChar(void);
void UART0_sendDec(unsigned int num);

void LED_init(void);
void ADC0_init(void);
unsigned short ADC0_read(void);
void keypad_init(void);
char keypad_getkey(void);
void buttons_init(void);
void delayMs(int n);

void display_main_menu(void);
void menu_led_control(void);
void menu_adc_monitoring(void);
void menu_keypad(void);
void menu_button_monitoring(void);

int main(void) {
    char option;

    UART0_init();
    LED_init();
    ADC0_init();
    keypad_init();
    buttons_init();

    delayMs(100);

    while (1) {
        display_main_menu();
        option = UART0_getChar();

        if (option >= 'a' && option <= 'z') {
            option -= 32; /* Convertir a mayuscula */
        }

        switch (option) {
            case 'L': menu_led_control(); break;
            case 'A': menu_adc_monitoring(); break;
            case 'K': menu_keypad(); break;
            case 'B': menu_button_monitoring(); break;
            default:
                UART0_sendString("Invalid command.\r\n");
                UART0_sendString("Please select an option:\r\n\r\n");
                break;
        }
    }
}

/* ================= Menus ================= */

void display_main_menu(void) {
    UART0_sendString("\r\n================================\r\n");
    UART0_sendString("KL25Z UART SYSTEM\r\n");
    UART0_sendString("================================\r\n");
    UART0_sendString("Commands:\r\n");
    UART0_sendString("L - LED control\r\n");
    UART0_sendString("A - Read ADC\r\n");
    UART0_sendString("K - Read keypad\r\n");
    UART0_sendString("B - Button status\r\n");
    UART0_sendString("================================\r\n");
    UART0_sendString("Select an option: ");
}

void menu_led_control(void) {
    char cmd;
    UART0_sendString("\r\n\r\nLED control\r\n");
    UART0_sendString("1 - Red\r\n");
    UART0_sendString("2 - Green\r\n");
    UART0_sendString("3 - Blue\r\n");
    UART0_sendString("0 - All OFF\r\n");

    while (1) {
        cmd = UART0_getChar();
        if (cmd == 'q' || cmd == 'Q') {
            break;
        }

        switch (cmd) {
            case '1':
                PTB->PCOR = (1 << 18);
                PTB->PSOR = (1 << 19);
                PTD->PSOR = (1 << 1);
                UART0_sendString("Red LED ON\r\n");
                break;
            case '2':
                PTB->PSOR = (1 << 18);
                PTB->PCOR = (1 << 19);
                PTD->PSOR = (1 << 1);
                UART0_sendString("Green LED ON\r\n");
                break;
            case '3':
                PTB->PSOR = (1 << 18);
                PTB->PSOR = (1 << 19);
                PTD->PCOR = (1 << 1);
                UART0_sendString("Blue LED ON\r\n");
                break;
            case '0':
                PTB->PSOR = (1 << 18) | (1 << 19);
                PTD->PSOR = (1 << 1);
                UART0_sendString("All LEDs OFF\r\n");
                break;
            default:
                UART0_sendString("Invalid command.\r\n");
                UART0_sendString("Please select an option:\r\n");
                break;
        }
    }
}

void menu_adc_monitoring(void) {
    unsigned short adc_val;
    unsigned int v_entero, v_dec;

    UART0_sendString("\r\n--- ADC Monitoring (Press Q to Exit) ---\r\n\r\n");

    while (1) {
        if (UART0_hasChar()) {
            char c = UART0_getChar();
            if (c == 'q' || c == 'Q') break;
        }

        adc_val = ADC0_read();
        /* Calculo sin operaciones flotantes: V = (ADC * 330) / 4095 */
        unsigned int mV = (adc_val * 330) / 4095;
        v_entero = mV / 100;
        v_dec = mV % 100;

        UART0_sendString("ADC Value: ");
        UART0_sendDec(adc_val);
        UART0_sendString("\r\nVoltage: ");
        UART0_sendDec(v_entero);
        UART0_sendChar('.');
        if (v_dec < 10) UART0_sendChar('0');
        UART0_sendDec(v_dec);
        UART0_sendString(" V\r\n\r\n");

        delayMs(500);
    }
}

void menu_keypad(void) {
    char key;

    UART0_sendString("\r\n--- Keypad Reader (Press Q on Terminal to Exit) ---\r\n");
    UART0_sendString("Press a key:\r\n");

    while (1) {
        if (UART0_hasChar()) {
            char c = UART0_getChar();
            if (c == 'q' || c == 'Q') break;
        }

        key = keypad_getkey();
        if (key != 0) {
            UART0_sendString("Key pressed: ");
            UART0_sendChar(key);
            UART0_sendString("\r\nPress a key:\r\n");
            while (keypad_getkey() != 0) { }
            delayMs(100);
        }
    }
}

void menu_button_monitoring(void) {
    int b1_state, b2_state;
    int last_b1 = -1, last_b2 = -1;

    UART0_sendString("\r\n--- Button Monitoring (Press Q to Exit) ---\r\n\r\n");

    while (1) {
        if (UART0_hasChar()) {
            char c = UART0_getChar();
            if (c == 'q' || c == 'Q') break;
        }

        /* Lectura activa en bajo: 0 = PRESSED, 1 = RELEASED */
        b1_state = (PTD->PDIR & (1 << 4)) ? 0 : 1;
        b2_state = (PTD->PDIR & (1 << 5)) ? 0 : 1;

        if (b1_state != last_b1 || b2_state != last_b2) {
            last_b1 = b1_state;
            last_b2 = b2_state;

            UART0_sendString(b1_state ? "Button 1: PRESSED\r\n" : "Button 1: RELEASED\r\n");
            UART0_sendString(b2_state ? "Button 2: PRESSED\r\n\r\n" : "Button 2: RELEASED\r\n\r\n");
            delayMs(50);
        }
    }
}

/* ================= Perifericos ================= */

void UART0_init(void) {
    SIM->SCGC4 |= 0x0400;      /* Reloj UART0 */
    SIM->SOPT2 |= 0x04000000;  /* Fuente FLL */

    UART0->C2 = 0x00;
    UART0->BDH = 0x00;
    UART0->BDL = 0x88;         /* 9600 Baud para FLL @ 20.97 MHz */
    UART0->C4 = 0x0F;          /* OSR = 16 */
    UART0->C1 = 0x00;
    UART0->C2 = 0x0C;          /* Habilitar TX y RX */

    SIM->SCGC5 |= 0x0200;      /* Reloj PORTA */
    PORTA->PCR[1] = 0x0200;    /* PTA1 = UART0_RX */
    PORTA->PCR[2] = 0x0200;    /* PTA2 = UART0_TX */
}

void UART0_sendChar(char c) {
    while (!(UART0->S1 & 0x80)) { }
    UART0->D = c;
}

void UART0_sendString(const char *str) {
    while (*str) {
        UART0_sendChar(*str++);
    }
}

void UART0_sendDec(unsigned int num) {
    char buf[10];
    int i = 0;
    if (num == 0) {
        UART0_sendChar('0');
        return;
    }
    while (num > 0) {
        buf[i++] = (num % 10) + '0';
        num /= 10;
    }
    while (i > 0) {
        UART0_sendChar(buf[--i]);
    }
}

int UART0_hasChar(void) {
    return (UART0->S1 & 0x20) ? 1 : 0;
}

char UART0_getChar(void) {
    while (!UART0_hasChar()) { }
    return UART0->D;
}

void LED_init(void) {
    SIM->SCGC5 |= 0x0400 | 0x1000;
    PORTB->PCR[18] = 0x0100;
    PORTB->PCR[19] = 0x0100;
    PORTD->PCR[1]  = 0x0100;

    PTB->PDDR |= (1 << 18) | (1 << 19);
    PTD->PDDR |= (1 << 1);

    PTB->PSOR = (1 << 18) | (1 << 19);
    PTD->PSOR = (1 << 1);
}

void ADC0_init(void) {
    SIM->SCGC6 |= 0x8000000;   /* Reloj ADC0 */
    SIM->SCGC5 |= (1 << 13);   /* Reloj PORTE */
    PORTE->PCR[20] = 0x0000;   /* PTE20 modo analogico */
    ADC0->CFG1 = 0x08;         /* 12 bits de resolucion */
    ADC0->SC2 = 0x00;          /* Disparo por software */
}

unsigned short ADC0_read(void) {
    ADC0->SC1[0] = 0;          /* Canal 0 (PTE20) */
    while (!(ADC0->SC1[0] & 0x80)) { }
    return (unsigned short)ADC0->R[0];
}

void keypad_init(void) {
    int i;
    SIM->SCGC5 |= 0x0800;      /* Reloj PORTC */
    for (i = 0; i < 4; i++) {
        PORTC->PCR[i] = 0x0100; /* Salidas Filas */
    }
    for (i = 4; i < 8; i++) {
        PORTC->PCR[i] = 0x0103; /* Entradas con Pull-Up Columnas */
    }
    PTC->PDDR = 0x0F;          /* PTC0-3 salidas, PTC4-7 entradas */
}

char keypad_getkey(void) {
    const char keymap[4][4] = {
        {'1', '2', '3', 'A'},
        {'4', '5', '6', 'B'},
        {'7', '8', '9', 'C'},
        {'*', '0', '#', 'D'}
    };
    int row, col;

    PTC->PCOR = 0x0F;
    delayMs(2);
    if ((PTC->PDIR & 0xF0) == 0xF0) {
        return 0;
    }

    for (row = 0; row < 4; row++) {
        PTC->PSOR = 0x0F;
        PTC->PCOR = (1 << row);
        delayMs(2);

        col = (PTC->PDIR >> 4) & 0x0F;
        if (col != 0x0F) {
            if (!(col & 0x01)) return keymap[row][0];
            if (!(col & 0x02)) return keymap[row][1];
            if (!(col & 0x04)) return keymap[row][2];
            if (!(col & 0x08)) return keymap[row][3];
        }
    }
    return 0;
}

void buttons_init(void) {
    SIM->SCGC5 |= 0x1000;      /* Reloj PORTD */
    PORTD->PCR[4] = 0x0103;    /* PTD4 GPIO con Pull-Up */
    PORTD->PCR[5] = 0x0103;    /* PTD5 GPIO con Pull-Up */
    PTD->PDDR &= ~((1 << 4) | (1 << 5)); /* Entradas */
}

void delayMs(int n) {
    int i;
    SysTick->LOAD = 20970 - 1; /* Retardo de 1 ms */
    SysTick->CTRL = 0x05;
    for (i = 0; i < n; i++) {
        while ((SysTick->CTRL & 0x10000) == 0) { }
    }
    SysTick->CTRL = 0;
}
