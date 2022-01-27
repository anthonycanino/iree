#ifndef IREE_CSHARP_VM_H_
#define IREE_CSHARP_VM_H_

#include "bindings/csharp/iree/runtime/def.h"
#include "bindings/csharp/iree/runtime/util.h"
#include "bindings/csharp/iree/runtime/base.h"

#include "iree/vm/api.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct {
  iree_vm_instance_t *storage;
} cs_iree_vm_instance_t;

typedef struct {
  iree_vm_module_t *storage;
} cs_iree_vm_module_t;

typedef struct {
  iree_vm_context_t *storage;
} cs_iree_vm_context_t;

typedef struct {
  iree_vm_function_t *storage;
} cs_iree_vm_function_t;

typedef struct {
  iree_vm_list_t *storage;
} cs_iree_vm_list_t;

typedef struct {
  iree_vm_ref_t *storage;
} cs_iree_vm_ref_t;


CSHARP_API_EXPORTED iree_status_t cs_iree_vm_instance_create(
    cs_iree_allocator_t allocator, cs_iree_vm_instance_t* out_instance);

// Retains the given |instance| for the caller.
CSHARP_API_EXPORTED void cs_iree_vm_instance_retain(cs_iree_vm_instance_t instance);

// Releases the given |instance| from the caller.
CSHARP_API_EXPORTED void cs_iree_vm_instance_release(cs_iree_vm_instance_t instance);

CSHARP_API_EXPORTED iree_status_t cs_iree_vm_bytecode_module_create(
    iree_const_byte_span_t flatbuffer_data,
    cs_iree_allocator_t flatbuffer_allocator, cs_iree_allocator_t allocator,
    cs_iree_vm_module_t* out_module);

CSHARP_API_EXPORTED iree_status_t cs_iree_vm_context_create_with_modules(
    cs_iree_vm_instance_t instance, 
    cs_iree_vm_module_t** modules, iree_host_size_t module_count,
    cs_iree_allocator_t allocator, cs_iree_vm_context_t* out_context);

CSHARP_API_EXPORTED iree_status_t cs_iree_vm_context_resolve_function(
    cs_iree_vm_context_t context, iree_string_view_t full_name,
    cs_iree_vm_function_t out_function);

CSHARP_API_EXPORTED cs_iree_vm_function_t cs_iree_vm_function_make();

CSHARP_API_EXPORTED iree_status_t cs_iree_vm_list_create(
    const iree_vm_type_def_t* element_type, iree_host_size_t initial_capacity,
    cs_iree_allocator_t allocator, cs_iree_vm_list_t* out_list);

CSHARP_API_EXPORTED cs_iree_vm_ref_t cs_iree_vm_ref_make();

CSHARP_API_EXPORTED iree_status_t cs_iree_vm_list_push_ref_move(cs_iree_vm_list_t list, cs_iree_vm_ref_t ref);

CSHARP_API_EXPORTED iree_status_t cs_iree_vm_invoke(
    cs_iree_vm_context_t context, cs_iree_vm_function_t function,
    iree_vm_invocation_flags_t flags, const iree_vm_invocation_policy_t* policy,
    cs_iree_vm_list_t inputs, cs_iree_vm_list_t outputs,
    cs_iree_allocator_t allocator);



#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif // IREE_CSHARP_VM_H_