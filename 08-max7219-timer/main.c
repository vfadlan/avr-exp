#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <ctype.h>
#include <max7219.h>
#include <stdbool.h>

#define F_CPU 16000000UL
#define DEBOUNCE_DELAY 3
#define SCK PB5
#define MOSI PB3
#define LOAD PB2

volatile uint32_t centiseconds = 0;

volatile bool count = true;
volatile bool update_display = true;
volatile int c = 0;

extern uint8_t EMOJI_NUM;
extern uint64_t IMAGES[];
extern uint8_t IMAGES_LEN;

volatile uint32_t pcint2_last_debounce = 0;
volatile bool pcint2_delayed = false;
volatile uint32_t int0_last_debounce = 0;
volatile bool int0_delayed = false;

int main() {
  uint8_t cs, h, m, s;
  uint32_t remaining_seconds, now;
  TCCR1B = (1 << 3) | 0b010; // ctc mode prescale 8, 2MHz
  OCR1A = 19999; // 10ms each match
  TIMSK1 = (1 << OCIE1A); // Timer Interrupt 1 Mask
  PORTD = (1 << PD4) | (1 << PD6) | (1 << PD7);
  EICRA = (1 << ISC00);
  EIMSK = 1;
  PCICR |= (1 << PCIE2);
  PCMSK2 |= (1 << PCINT22) | (1 << PCINT23);


  SPI_init(SCK, MOSI, LOAD);
  display_init(0xFF, 0x07, 7, LOAD, 0, 2);
  display_init(0x00, 0x07, 7, LOAD, 1, 2);

  sei();
  while (1) {
    if (update_display) {
      if (centiseconds >= 8640000) {
        centiseconds = 0;
      }
      
      cs = centiseconds % 100;
      remaining_seconds = centiseconds/100;
      h = remaining_seconds / 3600;
      m = remaining_seconds / 60 % 60;
      s = remaining_seconds % 60; // v

      display_codeB(1, ((cs % 10) | (1 << 7)), LOAD, 0);
      display_codeB(2, (cs / 10), LOAD, 0);

      display_codeB(3, ((s % 10) | (1 << 7)), LOAD, 0);
      display_codeB(4, (s / 10), LOAD, 0);

      display_codeB(5, ((m % 10) | (1 << 7)), LOAD, 0);
      display_codeB(6, (m / 10), LOAD, 0);

      display_codeB(7, ((h % 10) | (1 << 7)), LOAD, 0);
      display_codeB(8, (h / 10), LOAD, 0);

      display_char(c, LOAD, 1);

      update_display = false;
    }

    now = centiseconds;
    if (int0_delayed) {
      int0_delayed = false;
      if ((!(PIND >> PD4)) & 1) {
        count = false;
      } else if ((PIND >> PD4) & 1) {
        count = true;
      }
    }
    if (pcint2_delayed) {
      pcint2_delayed = false;
      if ((!(PIND >> PD7)) & 1) {
        if (c) {
          c--;
        } else {
          c = IMAGES_LEN-1;
        }

      } else if ((!(PIND >> PD6)) & 1) {
        if (c==(IMAGES_LEN-1)) {
          c = 0;
        } else {
          c++;
        }
      }
    }
  }
}

ISR (INT0_vect) {
  uint32_t now = centiseconds;
  if ((now - int0_last_debounce) >= DEBOUNCE_DELAY) {
    int0_last_debounce = now;
    int0_delayed = true;
  }
}

ISR (PCINT2_vect) {
  uint32_t now = centiseconds;
  if ((now - pcint2_last_debounce) >= DEBOUNCE_DELAY) {
    pcint2_last_debounce = now;
    pcint2_delayed = true;
  }
}

ISR (TIMER1_COMPA_vect) {
  if (count) {
    centiseconds++;
    update_display = true;
  }
}

/*
todo:
off-load int isr: only set event flag and debounce
off-load pcint isr: only set event flag and debounce
*/