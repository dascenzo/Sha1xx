#include "Definitions.h"

// Validate system
static_assert(CHAR_BIT == 8, "8 bit byte required");
static_assert(-1 == ~0,      "Two's complement required");
