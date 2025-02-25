// Copyright 2019 The IREE Authors
//
// Licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// See iree/base/api.h for documentation on the API conventions used.

#ifndef IREE_HAL_VULKAN_API_H_
#define IREE_HAL_VULKAN_API_H_

#include <stdint.h>

// clang-format off: must be included before all other headers.
#include "iree/hal/vulkan/vulkan_headers.h"
// clang-format on

#include <stdint.h>

#include "iree/base/api.h"
#include "iree/hal/api.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

//===----------------------------------------------------------------------===//
// iree_hal_vulkan_device_t extensibility util
//===----------------------------------------------------------------------===//

// TODO(benvanik): replace with feature list (easier to version).
// Bitfield that defines sets of Vulkan features.
enum iree_hal_vulkan_feature_bits_t {
  // Use VK_LAYER_KHRONOS_standard_validation to validate Vulkan API usage.
  // Has a significant performance penalty and is *not* a security mechanism.
  IREE_HAL_VULKAN_FEATURE_ENABLE_VALIDATION_LAYERS = 1u << 0,

  // Use VK_EXT_debug_utils, record markers, and log errors.
  IREE_HAL_VULKAN_FEATURE_ENABLE_DEBUG_UTILS = 1u << 1,

  // Enables tracing of command buffers when IREE tracing is enabled.
  // May take advantage of additional extensions for more accurate timing or
  // hardware-specific performance counters.
  //
  // NOTE: tracing has a non-trivial overhead and will skew the timing of
  // submissions and introduce false barriers between dispatches. Use this to
  // identify slow dispatches and refine from there; be wary of whole-program
  // tracing with this enabled.
  IREE_HAL_VULKAN_FEATURE_ENABLE_TRACING = 1u << 2,
};
typedef uint32_t iree_hal_vulkan_features_t;

// Describes the type of a set of Vulkan extensions.
typedef enum iree_hal_vulkan_extensibility_set_e {
  // A set of required instance layer names. These must all be enabled on
  // the VkInstance for IREE to function.
  IREE_HAL_VULKAN_EXTENSIBILITY_INSTANCE_LAYERS_REQUIRED = 0,

  // A set of optional instance layer names. If omitted fallbacks may be
  // used or debugging features may not be available.
  IREE_HAL_VULKAN_EXTENSIBILITY_INSTANCE_LAYERS_OPTIONAL,

  // A set of required instance extension names. These must all be enabled on
  // the VkInstance for IREE to function.
  IREE_HAL_VULKAN_EXTENSIBILITY_INSTANCE_EXTENSIONS_REQUIRED,

  // A set of optional instance extension names. If omitted fallbacks may be
  // used or debugging features may not be available.
  IREE_HAL_VULKAN_EXTENSIBILITY_INSTANCE_EXTENSIONS_OPTIONAL,

  // A set of required device extension names. These must all be enabled on
  // the VkDevice for IREE to function.
  IREE_HAL_VULKAN_EXTENSIBILITY_DEVICE_EXTENSIONS_REQUIRED,

  // A set of optional device extension names. If omitted fallbacks may be
  // used or debugging features may not be available.
  IREE_HAL_VULKAN_EXTENSIBILITY_DEVICE_EXTENSIONS_OPTIONAL,

  IREE_HAL_VULKAN_EXTENSIBILITY_SET_COUNT,  // used for sizing lookup tables
} iree_hal_vulkan_extensibility_set_t;

// Queries the names of the Vulkan layers and extensions used for a given set of
// IREE |requested_features|. All devices used by IREE must have the required
// layers and extensions as defined by these sets. Optional layers and
// extensions will be used when needed and otherwise have fallbacks for when
// they are not available.
//
// Instance extensions should be enabled on VkInstances passed to
// |iree_hal_vulkan_driver_create_using_instance| and device extensions should
// be enabled on VkDevices passed to |iree_hal_vulkan_driver_wrap_device|.
//
// |string_capacity| defines the number of elements available in
// |out_string_values| and |out_string_count| will be set with the actual number
// of strings returned. If |string_capacity| is too small then
// IREE_STATUS_OUT_OF_RANGE will be returned with the required capacity in
// |out_string_count|. To only query the required capacity then
// |out_string_values| may be passed as NULL.
//
// The returned strings originate from the _EXTENSION_NAME Vulkan macros
// (such as 'VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME') and have a
// lifetime matching whatever module they are defined in.
IREE_API_EXPORT iree_status_t iree_hal_vulkan_query_extensibility_set(
    iree_hal_vulkan_features_t requested_features,
    iree_hal_vulkan_extensibility_set_t set, iree_host_size_t string_capacity,
    const char** out_string_values, iree_host_size_t* out_string_count);

//===----------------------------------------------------------------------===//
// iree_hal_vulkan_syms_t
//===----------------------------------------------------------------------===//

typedef struct iree_hal_vulkan_syms_t iree_hal_vulkan_syms_t;

// Loads Vulkan functions by invoking |vkGetInstanceProcAddr|.
//
// |vkGetInstanceProcAddr| can be obtained in whatever way suites the calling
// application, such as via `dlsym` or `GetProcAddress` when dynamically
// loading Vulkan, or `reinterpret_cast<void*>(&vkGetInstanceProcAddr)` when
// statically linking Vulkan.
//
// |out_syms| must be released by the caller.
IREE_API_EXPORT iree_status_t iree_hal_vulkan_syms_create(
    void* vkGetInstanceProcAddr_fn, iree_allocator_t host_allocator,
    iree_hal_vulkan_syms_t** out_syms);

// Loads Vulkan functions from the Vulkan loader.
// This will look for a Vulkan loader on the system (like libvulkan.so) and
// dlsym the functions from that.
//
// |out_syms| must be released by the caller with iree_hal_vulkan_syms_release.
IREE_API_EXPORT iree_status_t iree_hal_vulkan_syms_create_from_system_loader(
    iree_allocator_t host_allocator, iree_hal_vulkan_syms_t** out_syms);

// Retains the given |syms| for the caller.
IREE_API_EXPORT void iree_hal_vulkan_syms_retain(iree_hal_vulkan_syms_t* syms);

// Releases the given |syms| from the caller.
IREE_API_EXPORT void iree_hal_vulkan_syms_release(iree_hal_vulkan_syms_t* syms);

//===----------------------------------------------------------------------===//
// iree_hal_vulkan_device_t
//===----------------------------------------------------------------------===//

// A set of queues within a specific queue family on a VkDevice.
typedef struct iree_hal_vulkan_queue_set_t {
  // The index of a particular queue family on a VkPhysicalDevice, as described
  // by vkGetPhysicalDeviceQueueFamilyProperties.
  uint32_t queue_family_index;

  // Bitfield of queue indices within the queue family at |queue_family_index|.
  uint64_t queue_indices;
} iree_hal_vulkan_queue_set_t;

// TODO(benvanik): replace with flag list (easier to version).
enum iree_hal_vulkan_device_flag_bits_t {
  IREE_HAL_VULKAN_DEVICE_FLAG_NONE = 0u,
};
typedef uint32_t iree_hal_vulkan_device_flags_t;

typedef struct iree_hal_vulkan_device_options_t {
  // Flags controlling device behavior.
  iree_hal_vulkan_device_flags_t flags;
} iree_hal_vulkan_device_options_t;

IREE_API_EXPORT void iree_hal_vulkan_device_options_initialize(
    iree_hal_vulkan_device_options_t* out_options);

// Creates a Vulkan HAL device that wraps an existing VkDevice.
//
// HAL devices created in this way may share Vulkan resources and synchronize
// within the same physical VkPhysicalDevice and logical VkDevice directly.
//
// |logical_device| is expected to have been created with all extensions
// returned by |iree_hal_vulkan_get_extensions| and
// IREE_HAL_VULKAN_DEVICE_REQUIRED using the features provided during driver
// creation.
//
// |instance_syms| must have at least the instance-specific functions resolved
// and device symbols will be queried from |logical_device| as needed.
//
// The device will schedule commands against the queues in
// |compute_queue_set| and (if set) |transfer_queue_set|.
//
// Applications may choose how these queues are created and selected in order
// to control how commands submitted by this device are prioritized and
// scheduled. For example, a low priority queue could be provided to one IREE
// device for background processing or a high priority queue could be provided
// for latency-sensitive processing.
//
// Dedicated compute queues (no graphics capabilities) are preferred within
// |compute_queue_set|, if they are available.
// Similarly, dedicated transfer queues (no compute or graphics) are preferred
// within |transfer_queue_set|.
// The queue sets can be the same.
//
// |out_device| must be released by the caller (see |iree_hal_device_release|).
IREE_API_EXPORT iree_status_t iree_hal_vulkan_wrap_device(
    iree_string_view_t identifier,
    const iree_hal_vulkan_device_options_t* options,
    const iree_hal_vulkan_syms_t* instance_syms, VkInstance instance,
    VkPhysicalDevice physical_device, VkDevice logical_device,
    const iree_hal_vulkan_queue_set_t* compute_queue_set,
    const iree_hal_vulkan_queue_set_t* transfer_queue_set,
    iree_allocator_t host_allocator, iree_hal_device_t** out_device);

//===----------------------------------------------------------------------===//
// iree_hal_vulkan_driver_t
//===----------------------------------------------------------------------===//

// Vulkan driver creation options.
typedef struct iree_hal_vulkan_driver_options_t {
  // Vulkan version that will be requested, e.g. `VK_API_VERSION_1_0`.
  // Driver creation will fail if the required version is not available.
  uint32_t api_version;

  // IREE features used to configure the VkInstance and VkDevices created using
  // it. These are used to populate the active Vulkan layers and extensions when
  // the instance and its devices are created.
  iree_hal_vulkan_features_t requested_features;

  // TODO(benvanik): remove this single setting - it would be nice instead to
  // pass a list to force device enumeration/matrix expansion or omit entirely
  // to have auto-discovered options based on capabilities. Right now this
  // forces all devices - even if from different vendors - to have the same
  // options.
  // Options to use for all devices created by the driver.
  iree_hal_vulkan_device_options_t device_options;

  // TODO(benvanik): change to something more canonically vulkan (like
  // VkPhysicalDeviceProperties::deviceID).
  // Index of the default Vulkan device to use within the list of available
  // devices. Devices are discovered via vkEnumeratePhysicalDevices then
  // considered "available" if compatible with the |requested_features|.
  int default_device_index;
} iree_hal_vulkan_driver_options_t;

IREE_API_EXPORT void iree_hal_vulkan_driver_options_initialize(
    iree_hal_vulkan_driver_options_t* out_options);

// Creates a Vulkan HAL driver that manages its own VkInstance.
//
// |out_driver| must be released by the caller (see |iree_hal_driver_release|).
IREE_API_EXPORT iree_status_t iree_hal_vulkan_driver_create(
    iree_string_view_t identifier,
    const iree_hal_vulkan_driver_options_t* options,
    iree_hal_vulkan_syms_t* syms, iree_allocator_t host_allocator,
    iree_hal_driver_t** out_driver);

// Creates a Vulkan HAL driver that shares an existing VkInstance.
//
// |instance| is expected to have been created with all extensions returned by
// the instance-specific |iree_hal_vulkan_query_extensibility_set| queries.
//
// |instance| must remain valid for the life of |out_driver| and |out_driver|
// itself must be released by the caller (see |iree_hal_driver_release|).
IREE_API_EXPORT iree_status_t iree_hal_vulkan_driver_create_using_instance(
    iree_string_view_t identifier,
    const iree_hal_vulkan_driver_options_t* options,
    iree_hal_vulkan_syms_t* instance_syms, VkInstance instance,
    iree_allocator_t host_allocator, iree_hal_driver_t** out_driver);

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  // IREE_HAL_VULKAN_API_H_
