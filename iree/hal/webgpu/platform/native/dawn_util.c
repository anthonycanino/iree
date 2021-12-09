// Copyright 2021 The IREE Authors
//
// Licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// DO NOT SUBMIT
#include <wgpu.h>  // wgpu-native implementation only

#include "iree/hal/webgpu/platform/webgpu.h"

//===----------------------------------------------------------------------===//
// Implementation compatibility layer
//===----------------------------------------------------------------------===//

#define WGPU_NATIVE_INSTANCE ((WGPUInstance)((uintptr_t)0xABADD00Du))

WGPUInstance wgpuCreateInstance(WGPUInstanceDescriptor const* descriptor) {
  // wgpu-native does not have instances and just uses globals for everything :(
  // We use a sentinel value here so that we can do null checks in places for
  // implementations that do use instances.
  return WGPU_NATIVE_INSTANCE;
}

void wgpuDeviceDestroy(WGPUDevice device) {
  // wgpu-native does not export this but does have a drop to deref.
  wgpuDeviceDrop(device);
}

void const* wgpuBufferGetConstMappedRange(WGPUBuffer buffer, size_t offset,
                                          size_t size) {
  // wgpu-native doesn't have this, for some reason.
  return wgpuBufferGetMappedRange(buffer, offset, size);
}

void wgpuCommandEncoderPopDebugGroup(WGPUCommandEncoder commandEncoder) {
  // No-op; wgpu-native does not export this symbol.
}

void wgpuCommandEncoderPushDebugGroup(WGPUCommandEncoder commandEncoder,
                                      char const* groupLabel) {
  // No-op; wgpu-native does not export this symbol.
}

void iree_wgpuBindGroupDrop(WGPUBindGroup bindGroup) {
  wgpuBindGroupDrop(bindGroup);
}

void iree_wgpuBindGroupLayoutDrop(WGPUBindGroupLayout bindGroupLayout) {
  wgpuBindGroupLayoutDrop(bindGroupLayout);
}

void iree_wgpuBufferDrop(WGPUBuffer buffer) { wgpuBufferDrop(buffer); }

void iree_wgpuCommandBufferDrop(WGPUCommandBuffer commandBuffer) {
  wgpuCommandBufferDrop(commandBuffer);
}

void iree_wgpuCommandEncoderDrop(WGPUCommandEncoder commandEncoder) {
  wgpuCommandEncoderDrop(commandEncoder);
}

void iree_wgpuComputePipelineDrop(WGPUComputePipeline computePipeline) {
  wgpuComputePipelineDrop(computePipeline);
}

void iree_wgpuPipelineLayoutDrop(WGPUPipelineLayout pipelineLayout) {
  wgpuPipelineLayoutDrop(pipelineLayout);
}

void iree_wgpuQuerySetDrop(WGPUQuerySet querySet) {
  wgpuQuerySetDrop(querySet);
}

void iree_wgpuShaderModuleDrop(WGPUShaderModule shaderModule) {
  wgpuShaderModuleDrop(shaderModule);
}

//===----------------------------------------------------------------------===//
// Platform abstraction layer
//===----------------------------------------------------------------------===//

iree_status_t iree_webgpu_queue_wait_idle(WGPUInstance instance,
                                          WGPUDevice device, WGPUQueue queue,
                                          iree_timeout_t timeout) {
  // TODO(benvanik): figure out if this does anything. WebGPU currently has a
  // pretty incomplete story around all of this :(
  // wgpuInstanceProcessEvents(instance);
  return iree_ok_status();
}
