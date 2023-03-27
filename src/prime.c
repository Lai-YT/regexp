#include "prime.h"

#include <math.h>
#include <stdbool.h>

bool is_prime(int n) {
  if (n < 2) {
    return false;
  }
  if (n < 4) {
    return true;
  }

  // fast pass the even number easily, and check odd numbers only
  if (n % 2 == 0) {
    return false;
  }
  const int square_root = floor(sqrt(n));
  for (int i = 3; i <= square_root; i += 2) {
    if (n % i == 0) {
      return false;
    }
  }
  return true;
}

int next_prime(int n) {
  if (n < 0) {
    return 2;
  }
  while (!is_prime(n)) {
    n++;
  }
  return n;
}
