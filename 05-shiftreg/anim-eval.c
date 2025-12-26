// emulate the shift register to preview animation
// routines: sr_write, shift_bit, latch, other animations
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>

uint8_t shift_register_buffer = 0;
uint8_t output_latch = 0;

void latch() {
    output_latch = shift_register_buffer;
    
    printf("\r[");
    for (int i = 7; i >= 0; i--) {
        if (output_latch & (1 << i)) printf(" ● ");
        else printf(" ○ ");
    }
    printf("]");
    fflush(stdout);
}

void shift_bit(bool bit) {
    shift_register_buffer <<= 1;
    if (bit) {
        shift_register_buffer |= 0x01;
    }
}

void sr_write(uint8_t data) {
  for (int i=0; i<8; i++) {
    shift_bit(data & 0x80);
    data <<= 1;
  }
  latch();
}

void draw_pattern(uint8_t pattern, int delay_ms) {
  sr_write(pattern);
  usleep(400 * 1000);
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
    for (int j=0; j<=i; j++) {
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
    usleep(300 * 1000);
  }
  for (int i=7; i>=0; i--) { 
    sr_write((1 << i));
    usleep(300 * 1000);
  }
}

void left_sign(int len) {
  for (int i=0; i<8+len; i++) {
    if (i<len) {
      shift_bit(1);
    } else {
      shift_bit(0);
    }
    latch();
    usleep(300 * 1000);
  }
}

int main() {
  printf("Shift Register Emulator Starting...\n");
    
  while(1) {
    // bounce_left_right();
    // sleep(1);
    // stack_to_end();
    // for (int i=0; i<8; i++) {
    //   shift_bit(0);
    //   latch();
    //   usleep(250 * 1000);
    // }
    // sleep(1);
    left_sign(3);
    sleep(1);
    left_sign(4);
    sleep(1);
  }

  return 0;
}
