#include <assert.h>
#include "../../fastrange.h"

// this just checks that we can compile successfully

int main() {
  for(uint32_t x = 0; x < 1000000; ++x) assert(fastrange32(x,5) < 5);
  for(uint64_t x = 0; x < 1000000; ++x) assert(fastrange64(x,5) < 5);
}
