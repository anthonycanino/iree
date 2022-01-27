#include "bindings/csharp/iree/runtime/base.h"

cs_iree_allocator_t cs_iree_allocator_system(void) {
  cs_iree_allocator_t val;
  val.allocator = malloc(sizeof(iree_allocator_t));
  *((iree_allocator_t*) val.allocator) = iree_allocator_system();
  return val;
}

cs_iree_allocator_t cs_iree_allocator_null(void) {
  cs_iree_allocator_t val;
  val.allocator = malloc(sizeof(iree_allocator_t));
  *((iree_allocator_t*) val.allocator) = iree_allocator_null();
  return val;
}


iree_byte_span_t cs_iree_make_byte_span(
    void* data, iree_host_size_t data_length) {
  iree_byte_span_t v = {(uint8_t*)data, data_length};
  return v;
}

iree_const_byte_span_t cs_iree_make_const_byte_span(
    const void* data, iree_host_size_t data_length) {
  iree_const_byte_span_t v = {(const uint8_t*)data, data_length};
  return v;
}
