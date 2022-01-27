#include "bindings/csharp/iree/runtime/hal.h"

#include "iree/modules/hal/module.h"

#include "iree/hal/vmvx/registration/driver_module.h"

iree_status_t cs_iree_hal_module_register_types(void) {
  return iree_hal_module_register_types();
}

cs_iree_hal_allocator_t
cs_iree_hal_device_allocator(cs_iree_hal_device_t device) {
  iree_hal_device_t *device_c = device.storage;
  cs_iree_hal_allocator_t ret;
  ret.storage = iree_hal_device_allocator(device_c);
  return ret;
}

iree_status_t
cs_iree_hal_module_create(cs_iree_hal_device_t device, cs_iree_allocator_t allocator, cs_iree_vm_module_t* out_module) {
  iree_hal_device_t *device_c = device.storage;
  iree_allocator_t allocator_c = *allocator.allocator;
  iree_vm_module_t **out_module_c = &out_module->storage;
  return iree_hal_module_create(device_c, allocator_c, out_module_c);
}

iree_status_t cs_create_vmvx_device(cs_iree_allocator_t host_allocator, cs_iree_hal_device_t *out_device) {
  iree_allocator_t host_allocator_c = *host_allocator.allocator;
  iree_hal_device_t **out_device_c = &out_device->storage;


  IREE_RETURN_IF_ERROR(iree_hal_vmvx_driver_module_register(
      iree_hal_driver_registry_default()));
  // Create the hal driver from the name.
  iree_hal_driver_t* driver = NULL;
  iree_string_view_t identifier = iree_make_cstring_view("vmvx");
  iree_status_t status = iree_hal_driver_registry_try_create_by_name(
      iree_hal_driver_registry_default(), identifier, host_allocator_c, &driver);

  if (iree_status_is_ok(status)) {
    status = iree_hal_driver_create_default_device(driver, host_allocator_c, out_device_c);
  }

  iree_hal_driver_release(driver);
  return iree_ok_status();
}

iree_status_t cs_iree_hal_buffer_view_allocate_buffer(
    cs_iree_hal_allocator_t allocator, const iree_hal_dim_t* shape,
    iree_host_size_t shape_rank, iree_hal_element_type_t element_type,
    iree_hal_encoding_type_t encoding_type, iree_hal_memory_type_t memory_type,
    iree_hal_buffer_usage_t allowed_usage, iree_const_byte_span_t initial_data,
    cs_iree_hal_buffer_view_t* out_buffer_view) {
  iree_hal_allocator_t *allocator_c = allocator.storage;
  iree_hal_buffer_view_t **out_buffer_view_c = &out_buffer_view->storage;
  return iree_hal_buffer_view_allocate_buffer(
    allocator_c, shape, shape_rank, element_type, encoding_type, 
    memory_type, allowed_usage, initial_data, out_buffer_view_c);
}

void cs_iree_hal_buffer_view_move_ref(cs_iree_hal_buffer_view_t view, cs_iree_vm_ref_t out_ref) {
  iree_hal_buffer_view_t *view_c = view.storage;
  *out_ref.storage = iree_hal_buffer_view_move_ref(view_c);
}

cs_iree_hal_buffer_view_t cs_iree_vm_list_get_ref_deref_hal_buffer_view(cs_iree_vm_list_t list, iree_host_size_t i) {
  iree_vm_list_t *list_c = list.storage;
  cs_iree_hal_buffer_view_t out_view;
  out_view.storage = iree_vm_list_get_ref_deref(list_c, i, iree_hal_buffer_view_get_descriptor());
  return out_view;
}

cs_iree_hal_buffer_t cs_iree_hal_buffer_view_buffer(cs_iree_hal_buffer_view_t buffer_view) {
  iree_hal_buffer_view_t *buffer_view_c = buffer_view.storage;
  cs_iree_hal_buffer_t out_buffer;
  out_buffer.storage = iree_hal_buffer_view_buffer(buffer_view_c);
  return out_buffer;
}

iree_status_t cs_iree_hal_buffer_read_data(
    cs_iree_hal_buffer_t source_buffer, iree_device_size_t source_offset,
    void* target_buffer, iree_device_size_t data_length) {
  iree_hal_buffer_t *source_buffer_c = source_buffer.storage;
  return iree_hal_buffer_read_data(source_buffer_c, source_offset, target_buffer, data_length);
}
