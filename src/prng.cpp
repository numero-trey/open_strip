// Based on http://www.firstpr.com.au/dsp/rand31/rand31-park-miller-carta.cc.txt
#include "prng.hpp"

long unsigned int PRNG::rand() {
  long unsigned int hi, lo;
  // Multiply the high and low 2 bytes by the magic constant
  lo = kConst * (seed_val & 0xFFFF);
  hi = kConst * (seed_val >> 16);
  // Recombine
  lo += (hi & 0x7FFF) << 16;
  lo += hi >> 15;
  // Limit to 31 bitz
  if (lo > 0x7FFFFFFF) lo -= 0x7FFFFFFF;
  // Update seed and return vlue
  return ( seed_val = (long)lo );
}

void PRNG::seed(long unsigned int seedin) {
    if (seedin == 0) seedin = 1;
    seed_val = seedin;
}
