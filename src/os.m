#include <Foundation/Foundation.h>
#import  <CoreFoundation/CoreFoundation.h>

int apple__numcores() {
    @autoreleasepool { return (unsigned int)[[NSProcessInfo processInfo] processorCount]; }
}

