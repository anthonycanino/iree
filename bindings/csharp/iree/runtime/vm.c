#include "bindings/csharp/iree/runtime/vm.h"

#include "iree/vm/bytecode_module.h"
#include "iree/vm/context.h"

iree_status_t cs_iree_vm_instance_create(
    cs_iree_allocator_t allocator, cs_iree_vm_instance_t* out_instance) {
  return iree_vm_instance_create(*allocator.allocator, &out_instance->storage);
}

void cs_iree_vm_instance_retain(cs_iree_vm_instance_t instance) {
  iree_vm_instance_retain((iree_vm_instance_t*) instance.storage);
}

void cs_iree_vm_instance_release(cs_iree_vm_instance_t instance) {
  iree_vm_instance_release((iree_vm_instance_t*) instance.storage);
}

iree_status_t cs_iree_vm_bytecode_module_create(iree_const_byte_span_t flatbuffer_data,
    cs_iree_allocator_t flatbuffer_allocator, cs_iree_allocator_t allocator,
    cs_iree_vm_module_t* out_module) {
  iree_allocator_t flatbuffer_allocator_c = *flatbuffer_allocator.allocator;
  iree_allocator_t allocator_c = *allocator.allocator;
  iree_vm_module_t **out_module_c = &out_module->storage;
  return iree_vm_bytecode_module_create(flatbuffer_data, flatbuffer_allocator_c, allocator_c, out_module_c);
}

iree_status_t cs_iree_vm_context_create_with_modules(cs_iree_vm_instance_t instance, 
    cs_iree_vm_module_t** modules, iree_host_size_t module_count,
    cs_iree_allocator_t allocator, cs_iree_vm_context_t* out_context) {
  iree_vm_instance_t *instance_c = instance.storage;
  iree_vm_context_t **out_context_c = &out_context->storage;

  // Unwrapping the array
  iree_vm_module_t **modules_c = (iree_vm_module_t**) malloc(sizeof(iree_vm_module_t*) * module_count);
  for (iree_host_size_t i = 0; i < module_count; i++) {
    modules_c[i] = modules[i]->storage;
  }
  
  iree_allocator_t allocator_c = *allocator.allocator;
  iree_status_t stat = iree_vm_context_create_with_modules(
    instance_c, IREE_VM_CONTEXT_FLAG_NONE, modules_c, module_count, allocator_c, out_context_c);
  free(modules_c);
  return stat;
}

iree_status_t cs_iree_vm_context_resolve_function(
    cs_iree_vm_context_t context, iree_string_view_t full_name, cs_iree_vm_function_t out_function) {
  iree_vm_context_t* context_c = context.storage;
  iree_vm_function_t* function_c = out_function.storage;
  return iree_vm_context_resolve_function(context_c, full_name, function_c);
}

cs_iree_vm_function_t cs_iree_vm_function_make() {
  cs_iree_vm_function_t val;
  val.storage = malloc(sizeof(iree_vm_function_t));
  return val;
}

iree_status_t cs_iree_vm_list_create(
    const iree_vm_type_def_t* element_type, iree_host_size_t initial_capacity,
    cs_iree_allocator_t allocator, cs_iree_vm_list_t* out_list) {
  iree_allocator_t allocator_c = *allocator.allocator;
  iree_vm_list_t **out_list_c = &out_list->storage;
  return iree_vm_list_create(element_type, initial_capacity, allocator_c, out_list_c);
}

cs_iree_vm_ref_t cs_iree_vm_ref_make() {
  cs_iree_vm_ref_t val;
  val.storage = malloc(sizeof(iree_vm_ref_t));
  return val;
}

iree_status_t cs_iree_vm_list_push_ref_move(cs_iree_vm_list_t list, cs_iree_vm_ref_t ref) {
  iree_vm_list_t *list_c = list.storage;
  iree_vm_ref_t *ref_c = ref.storage;
  return iree_vm_list_push_ref_move(list_c, ref_c);
}

iree_status_t cs_iree_vm_invoke(
    cs_iree_vm_context_t context, cs_iree_vm_function_t function,
    iree_vm_invocation_flags_t flags, const iree_vm_invocation_policy_t* policy,
    cs_iree_vm_list_t inputs, cs_iree_vm_list_t outputs,
    cs_iree_allocator_t allocator) {
  iree_vm_context_t *context_c = context.storage;    
  iree_vm_function_t function_c = *function.storage;
  iree_vm_list_t *inputs_c = inputs.storage;
  iree_vm_list_t *outputs_c = outputs.storage;
  iree_allocator_t allocator_c = *allocator.allocator;
  return iree_vm_invoke(context_c, function_c, flags, policy, inputs_c, outputs_c, allocator_c);
}






