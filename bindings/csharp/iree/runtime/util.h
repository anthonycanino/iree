#ifndef IREE_CSHARP_UTIL_H_
#define IREE_CSHARP_UTIL_H_

#include "bindings/csharp/iree/runtime/def.h"
#include "iree/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

CSHARP_API_EXPORTED int cs_iree_status_is_ok(iree_status_t value);

CSHARP_API_EXPORTED int cs_iree_status_code(iree_status_t value);

CSHARP_API_EXPORTED void cs_iree_status_print(iree_status_t value);

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus


#endif // IREE_CSHARP_UTIL_H_