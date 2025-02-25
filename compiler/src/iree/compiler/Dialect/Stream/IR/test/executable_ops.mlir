// RUN: iree-opt --split-input-file %s --verify-diagnostics | FileCheck %s

// CHECK-LABEL: stream.executable private @executable
stream.executable private @executable {
  // CHECK-NEXT: stream.executable.export public @dispatch
  stream.executable.export public @dispatch
  // CHECK-NEXT: builtin.module
  builtin.module {
    // CHECK-NEXT: func.func @dispatch(%arg0: !stream.binding, %arg1: !stream.binding, %arg2: index) {
    func.func @dispatch(%arg0: !stream.binding, %arg1: !stream.binding, %arg2: index) {
      %c0 = arith.constant 0 : index
      // CHECK-DAG: = stream.binding.subspan %arg0[%c0] : !stream.binding -> !flow.dispatch.tensor<readwrite:?x5x64xf32>{%arg2}
      %0 = stream.binding.subspan %arg0[%c0] : !stream.binding -> !flow.dispatch.tensor<readwrite:?x5x64xf32>{%arg2}
      // CHECK-DAG: = stream.binding.subspan %arg1[%c0] : !stream.binding -> !flow.dispatch.tensor<readonly:?x5x4xf32>{%arg2}
      %1 = stream.binding.subspan %arg1[%c0] : !stream.binding -> !flow.dispatch.tensor<readonly:?x5x4xf32>{%arg2}
      // CHECK: return
      return
    }
  }
}

// -----

// CHECK-LABEL: stream.executable private @executable_with_workgroup_count
stream.executable private @executable_with_workgroup_count {
  // CHECK-NEXT: stream.executable.export public @dispatch
  stream.executable.export public @dispatch
      // CHECK-SAME: workgroups(%arg0: index, %arg1: index, %arg2: index) -> (index, index, index) {
      workgroups(%arg0: index, %arg1: index, %arg2: index) -> (index, index, index) {
        // CHECK-NEXT: stream.return %arg0, %arg1, %arg2 : index, index, index
        stream.return %arg0, %arg1, %arg2 : index, index, index
      }
  // CHECK: builtin.module
  builtin.module {
    // CHECK-NEXT: func.func @dispatch
    func.func @dispatch() {
      // CHECK: return
      return
    }
  }
}

// -----

stream.executable private @bad_workgroup_result_count {
  // expected-error @+1 {{workgroup count region must return the XYZ dimension counts}}
  stream.executable.export public @dispatch workgroups(%arg0: index, %arg1: index) -> (index, index) {
    stream.return %arg0, %arg1 : index, index
  }
  builtin.module  {
    func.func @dispatch() {
      return
    }
  }
}

// -----

stream.executable private @bad_workgroup_result_types {
  // expected-error @+1 {{workgroup count region must return the XYZ dimension counts}}
  stream.executable.export public @dispatch workgroups(%arg0: index, %arg1: f32) -> (index, f32, index) {
    stream.return %arg0, %arg1, %arg0 : index, f32, index
  }
  builtin.module  {
    func.func @dispatch() {
      return
    }
  }
}
