#ifndef IREE_CSHARP_HAL_H_
#define IREE_CSHARP_HAL_H_

#include "bindings/csharp/iree/runtime/def.h"
#include "bindings/csharp/iree/runtime/base.h"
#include "bindings/csharp/iree/runtime/vm.h"
#include "bindings/csharp/iree/runtime/util.h"

#include "iree/base/attributes.h"
#include "iree/base/status.h"

#include "iree/hal/api.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct {
  iree_hal_device_t *storage;
} cs_iree_hal_device_t;

typedef struct {
  iree_hal_buffer_t *storage;
} cs_iree_hal_buffer_t;

typedef struct {
  iree_hal_buffer_view_t *storage;
} cs_iree_hal_buffer_view_t;

typedef struct {
  iree_hal_allocator_t *storage;
} cs_iree_hal_allocator_t;



CSHARP_API_EXPORTED iree_status_t cs_iree_hal_module_register_types(void);

CSHARP_API_EXPORTED cs_iree_hal_allocator_t cs_iree_hal_device_allocator(cs_iree_hal_device_t device);

CSHARP_API_EXPORTED iree_status_t
cs_iree_hal_module_create(cs_iree_hal_device_t device, cs_iree_allocator_t allocator, cs_iree_vm_module_t* out_module);

CSHARP_API_EXPORTED iree_status_t cs_iree_hal_buffer_view_allocate_buffer(
    cs_iree_hal_allocator_t allocator, const iree_hal_dim_t* shape,
    iree_host_size_t shape_rank, iree_hal_element_type_t element_type,
    iree_hal_encoding_type_t encoding_type, iree_hal_memory_type_t memory_type,
    iree_hal_buffer_usage_t allowed_usage, iree_const_byte_span_t initial_data,
    cs_iree_hal_buffer_view_t* out_buffer_view);

// VM Ref stuff

CSHARP_API_EXPORTED void cs_iree_hal_buffer_view_move_ref(cs_iree_hal_buffer_view_t view, cs_iree_vm_ref_t out_ref);

CSHARP_API_EXPORTED cs_iree_hal_buffer_view_t 
cs_iree_vm_list_get_ref_deref_hal_buffer_view(cs_iree_vm_list_t list, iree_host_size_t i);

CSHARP_API_EXPORTED cs_iree_hal_buffer_t cs_iree_hal_buffer_view_buffer(cs_iree_hal_buffer_view_t buffer_view);

CSHARP_API_EXPORTED iree_status_t cs_iree_hal_buffer_read_data(
    cs_iree_hal_buffer_t source_buffer, iree_device_size_t source_offset,
    void* target_buffer, iree_device_size_t data_length); 


CSHARP_API_EXPORTED iree_status_t cs_create_vmvx_device(cs_iree_allocator_t host_allocator, cs_iree_hal_device_t *out_device);

CSHARP_API_EXPORTED iree_status_t cs_create_vulkan_device(cs_iree_allocator_t host_allocator, cs_iree_hal_device_t *out_device);



#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif // IREE_CSHARP_HAL_H_