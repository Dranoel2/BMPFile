#include "pattern.h"

#define WIDTH 11
#define HEIGHT 8

DEFINE_PATTERN {
  { TRANS, TRANS, PURPLE, TRANS, TRANS, TRANS, TRANS, TRANS, PURPLE, TRANS, TRANS },
  { TRANS, TRANS, TRANS, PURPLE, TRANS, TRANS, TRANS, PURPLE, TRANS, TRANS, TRANS },
  { TRANS, TRANS, PURPLE, PURPLE, PURPLE, PURPLE, PURPLE, PURPLE, PURPLE, TRANS, TRANS },
  { TRANS, PURPLE, PURPLE, TRANS, PURPLE, PURPLE, PURPLE, TRANS, PURPLE, PURPLE, TRANS},
  { PURPLE, PURPLE, PURPLE, PURPLE, PURPLE, PURPLE, PURPLE, PURPLE, PURPLE, PURPLE, PURPLE },
  { PURPLE, TRANS, PURPLE, PURPLE, PURPLE, PURPLE, PURPLE, PURPLE, PURPLE, TRANS, PURPLE },
  { PURPLE, TRANS, PURPLE, TRANS, TRANS, TRANS, TRANS, TRANS, PURPLE, TRANS, PURPLE },
  { TRANS, TRANS, TRANS, PURPLE, PURPLE, TRANS, PURPLE, PURPLE, TRANS, TRANS, TRANS }
};