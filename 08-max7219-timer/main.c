#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <ctype.h>
#include <max7219.h>
#include <stdbool.h>

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
bool count = true;
int c = 1;

extern uint8_t EMOJI_NUM;
extern uint64_t IMAGES[];
extern uint8_t IMAGES_LEN;

int main() {
  TCCR1B = (1 << 3) | 0b010; // ctc mode prescale 8
  OCR1A = 19999;
  TIMSK1 = (1 << OCIE1A);
  PORTD = (1 << PD4);
  EICRA = (1 < ISC00);
  EIMSK = 1;


  SPI_init(SCK, MOSI, LOAD);
  display_init(0x00, 0x07, 7, LOAD, 0);
  display_init(0xFF, 0x07, 7, LOAD, 1);

  sei();
  while (1) {
    display_codeB(1, ms1, LOAD, 1);
    display_codeB(2, ms2, LOAD, 1);
    display_codeB(3, (s0 | (1 << 7)), LOAD, 1);
    display_codeB(4, s1, LOAD, 1);
    display_codeB(5, (m0 | (1 << 7)), LOAD, 1);
    display_codeB(6, m1, LOAD, 1);
    display_codeB(7, (h0 | (1 << 7)), LOAD, 1);
    display_codeB(8, h1, LOAD, 1);

    display_char(c, LOAD, 0);
  }
}

ISR (INT0_vect) {
  if ((!(PIND >> PD4)) & 1) {
    count = false;
  } else if (((PIND >> PD4) & 1)) {
    count = true;
  }
}

ISR (PCINT2_vect) {
  if ((!(PIND >> PD5)) & 1) {
    if (c) {
      c--;
    } else {
      c = IMAGES_LEN-1;
    }

  } else if ((!(PIND >> PD7)) & 1) {
    if (c==(IMAGES_LEN-1)) {
      c = 0;
    } else {
      c++;
    }
  }
}

ISR (TIMER1_COMPA_vect) {
  if ((ms1<9) && count) {
    ms1++;
  } else if (count) {
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