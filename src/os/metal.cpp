// This file must be compiled exactly once.
// It instantiates the Metal-cpp implementation headers.
// Every other .cpp file includes the Metal-cpp headers WITHOUT these defines.

#define NS_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION

#include "metal.h"
