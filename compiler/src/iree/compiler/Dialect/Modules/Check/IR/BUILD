# Copyright 2020 The IREE Authors
#
# Licensed under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

load("@llvm-project//mlir:tblgen.bzl", "td_library")
load("//build_tools/bazel:build_defs.oss.bzl", "iree_compiler_cc_library")
load("//build_tools/bazel:enforce_glob.bzl", "enforce_glob")
load("//build_tools/bazel:iree_tablegen.bzl", "iree_gentbl_cc_library", "iree_tablegen_doc")

package(
    default_visibility = ["//visibility:public"],
    features = ["layering_check"],
    licenses = ["notice"],  # Apache 2.0
)

td_library(
    name = "td_files",
    srcs = enforce_glob(
        ["CheckOps.td"],
        include = ["*.td"],
    ),
    deps = [
        "//compiler/src/iree/compiler/Dialect/HAL/IR:td_files",
        "//compiler/src/iree/compiler/Dialect/Util/IR:td_files",
        "@llvm-project//mlir:OpBaseTdFiles",
    ],
)

iree_compiler_cc_library(
    name = "IR",
    srcs = [
        "CheckOps.cpp",
        "CheckOps.cpp.inc",
    ],
    hdrs = [
        "CheckOps.h",
        "CheckOps.h.inc",
    ],
    deps = [
        ":check_ops_gen",
        "//compiler/src/iree/compiler/Dialect/HAL/IR",
        "@llvm-project//mlir:ArithmeticDialect",
        "@llvm-project//mlir:FuncDialect",
        "@llvm-project//mlir:IR",
    ],
)

iree_compiler_cc_library(
    name = "CheckDialect",
    srcs = [
        "CheckDialect.cpp",
    ],
    hdrs = [
        "CheckDialect.h",
    ],
    deps = [
        ":IR",
        ":check_ops_gen",
        "//compiler/src/iree/compiler/Dialect/HAL/Conversion",
        "//compiler/src/iree/compiler/Dialect/Modules/Check:check_imports",
        "//compiler/src/iree/compiler/Dialect/Modules/Check/Conversion",
        "//compiler/src/iree/compiler/Dialect/VM/Conversion",
        "@llvm-project//mlir:IR",
        "@llvm-project//mlir:Parser",
        "@llvm-project//mlir:Transforms",
    ],
)

iree_gentbl_cc_library(
    name = "check_ops_gen",
    tbl_outs = [
        (
            ["--gen-op-decls"],
            "CheckOps.h.inc",
        ),
        (
            ["--gen-op-defs"],
            "CheckOps.cpp.inc",
        ),
    ],
    tblgen = "@llvm-project//mlir:mlir-tblgen",
    td_file = "CheckOps.td",
    deps = [":td_files"],
)

iree_tablegen_doc(
    name = "CheckDialectDocGen",
    tbl_outs = [
        (
            [
                "--gen-dialect-doc",
                "--dialect=check",
            ],
            "CheckDialect.md",
        ),
    ],
    tblgen = "@llvm-project//mlir:mlir-tblgen",
    td_file = "CheckOps.td",
    deps = [":td_files"],
)
