# Copyright 2021 The IREE Authors
#
# Licensed under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

include(CMakeParseArguments)

# iree_trace_runner_test()
#
# Creates a test using a specified trace-runner program for the specified
# replay trace.
#
# Parameters:
#   NAME: Name of the target
#   SRC: mlir source file to be compiled to an IREE module.
#   TARGET_BACKEND: target backend to compile for.
#   DRIVER: driver to run the module with.
#   COMPILER_FLAGS: additional flags to pass to the compiler. Bytecode
#       translation and backend flags are passed automatically.
#   RUNNER_ARGS: additional args to pass to the trace-runner program. The driver
#       and input file flags are passed automatically.
#   LABELS: Additional labels to apply to the test. The package path and
#       "driver=${DRIVER}" are added automatically.
#   TRACE_RUNNER: trace-runner program to run.
#   TRACE: trace file input to the trace-runner program.
#   MODULE_FILE_NAME: specifies the absolute path to the filename to use for the
#       generated IREE module (.vmfb). Mandatory, unlike in iree_check_test,
#       because trace files (.yaml) reference a specific module file path.
#   TARGET_CPU_FEATURES: If specified, a string passed as argument to
#       --iree-llvm-target-cpu-features.
function(iree_trace_runner_test)
  if(NOT IREE_BUILD_TESTS)
    return()
  endif()

  # See comment in iree_check_test about this condition.
  if(NOT IREE_BUILD_COMPILER AND NOT CMAKE_CROSSCOMPILING)
    return()
  endif()

  cmake_parse_arguments(
    _RULE
    ""
    "NAME;SRC;TRACE;TARGET_BACKEND;DRIVER;TRACE_RUNNER;MODULE_FILE_NAME"
    "COMPILER_FLAGS;RUNNER_ARGS;LABELS;TARGET_CPU_FEATURES"
    ${ARGN}
  )

  iree_package_name(_PACKAGE_NAME)
  set(_NAME "${_PACKAGE_NAME}_${_RULE_NAME}")

  set(_MODULE_NAME "${_RULE_NAME}_module")

  iree_bytecode_module_for_iree_check_test_and_friends(
    MODULE_NAME
      "${_MODULE_NAME}"
    MODULE_FILE_NAME
      "${_RULE_MODULE_FILE_NAME}"
    SRC
      "${_RULE_SRC}"
    TARGET_BACKEND
      "${_RULE_TARGET_BACKEND}"
    FLAGS
      ${_RULE_COMPILER_FLAGS}
    TARGET_CPU_FEATURES
      ${_RULE_TARGET_CPU_FEATURES}
  )

  # iree_bytecode_module does not define a target, only a custom command.
  # We need to create a target that depends on the command to ensure the
  # module gets built.
  # TODO(b/146898896): Do this in iree_bytecode_module and avoid having to
  # reach into the internals.
  set(_MODULE_TARGET_NAME "${_NAME}_module")
  add_custom_target(
    "${_MODULE_TARGET_NAME}"
     DEPENDS
       "${_RULE_MODULE_FILE_NAME}"
  )

  # A target specifically for the test. We could combine this with the above,
  # but we want that one to get pulled into iree_bytecode_module.
  add_custom_target("${_NAME}" ALL)
  add_dependencies(
    "${_NAME}"
    "${_MODULE_TARGET_NAME}"
    "${_RULE_TRACE_RUNNER}"
  )

  add_dependencies(iree-test-deps "${_NAME}")

  iree_native_test(
    NAME
      "${_RULE_NAME}"
    DRIVER
      "${_RULE_DRIVER}"
    SRC
      "${_RULE_TRACE_RUNNER}"
    TEST_INPUT_FILE_ARG
      ${_RULE_TRACE}
    DATA
      ${_MODULE_FILE_NAME}
    ARGS
      ${_RULE_RUNNER_ARGS}
    LABELS
      ${_RULE_LABELS}
      ${_RULE_TARGET_CPU_FEATURES}
  )
endfunction()

# iree_single_backend_generated_trace_runner_test()
#
# Variant of iree_trace_runner_test where instead of specifying
# a source file (and possibly a trace file and module path), one passes a
# generator program.
#
# Parameters:
#   NAME: Name of the target
#   GENERATOR: Program (at the moment, must be Python3) to run to generate the
#       source file (and possibly a trace file and module path). It will be
#       invoked with the following standard flags, in addition to GENERATOR_ARGS:
#         --output_code=${CMAKE_CURRENT_BINARY_DIR}/name.mlir
#         --output_trace=${CMAKE_CURRENT_BINARY_DIR}/name.yaml
#         --module_path=${CMAKE_CURRENT_BINARY_DIR}/name.vmfb
#       and if TARGET_CPU_FEATURES is not empty:
#         --requirements=${TARGET_CPU_FEATURES}
#   GENERATOR_ARGS: additional args to pass to the generator program.
#   TARGET_BACKEND: target backend to compile for.
#   DRIVER: driver to run the module with.
#   COMPILER_FLAGS: additional flags to pass to the compiler. Bytecode
#       translation and backend flags are passed automatically.
#   RUNNER_ARGS: additional args to pass to the trace-runner program. The driver
#       and input file flags are passed automatically.
#   LABELS: Additional labels to apply to the test. The package path and
#       "driver=${DRIVER}" are added automatically.
#   TRACE_RUNNER: trace-runner program to run.
#   TARGET_CPU_FEATURES: If specified, a string passed as argument to
#       --iree-llvm-target-cpu-features.
function(iree_single_backend_generated_trace_runner_test)
  if(NOT IREE_BUILD_TESTS)
    return()
  endif()

  # Copied from iree_check_test. Refer to the comment there.
  if(NOT IREE_BUILD_COMPILER AND NOT CMAKE_CROSSCOMPILING)
    return()
  endif()

  # Traces are YAML files and we assume that PyYAML is required. See the
  # warning that is emitted in aggregate in the main CMakeLists.txt if this
  # is not true.
  if(NOT IREE_PYYAML_FOUND)
    return()
  endif()

  cmake_parse_arguments(
    _RULE
    ""
    "NAME;GENERATOR;TARGET_BACKEND;DRIVER;TRACE_RUNNER"
    "GENERATOR_ARGS;COMPILER_FLAGS;RUNNER_ARGS;LABELS;TARGET_CPU_FEATURES"
    ${ARGN}
  )

  # Omit tests for which the specified driver or target backend is not enabled.
  # This overlaps with directory exclusions and other filtering mechanisms.
  string(TOUPPER ${_RULE_DRIVER} _UPPERCASE_DRIVER)
  string(REPLACE "-" "_" _NORMALIZED_DRIVER ${_UPPERCASE_DRIVER})
  if(NOT DEFINED IREE_HAL_DRIVER_${_NORMALIZED_DRIVER})
    message(SEND_ERROR "Unknown driver '${_RULE_DRIVER}'. Check IREE_HAL_DRIVER_* options.")
  endif()
  if(NOT IREE_HAL_DRIVER_${_NORMALIZED_DRIVER})
    return()
  endif()
  string(TOUPPER ${_RULE_TARGET_BACKEND} _UPPERCASE_TARGET_BACKEND)
  string(REPLACE "-" "_" _NORMALIZED_TARGET_BACKEND ${_UPPERCASE_TARGET_BACKEND})
  if(NOT DEFINED IREE_TARGET_BACKEND_${_NORMALIZED_TARGET_BACKEND})
    message(SEND_ERROR "Unknown backend '${_RULE_TARGET_BACKEND}'. Check IREE_TARGET_BACKEND_* options.")
  endif()
  if(DEFINED IREE_HOST_BINARY_ROOT)
    # If we're not building the host tools from source under this configuration,
    # such as when cross compiling, then we can't easily check for which
    # compiler target backends are enabled. Just assume all are enabled and only
    # rely on the runtime HAL driver check above for filtering.
  else()
    # We are building the host tools, so check enabled compiler target backends.
    if(NOT IREE_TARGET_BACKEND_${_NORMALIZED_TARGET_BACKEND})
      return()
    endif()
  endif()

  iree_package_name(_PACKAGE_NAME)
  set(_NAME "${_PACKAGE_NAME}_${_RULE_NAME}")

  set(_SRC "${CMAKE_CURRENT_BINARY_DIR}/${_RULE_NAME}.mlir")

  set(_GENERATOR_OUTPUT "${_SRC}")
  set(_TRACE "${CMAKE_CURRENT_BINARY_DIR}/${_RULE_NAME}.yaml")
  set(_MODULE_FILE_NAME "${CMAKE_CURRENT_BINARY_DIR}/${_RULE_NAME}.vmfb")
  list(APPEND _GENERATOR_STANDARD_FLAGS "--output_code=${_SRC}")
  list(APPEND _GENERATOR_STANDARD_FLAGS "--output_trace=${_TRACE}")
  list(APPEND _GENERATOR_STANDARD_FLAGS "--module_path=${_MODULE_FILE_NAME}")
  if(_RULE_TARGET_CPU_FEATURES)
    list(APPEND _GENERATOR_STANDARD_FLAGS "--requirements=${_RULE_TARGET_CPU_FEATURES}")
  endif()
  list(APPEND _GENERATOR_OUTPUT "${_TRACE}")

  add_custom_command(
    COMMAND
      "${Python3_EXECUTABLE}"
      "${CMAKE_CURRENT_SOURCE_DIR}/${_RULE_GENERATOR}"
      ${_GENERATOR_STANDARD_FLAGS}
      ${_RULE_GENERATOR_ARGS}
    OUTPUT
      ${_GENERATOR_OUTPUT}
    DEPENDS
      ${_RULE_GENERATOR}
  )

  add_custom_target(
    "${_NAME}_generated_files"
    DEPENDS
      ${_GENERATOR_OUTPUT}
  )

  iree_trace_runner_test(
    NAME
      "${_RULE_NAME}"
    SRC
      "${_SRC}"
    TRACE
      "${_TRACE}"
    TRACE_RUNNER
      "${_RULE_TRACE_RUNNER}"
    MODULE_FILE_NAME
      "${_MODULE_FILE_NAME}"
    TARGET_BACKEND
      ${_RULE_TARGET_BACKEND}
    DRIVER
      ${_RULE_DRIVER}
    COMPILER_FLAGS
      ${_RULE_COMPILER_FLAGS}
    RUNNER_ARGS
      ${_RULE_RUNNER_ARGS}
    LABELS
      ${_RULE_LABELS}
    TARGET_CPU_FEATURES
      ${_RULE_TARGET_CPU_FEATURES}
  )

  # Note we are relying on the fact that the target created by
  # iree_trace_runner_test is _NAME, even though we passed _RULE_NAME to it,
  # i.e. we are relying on the prefixing to be identical.
  add_dependencies("${_NAME}" "${_NAME}_generated_files")
endfunction()


# iree_generated_trace_runner_test()
#
# Creates a set of iree_single_backend_generated_trace_runner_test's differing
# by target backend and driver.
#
# Mirrors the bzl rule of the same name.
#
# One test is generated per source and backend/driver pair.
# Parameters:
#   NAME: Name of the target
#   GENERATOR: Program (at the moment, must be Python3) to run to generate the
#       source file (and possibly a trace file and module path). It will be
#       invoked with the following standard flags, in addition to GENERATOR_ARGS:
#         --output_code=${CMAKE_CURRENT_BINARY_DIR}/name.mlir
#         --output_trace=${CMAKE_CURRENT_BINARY_DIR}/name.yaml
#         --module_path=${CMAKE_CURRENT_BINARY_DIR}/name.vmfb
#   GENERATOR_ARGS: additional args to pass to the generator program.
#   TARGET_BACKENDS: backends to compile the module for. These form pairs with
#       the DRIVERS argument (due to cmake limitations they are separate list
#       arguments). The lengths must exactly match. If no backends or drivers are
#       specified, a test will be generated for every supported pair.
#   DRIVERS: drivers to run the module with. These form pairs with the
#       TARGET_BACKENDS argument (due to cmake limitations they are separate list
#       arguments). The lengths must exactly match. If no backends or drivers are
#       specified, a test will be generated for every supported pair.
#   COMPILER_FLAGS: additional flags to pass to the compiler. Bytecode
#       translation and backend flags are passed automatically.
#   RUNNER_ARGS: additional args to pass to the trace-runner program. The driver
#       and input file flags are passed automatically.
#   LABELS: Additional labels to apply to the test. The package path and
#       "driver=${DRIVER}" are added automatically.
#   TRACE_RUNNER: trace-runner program to run.
#   TARGET_CPU_FEATURES_VARIANTS: list of target cpu features variants. Only used
#       for drivers that vary based on the target CPU features. For each list
#       element, a separate test is created, with the list element passed as
#       argument to --iree-llvm-target-cpu-features. The special value "default"
#       is interpreted as no --iree-llvm-target-cpu-features flag to work around
#       corner cases with empty entries in CMake lists.
function(iree_generated_trace_runner_test)
  if(NOT IREE_BUILD_TESTS)
    return()
  endif()

  cmake_parse_arguments(
    _RULE
    ""
    "NAME;GENERATOR;TRACE_RUNNER"
    "TARGET_BACKENDS;DRIVERS;GENERATOR_ARGS;COMPILER_FLAGS;RUNNER_ARGS;LABELS;TARGET_CPU_FEATURES_VARIANTS"
    ${ARGN}
  )

  if(NOT DEFINED _RULE_TARGET_BACKENDS AND NOT DEFINED _RULE_DRIVERS)
    set(_RULE_TARGET_BACKENDS "vmvx" "vulkan-spirv" "dylib-llvm-aot")
    set(_RULE_DRIVERS "vmvx" "vulkan" "dylib")
  endif()

  list(LENGTH _RULE_TARGET_BACKENDS _TARGET_BACKEND_COUNT)
  list(LENGTH _RULE_DRIVERS _DRIVER_COUNT)

  if(NOT _TARGET_BACKEND_COUNT EQUAL _DRIVER_COUNT)
    message(SEND_ERROR
        "TARGET_BACKENDS count ${_TARGET_BACKEND_COUNT} does not match DRIVERS count ${_DRIVER_COUNT}")
  endif()

  math(EXPR _MAX_INDEX "${_TARGET_BACKEND_COUNT} - 1")
  foreach(_INDEX RANGE "${_MAX_INDEX}")
    list(GET _RULE_TARGET_BACKENDS ${_INDEX} _TARGET_BACKEND)
    list(GET _RULE_DRIVERS ${_INDEX} _DRIVER)
    if(_TARGET_BACKEND STREQUAL "dylib-llvm-aot" AND _RULE_TARGET_CPU_FEATURES_VARIANTS)
      set(_TARGET_CPU_FEATURES_VARIANTS "${_RULE_TARGET_CPU_FEATURES_VARIANTS}")
    else()
      set(_TARGET_CPU_FEATURES_VARIANTS "default")
    endif()
    foreach(_TARGET_CPU_FEATURES_LIST_ELEM IN LISTS _TARGET_CPU_FEATURES_VARIANTS)
      process_target_cpu_features("${_TARGET_CPU_FEATURES_LIST_ELEM}" _ENABLED _TARGET_CPU_FEATURES _TARGET_CPU_FEATURES_SUFFIX _TARGET_PASS_OPTIONS)
      string(REPLACE "#pass_options_variant#" "${_TARGET_PASS_OPTIONS}" _PROCESSED_COMPILER_FLAGS "${_RULE_COMPILER_FLAGS}")
      if(NOT _ENABLED)
        # The current entry is disabled on the target CPU architecture.
        continue()
      endif()
      iree_single_backend_generated_trace_runner_test(
        NAME
          "${_RULE_NAME}_${_TARGET_BACKEND}_${_DRIVER}${_TARGET_CPU_FEATURES_SUFFIX}"
        GENERATOR
          ${_RULE_GENERATOR}
        GENERATOR_ARGS
          ${_RULE_GENERATOR_ARGS}
        TRACE_RUNNER
          ${_RULE_TRACE_RUNNER}
        TARGET_BACKEND
          ${_TARGET_BACKEND}
        DRIVER
          ${_DRIVER}
        COMPILER_FLAGS
          ${_PROCESSED_COMPILER_FLAGS}
        RUNNER_ARGS
          ${_RULE_RUNNER_ARGS}
        LABELS
          ${_RULE_LABELS}
        TARGET_CPU_FEATURES
          ${_TARGET_CPU_FEATURES}
      )
    endforeach()
  endforeach()
endfunction()
