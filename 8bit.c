#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef uint8_t ufloat8;

struct float_t {
  union {
    struct {
      uint32_t mantissa:23;
      uint32_t exponent:8;
      uint32_t sign:1;
    } fval;
    float f;
  };
};

void dumpn(uint64_t val, int8_t bits) {
  int8_t i;
  printf("0b");
  for(i = bits - 1; i >= 0; i--) {
    printf("%u", !!(val & (1<<i)));
  }
  printf("\n");
}

ufloat8 float_to_ufloat8(float val) {
  struct float_t f;
  f.f = val;
  if(val < 0.1275) {
    return 0b0000000;
  }
  if(val > 15872) {
    return 0b1111111;
  }
//  printf("Exponent: %u, Mantissa: %u\n", f.fval.exponent, f.fval.mantissa);
//  dumpn(f.fval.exponent, 8);
//  dumpn(f.fval.mantissa, 23);
  return ((uint8_t)(((((int32_t)f.fval.exponent) - 127) + 2) & 0xf) << 4) | (uint8_t)((f.fval.mantissa >> 19) & 0xf);
}

float ufloat8_to_float(ufloat8 uf) {
  uint16_t mantissa = (uf & 0xf) | (1<<4);
  int8_t exponent = ((uf >> 4) & 0xf) - 2;
  if(exponent == 0) {
    return 1.0;
  }
  exponent -= 4;
  if(exponent >= 0) {
    return (float)(mantissa << exponent);
  } else {
    float f = mantissa;
    while(exponent++ < 0) {
      f /= 2;
    }
    return f;
  }
}

void dump8(uint8_t val) {
  dumpn(val, 8);
}

int main() {
  printf("float -> ufloat8\n");
  dump8(float_to_ufloat8(1));
  dump8(float_to_ufloat8(2));
  dump8(float_to_ufloat8(2.5));
  dump8(float_to_ufloat8(3));
  printf("ufloat8 -> float\n");
  printf("%f\n", ufloat8_to_float(float_to_ufloat8(1)));
  printf("%f\n", ufloat8_to_float(float_to_ufloat8(2)));
  printf("%f\n", ufloat8_to_float(float_to_ufloat8(2.5)));
  printf("%f\n", ufloat8_to_float(float_to_ufloat8(3)));
  printf("%f\n", ufloat8_to_float(float_to_ufloat8(0.25)));
  printf("%f\n", ufloat8_to_float(float_to_ufloat8(15872)));
  return 0;
}