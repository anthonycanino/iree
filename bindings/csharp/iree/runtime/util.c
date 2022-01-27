#include "bindings/csharp/iree/runtime/util.h"

#include <stdio.h>

int cs_iree_status_is_ok(iree_status_t value) {
  return iree_status_is_ok(value);
}

int cs_iree_status_code(iree_status_t value) {
  return iree_status_code(value);
}

void cs_iree_status_print(iree_status_t value) {
  iree_status_fprint(stdout, value);
}