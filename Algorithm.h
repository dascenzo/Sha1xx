#pragma once
#include "Definitions.h"

// 6.1 Method 1
namespace Algorithm {
  // Initialize context; call this before processing any message blocks
  void initContext(Context& context);

  // Processes block M(i) from the message, using the context
  void processMessageBlock(Context& context, const Block& M_i);
}
