// Based on http://www.firstpr.com.au/dsp/rand31/rand31-park-miller-carta.cc.txt
#ifndef OPEN_STRIP_INCLUDE_PRNG_HPP_
#define OPEN_STRIP_INCLUDE_PRNG_HPP_

class PRNG {
  long unsigned int seed_val;
  const unsigned int kConst = 16807;

public:
  PRNG() {seed_val = 1;}

  // Set the seed
  void seed(long unsigned int);

  // Get a random value
  long unsigned int rand();
};

#endif //ndef OPEN_STRIP_INCLUDE_PRNG_HPP_
