#include <avr/io.h>
#define F_CPU 2000000L
#include <util/delay.h>

int main() {
  DDRB |= 1 << PB5;

  while (1) {
    PORTB |= 1 << PB5;
    _delay_ms(50);
    PORTB &= ~(1 <<PB5);
    _delay_ms(100);
    PORTB |= 1 << PB5;
    _delay_ms(50);
    PORTB &= ~(1 << PB5);
    _delay_ms(500);
  }
}
