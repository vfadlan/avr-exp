#include <anim.h>

void draw_pattern(uint8_t pattern, int delay_ms) {
  sr_write(pattern);
  _delay_ms(delay_ms);
}

void binary_counter() {
  for (int i=0; i<256; i++) {
    draw_pattern(i, 150); 
  }
}

void stack_to_end() {
  uint8_t pattern = 0;
  uint8_t carry = 0;
  for (int i=7; i>=0; i--) {
    for (int j=0; j<i; j++) {
      pattern = (1 << j);
      draw_pattern((pattern | carry), 150);
    }
    carry |= pattern;
  }
}

void bounce_left_right() {
  for (int i=1; i<7; i++) {
    if (i==1) {
      shift_bit(1);
    } else {
      shift_bit(0);
    }
    latch();
    _delay_ms(200);
  }
  for (int i=7; i>=0; i--) { 
    sr_write((1 << i));
    _delay_ms(200);
  }
}
