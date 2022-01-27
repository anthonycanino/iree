#ifndef IREE_CSHARP_BASE_H_
#define IREE_CSHARP_BASE_H_

#include "bindings/csharp/iree/runtime/def.h"
#include "bindings/csharp/iree/runtime/util.h"

#include "iree/base/api.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct {
  iree_allocator_t *allocator;
} cs_iree_allocator_t;

CSHARP_API_EXPORTED cs_iree_allocator_t cs_iree_allocator_system(void);

CSHARP_API_EXPORTED cs_iree_allocator_t cs_iree_allocator_null(void);

CSHARP_API_EXPORTED iree_byte_span_t cs_iree_make_byte_span(void* data, iree_host_size_t data_length);

CSHARP_API_EXPORTED iree_const_byte_span_t cs_iree_make_const_byte_span(const void* data, iree_host_size_t data_length);


#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif // IREE_CSHARP_BASE_H_