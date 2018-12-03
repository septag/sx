#include <Foundation/Foundation.h>
#import  <CoreFoundation/CoreFoundation.h>

int apple__num_cores()
{
    @autoreleasepool { return (unsigned int)[[NSProcessInfo processInfo] processorCount]; }
}

