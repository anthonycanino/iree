// Copyright 2022 The IREE Authors
//
// Licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef IREE_DIALECTS_DIALECT_LINALGEXT_TRANSFORMOPS_LINALGEXTTRANSFORMOPS_H
#define IREE_DIALECTS_DIALECT_LINALGEXT_TRANSFORMOPS_LINALGEXTTRANSFORMOPS_H

#include "iree-dialects/Dialect/LinalgExt/IR/LinalgExtDialect.h"
#include "iree-dialects/Dialect/LinalgTransform/TransformOpTraits.h"
#include "mlir/Dialect/PDL/IR/PDLTypes.h"
#include "mlir/Dialect/Transform/IR/TransformDialect.h"
#include "mlir/Dialect/Transform/IR/TransformInterfaces.h"
#include "mlir/IR/OpDefinition.h"

namespace mlir {
namespace scf {
class ForOp;
} // namespace scf

namespace iree_compiler {
namespace IREE {
namespace LinalgExt {
class InParallelOp;
class TileOp;
} // namespace LinalgExt
} // namespace IREE
} // namespace iree_compiler
} // namespace mlir

#define GET_OP_CLASSES
#include "iree-dialects/Dialect/LinalgExt/TransformOps/LinalgExtTransformOps.h.inc"

namespace mlir {
namespace iree_compiler {
namespace IREE {
namespace LinalgExt {
class LinalgExtTransformOpsExtension
    : public transform::TransformDialectExtension<
          LinalgExtTransformOpsExtension, IREELinalgExtDialect> {
public:
  LinalgExtTransformOpsExtension();
};
} // namespace LinalgExt
} // namespace IREE
} // namespace iree_compiler
} // namespace mlir

#endif // IREE_DIALECTS_DIALECT_LINALGEXT_TRANSFORMOPS_LINALGEXTTRANSFORMOPS_H
