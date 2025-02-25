# Copyright 2020 The IREE Authors
#
# Licensed under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

"""Macros for defining tests that run a module using iree-check-module."""

load("//build_tools/bazel:iree_bytecode_module.bzl", "iree_bytecode_module")
load("//build_tools/bazel:native_binary.bzl", "native_test")

ALL_TARGET_BACKENDS_AND_DRIVERS = [
    ("vmvx", "vmvx"),
    ("vulkan-spirv", "vulkan"),
    ("dylib-llvm-aot", "dylib"),
]

def iree_check_test(
        name,
        src,
        target_backend,
        driver = None,
        compiler_flags = [],
        runner_args = [],
        tags = [],
        target_cpu_features = None,
        timeout = None,
        **kwargs):
    """Creates an iree-check-module test for the specified source file.

    Args:
      name: name of the generated test.
      src: source mlir file containing the module.
      target_backend: target backend to compile for.
      driver: driver to run the module with. This can be omitted to test only
          compilation, but consider omiting the driver as a hacky abuse of the
          rule since compilation on its own not use iree-check-module.
      compiler_flags: additional flags to pass to the compiler. Bytecode translation and backend
          flags are passed automatically.
      runner_args: additional runner_args to pass to iree-check-module. The driver and input file
          are passed automatically.
      tags: additional tags to apply to the generated test. A tag "driver=DRIVER" is added
          automatically.
      target_cpu_features: currently unimplemented (must be empty), will eventually allow specifying target CPU features.
      timeout: timeout for the generated tests.
      **kwargs: any additional attributes to pass to the underlying native_test.
    """

    if target_cpu_features:
        fail("target_cpu_features must currently be empty")

    bytecode_module_name = name + "_bytecode_module"
    iree_bytecode_module(
        name = bytecode_module_name,
        src = src,
        flags = [
            "--iree-mlir-to-vm-bytecode-module",
            "--mlir-print-op-on-diagnostic=false",
            "--iree-hal-target-backends=%s" % target_backend,
        ] + compiler_flags,
        visibility = ["//visibility:private"],
    )

    if not driver:
        return

    native_test(
        name = name,
        args = [
            "--driver=%s" % driver,
            "$(location :%s)" % bytecode_module_name,
        ] + runner_args,
        data = [":%s" % bytecode_module_name],
        src = "//tools:iree-check-module",
        tags = tags + ["driver=%s" % driver],
        timeout = timeout,
        **kwargs
    )

def iree_check_single_backend_test_suite(
        name,
        srcs,
        target_backend,
        driver = None,
        compiler_flags = [],
        runner_args = [],
        tags = [],
        target_cpu_features = None,
        timeout = None,
        **kwargs):
    """Creates a test suite of iree-check-module tests for a single backend/driver pair.

    One test is generated per source file.

    Args:
      name: name of the generated test suite.
      srcs: source mlir files containing the module.
      target_backend: target backend to compile for.
      driver: driver to run the module with. This can be omitted to test only
          compilation, but consider omiting the driver as a hacky abuse of the
          rule since compilation on its own not use iree-check-module.
      compiler_flags: additional flags to pass to the compiler. Bytecode translation and backend
          flags are passed automatically.
      runner_args: additional runner_args to pass to the underlying iree-check-module tests. The
          driver and input file are passed automatically. To use different runner_args per test,
          create a separate suite or iree_check_test.
      target_cpu_features: currently unimplemented (must be empty), will eventually allow specifying target CPU features.
      tags: tags to apply to the generated tests. Note that as in standard test suites, manual
          is treated specially and will also apply to the test suite itself.
      timeout: timeout for the generated tests.
      **kwargs: any additional attributes to pass to the underlying tests and test suite.
    """

    # We haven't implemented this so far because we have been using target_cpu_features so far only
    # for aarch64 targets, for which we use the CMake build. To future people implementing this:
    # target_cpu_features should be a list, and here it should be joined into a comma-separated
    # string to be passed to --iree-llvm-target-cpu-features
    if target_cpu_features:
        fail("target_cpu_features must currently be empty")

    tests = []
    for src in srcs:
        # CUDA backend/driver not supported by Bazel build.
        if target_backend == "cuda" or driver == "cuda":
            continue
        test_name = "_".join([name, src])
        iree_check_test(
            name = test_name,
            src = src,
            target_backend = target_backend,
            driver = driver,
            compiler_flags = compiler_flags,
            runner_args = runner_args,
            tags = tags,
            timeout = timeout,
            **kwargs
        )
        tests.append(test_name)

    if not driver:
        return

    native.test_suite(
        name = name,
        tests = tests,
        # Note that only the manual tag really has any effect here. Others are
        # used for test suite filtering, but all tests are passed the same tags.
        tags = tags,
        # If there are kwargs that need to be passed here which only apply to
        # the generated tests and not to test_suite, they should be extracted
        # into separate named arguments.
        **kwargs
    )

def iree_check_test_suite(
        name,
        srcs,
        target_backends_and_drivers = ALL_TARGET_BACKENDS_AND_DRIVERS,
        compiler_flags = [],
        runner_args = [],
        tags = [],
        target_cpu_features_variants = [],
        **kwargs):
    """Creates a test suite of iree-check-module tests.

    One test is generated per source file and backend/driver.

    Args:
      name: name of the generated test suite.
      srcs: source mlir files containing the module.
      target_backends_and_drivers: backend/driver pairs to compile and run the module, respectively.
      compiler_flags: additional flags to pass to the compiler. Bytecode translation and backend
          flags are passed automatically.
      runner_args: additional runner_args to pass to the underlying iree-check-module tests. The
          driver and input file are passed automatically. To use different runner_args per test,
          create a separate suite or iree_check_test.
      tags: tags to apply to the generated tests. Note that as in standard test suites, manual
          is treated specially and will also apply to the test suite itself.
      target_cpu_features_variants: list of target cpu features variants. Currently unimplemented, so each
            entry must be either "default" or start with "aarch64:" so as Bazel builds are currently x86-only,
            we know that it is correct to ignore this.
      **kwargs: any additional attributes to pass to the underlying tests and test suite.
    """

    for target_cpu_features in target_cpu_features_variants:
        if not (target_cpu_features == "default" or target_cpu_features.startswith("aarch64:")):
            fail("Entry %s in target_cpu_features_variants: unimplemented" % target_cpu_features)

    # We could have complicated argument override logic for runner_args and such, or... the client
    # could just create a test suite. The latter seems simpler and more readable.
    tests = []
    for backend, driver in target_backends_and_drivers:
        # CUDA backend/driver not supported by Bazel build.
        if backend == "cuda" or driver == "cuda":
            continue
        suite_name = "_".join([name, backend, driver])
        iree_check_single_backend_test_suite(
            name = suite_name,
            srcs = srcs,
            driver = driver,
            target_backend = backend,
            compiler_flags = compiler_flags,
            runner_args = runner_args,
            tags = tags,
            **kwargs
        )
        tests.append(suite_name)
    native.test_suite(
        name = name,
        tests = tests,
        # Note that only the manual tag really has any effect here. Others are
        # used for test suite filtering, but all tests are passed the same tags.
        tags = tags,
        # If there are kwargs that need to be passed here which only apply to
        # the generated tests and not to test_suite, they should be extracted
        # into separate named arguments.
        **kwargs
    )
