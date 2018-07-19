# fastrange: A fast alternative to the modulo reduction

A common operation in software is to take a machine word
and map it to an integer value in a range [0,p) as fairly as possible.
That is, you want that if all values of the machine word are
equally likely then, as much as possible, all integer values in
[0,p) are (almost) equally likely. This is common in hashing and probabilistic
algorithms.

The standard approach to this problem is the modulo reduction (``x % p``).
Though a modulo reduction works fine and has several nice properties,
it can be slow even on recent processors because it relies on an
integer division.

Thankfully, there is a faster way: we can replace the modulo by a multiplication
followed by a shift.

It has accelerated some operations in Google's Tensorflow by 10% to 20%
(https://github.com/tensorflow/tensorflow/commit/a47a300185026fe7829990def9113bf3a5109fed).

Further reading : http://lemire.me/blog/2016/06/27/a-fast-alternative-to-the-modulo-reduction/


See also:

* Daniel Lemire, [Fast Random Integer Generation in an Interval](https://arxiv.org/abs/1805.10941), ACM Transactions on Modeling and Computer Simulation (to appear)


This library provides a single portable header file that you should
be able to just drop in your C/C++ projects. The API is simple:


```C
#include "fastrange"

// given a value word, produces an integer in [0,p) without division
uint32_t fastrange32(uint32_t word, uint32_t p);
uint64_t fastrange64(uint64_t word, uint64_t p);
size_t fastrangesize(size_t word, size_t p);
int fastrangeint(int word, int p);
```
## Pre-conditions

For this code to give the desired result, the provided words should span the whole range (e.g., all 32-bit integers). The C ``rand`` function does not meet this requirement. If you must use the ``rand``  function, wrap it like so:

```
uint32_t get32rand() {
    return (rand() ^ (rand() << 15) ^ (rand() << 30));
}

uint64_t get64rand() {
    return (((uint64_t)get32rand()) << 32) | get32rand();
}
```


However, the underlying idea is general: we only require that the word values span an interval of the form ``[0,1<<L)``. It suffices then to do `` ( x * range ) >> L `` to get a fair map from ``[0,1<<L)`` to ``[0,range)``. For example, if your word values span the interval [0,65536), then you could simply do ``( x * range ) >> 16``.


## Unbiased range functions

To generate an unbiased random number in a range, you can use an extension of this approach:

```C
uint32_t random_bounded(uint32_t range) {
  uint64_t random32bit =  random32(); //32-bit random number 
  multiresult = random32bit * range;
  leftover = (uint32_t) multiresult;
  if(leftover < range ) {
      threshold = -range % range ;
      while (leftover < threshold) {
            random32bit =  random32();
            multiresult = random32bit * range;
            leftover = (uint32_t) multiresult;
      }
   }
  return multiresult >> 32;
}
```

See http://lemire.me/blog/2016/06/30/fast-random-shuffling/
