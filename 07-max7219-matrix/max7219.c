#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <ctype.h>

#define F_CPU 16000000UL

#define SCK PB5
#define MOSI PB3
#define LOAD PB2

#define MAX_TEST 0xF
#define MAX_SHUTDOWN 0xC
#define MAX_SCAN 0xB
#define MAX_INTENSITY 0xA
#define MAX_MODE 0x9

void SPI_init(void);
void SPI_transmit(uint8_t);
void send_instruction(uint16_t);
void display_test(void);
void display_init(uint8_t, uint8_t, uint8_t);
void display(char);
uint8_t reverse_byte(uint8_t);

const uint8_t EMOJI_NUM = 6;
const uint64_t IMAGES[] = {
  0x00183C7EFFFF6600, // 0 heart emoji
  0x3C4299A581A5423C, // 1 smile emoji
  0x3C42A59981A5423C, // 2 sad face emoji
  0x3C4281BD81A5423C, // 3 flat face emoji
  0x040A112040800000, // 4 v
  0x8142241818244281, // 5 x
  0x3c3c1818181e1c18,
  0x7e7e1c3060667e3c,
  0x3c7c603878607c3c,
  0x60607f7f666c7870,
  0x1e3e30301e063e3e,
  0x1c3e26261e063e1c,
  0x0c0c183030607e7e,
  0x3c24243c3c24243c,
  0x387c607864647c38,
  0x3c7e666666667e3c,
  0x66667e7e66667e3c,
  0x3e66261e26663e1c,
  0x387c0e06060e7c38,
  0x1e3e366666363e1e,
  0x3e3e061e1e063e3e,
  0x0606061e1e063e3e,
  0x3c7e667606067e3c,
  0x6666667e7e666666,
  0x3c1818181818183c,
  0x0c3c303030303c3c,
  0x6666361e1e366666,
  0x7c7c0c0c0c0c0c0c,
  0xc3c3c3c3dbffe7c3,
  0x6676767e7e6e6e66,
  0x3c7e666666667e3c,
  0x06063e7e66667e3e,
  0xc07c665646667e3c,
  0x66361e3e76667e3e,
  0x1e3e303c1e063e3c,
  0x1818181818187e7e,
  0x3c7e666666666666,
  0x183c666666666642,
  0xffffdbdbdbdbc3c3,
  0x66663c18183c6666,
  0x181818183c246666,
  0x7e7e0c1830607e7e,
  0x5c7e667e7c607c3c,
  0x3a7e66663e060606,
  0x3c7e06067e3c0000,
  0x5c7e66667c606060,
  0x3c7e063e66663c00,
  0x0606063e063e1c00,
  0x1c22203c22223c00,
  0x6666667e3e060606,
  0x1818181818001818,
  0x081c363030300030,
  0x66361e0e1e360606,
  0x301818181818181c,
  0x4242425a5a660000,
  0x464646464e360000,
  0x1824242418000000,
  0x1824242418000000,
  0x04041c2424180000,
  0x2020382424180000,
  0x0404040c34000000,
  0x1c20180438000000,
  0x1028081c08080000,
  0x2c34242400000000,
  0x1824424242420000,
  0x446e918181000000,
  0x4428102844000000,
  0x1824203824242400,
  0x3c0418203c000000,
  0x1818001818181818,
  0x0800080810202418,
};
const int IMAGES_LEN = sizeof(IMAGES)/8;

int main(void) {
  SPI_init();
  display_init(0x00, 0x07, 7);
  uint8_t d;

  while (1) {
    for (int i=0; i<IMAGES_LEN; i++) {
      for (int j=0; j<8; j++) {
        d = ((IMAGES[i] >> (8 * j)) & 0xFF );
        send_instruction(((8-j) << 8) | d);
      }
      _delay_ms(75);
    }
    display('H');
    _delay_ms(500);
    display('I');
    _delay_ms(500);
    display(0);
    _delay_ms(500);
    display(1);
    _delay_ms(500);
  }
}

void SPI_init(void) {
  DDRB |= (1 << SCK) | (1 << MOSI) | (1 << LOAD);
  SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);
  PORTB |= (1 << LOAD);
}

void SPI_transmit(uint8_t data) {
  SPDR = data;
  while (!(SPSR & (1 << SPIF)));
}

void send_instruction(uint16_t instruction) {
  SPI_transmit(instruction >> 8);
  SPI_transmit(instruction & 0xFF);
  PORTB &= (~(1 << LOAD));
  _delay_us(50);
  PORTB |= (1 << LOAD);
}

void display_test(void) {
  send_instruction((MAX_TEST << 8) | 1);
  _delay_ms(100);
  send_instruction((MAX_TEST << 8));
}

void display_init(uint8_t decode_mode, uint8_t scan_limit, uint8_t intensity) {
  send_instruction((MAX_SHUTDOWN << 8) | 1);
  display_test();
  send_instruction((MAX_MODE << 8) | decode_mode);
  send_instruction((MAX_SCAN<< 8) | (scan_limit & 0x7));
  send_instruction((MAX_INTENSITY<< 8) | (intensity & 0xF));
}

uint8_t reverse_byte(uint8_t b) {
    b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
    b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
    b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
    return b;
}

void display(char c) {
  uint64_t image;
  uint8_t d;

  if (isdigit(c)) {
    image = IMAGES[c - '0' + EMOJI_NUM];
  } else if (isupper(c)) {
    image = IMAGES[c - 'A' + 10 + EMOJI_NUM];
  } else if (islower(c)) {
    image = IMAGES[c - 'a' + 36 + EMOJI_NUM];
  } else {
    image = IMAGES[c];
  }

  for (int j=0; j<8; j++) {
    d = ((image >> (8 * j)) & 0xFF );
    send_instruction(((8-j) << 8) | d);
  }
}