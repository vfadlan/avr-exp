#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>
#include <ctype.h>
#include <max7219.h>

#define F_CPU 16000000UL

#define SCK PB5
#define MOSI PB3
#define LOAD PB2

uint8_t ms1 = 0;
uint8_t ms2 = 0;
uint8_t s0 = 0;
uint8_t s1 = 0;
uint8_t m0 = 0;
uint8_t m1 = 0;
uint8_t h0 = 0;
uint8_t h1 = 0;

int main() {
  TCCR1B |= (1 << 3) | 0b010; // ctc mode prescale 8
  OCR1A = 19999;
  TIMSK1 |= (1 << OCIE1A);

  SPI_init(SCK, MOSI, LOAD);
  display_init(0xFF, 0x07, 7, LOAD, 0);

  sei();
  while (1) {
    display_codeB(1, ms1, LOAD, 0);
    display_codeB(2, ms2, LOAD, 0);
    display_codeB(3, (s0 | (1 << 7)), LOAD, 0);
    display_codeB(4, s1, LOAD, 0);
    display_codeB(5, (m0 | (1 << 7)), LOAD, 0);
    display_codeB(6, m1, LOAD, 0);
    display_codeB(7, (h0 | (1 << 7)), LOAD, 0);
    display_codeB(8, h1, LOAD, 0);
  }
}

ISR (TIMER1_COMPA_vect) {
  if (ms1<9) {
    ms1++;
  } else {
    ms1 = 0;
    if (ms2<9) {
      ms2++;
    } else {
      ms2 = 0;
      if (s0<9) {
        s0++;
      } else {
        s0 = 0;
        if (s1<5) {
          s1++;
        } else {
          s1 = 0;
          if (m0<9) {
            m0++;
          } else {
            m0 = 0;
            if (m1<5) {
              m1++;
            } else {
              m1 = 0;
              if (h0<9) {
                h0++;
              } else {
                h0 = 0;
                if (h1<9) {
                  h1++;
                } else {
                  h1 = 0;
                }
              }
            }
          }
        }
      }
    }
  }
}