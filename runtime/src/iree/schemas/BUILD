# Copyright 2019 The IREE Authors
#
# Licensed under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

load("//build_tools/bazel:build_defs.oss.bzl", "iree_build_test")
load("//build_tools/bazel:iree_flatcc.bzl", "iree_flatbuffer_c_library")

package(
    default_visibility = ["//visibility:public"],
    features = ["layering_check"],
    licenses = ["notice"],  # Apache 2.0
)

FLATCC_ARGS = [
    "--reader",
    "--builder",
    "--verifier",
    "--json",
]

iree_flatbuffer_c_library(
    name = "bytecode_module_def_c_fbs",
    srcs = ["bytecode_module_def.fbs"],
    flatcc_args = FLATCC_ARGS,
)

iree_flatbuffer_c_library(
    name = "cuda_executable_def_c_fbs",
    srcs = ["cuda_executable_def.fbs"],
    flatcc_args = FLATCC_ARGS,
)

iree_flatbuffer_c_library(
    name = "rocm_executable_def_c_fbs",
    srcs = ["rocm_executable_def.fbs"],
    flatcc_args = FLATCC_ARGS,
)

iree_flatbuffer_c_library(
    name = "metal_executable_def_c_fbs",
    srcs = ["metal_executable_def.fbs"],
    flatcc_args = FLATCC_ARGS,
)

iree_flatbuffer_c_library(
    name = "spirv_executable_def_c_fbs",
    srcs = ["spirv_executable_def.fbs"],
    flatcc_args = FLATCC_ARGS,
)

iree_flatbuffer_c_library(
    name = "wgsl_executable_def_c_fbs",
    srcs = ["wgsl_executable_def.fbs"],
    flatcc_args = FLATCC_ARGS,
)

iree_build_test(
    name = "schema_build_test",
    targets = [
        ":bytecode_module_def_c_fbs",
        ":metal_executable_def_c_fbs",
        ":spirv_executable_def_c_fbs",
        ":wgsl_executable_def_c_fbs",
    ],
)
