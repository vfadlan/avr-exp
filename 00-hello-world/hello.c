#include <avr/io.h>
#include <util/delay.h>

#define F_CPU 16000000UL

int main() {
  DDRB |= 1 << PB5;

  while (1) {
    PORTB |= 1 << PB5;
    _delay_ms(2500);
    PORTB &= ~(1 <<PB5);
    _delay_ms(3725);
    PORTB |= 1 << PB5;
    _delay_ms(2500);
    PORTB &= ~(1 << PB5);
    _delay_ms(6250);
  }
}
