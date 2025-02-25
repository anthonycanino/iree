# Dynamic Library CPU HAL Driver

IREE supports efficient model execution on CPU. IREE uses [LLVM][llvm] to
compile dense computation in the model into highly optimized CPU native
instruction streams, which are embedded in IREE's deployable format as dynamic
libraries (dylibs). IREE uses its own low-overhead minimal dynamic library
loader to load them and then schedule them with concrete workloads onto various
CPU cores.

!!! todo

    Add IREE's CPU support matrix: what architectures are supported; what
    architectures are well optimized; etc.

<!-- TODO(??): when to use CPU vs GPU vs other backends -->

## Get runtime and compiler

### Get IREE runtime with dylib HAL driver

You will need to get an IREE runtime that supports the dylib HAL driver
so it can execute the model on CPU via dynamic libraries containing native
CPU instructions.

<!-- TODO(??): vcpkg -->


#### Build runtime from source

Please make sure you have followed the [Getting started][get-started] page
to build IREE for your host platform and the
[Android cross-compilation][android-cc]
page if you are cross compiling for Android. The dylib HAL driver is compiled
in by default on all platforms.

<!-- TODO(??): a way to verify dylib is compiled in and supported -->

Ensure that the `IREE_HAL_DRIVER_DYLIB` CMake option is `ON` when configuring
for the target.

### Get compiler for CPU native instructions

<!-- TODO(??): vcpkg -->

#### Download as Python package

Python packages for various IREE functionalities are regularly published
to [PyPI][pypi]. See the [Python Bindings][python-bindings] page for more
details. The core `iree-compiler` package includes the LLVM-based CPU compiler:

``` shell
python -m pip install iree-compiler
```

!!! tip
    `iree-compile` is installed to your python module installation path. If you
    pip install with the user mode, it is under `${HOME}/.local/bin`, or
    `%APPDATA%Python` on Windows. You may want to include the path in your
    system's `PATH` environment variable.
    ``` shell
    export PATH=${HOME}/.local/bin:${PATH}
    ```

#### Build compiler from source

Please make sure you have followed the [Getting started][get-started] page
to build IREE for your host platform and the
[Android cross-compilation][android-cc]
page if you are cross compiling for Android. The dylib compiler backend is
compiled in by default on all platforms.

Ensure that the `IREE_TARGET_BACKEND_DYLIB_LLVM_AOT` CMake option is `ON` when
configuring for the host.

!!! tip
    `iree-compile` is under `iree-build/tools/` directory. You may want to
    include this path in your system's `PATH` environment variable.

## Compile and run the model

With the compiler and runtime for dynamic libraries, we can now compile a model
and run it on the CPU.

### Compile the model

IREE compilers transform a model into its final deployable format in many
sequential steps. A model authored with Python in an ML framework should use the
corresponding framework's import tool to convert into a format (i.e.,
[MLIR][mlir]) expected by main IREE compilers first.

Using MobileNet v2 as an example, you can download the SavedModel with trained
weights from [TensorFlow Hub][tf-hub-mobilenetv2] and convert it using IREE's
[TensorFlow importer][tf-import]. Then,

#### Compile using the command-line

Run the following command (assuming the path to `iree-compile` is in your
system's `PATH`):

``` shell hl_lines="3"
iree-compile \
    --iree-mlir-to-vm-bytecode-module \
    --iree-hal-target-backends=dylib-llvm-aot \
    iree_input.mlir -o mobilenet-dylib.vmfb
```

!!! todo

    Choose the suitable target triple for the current CPU

where `iree_input.mlir` is the model's initial MLIR representation generated by
IREE's TensorFlow importer.

### Run the model

#### Run using the command-line

In the build directory, run the following command:

``` shell hl_lines="2"
tools/iree-run-module \
    --driver=dylib \
    --module_file=mobilenet-dylib.vmfb \
    --entry_function=predict \
    --function_input="1x224x224x3xf32=0"
```

The above assumes the exported function in the model is named as `predict` and
it expects one 224x224 RGB image. We are feeding in an image with all 0 values
here for brevity, see `iree-run-module --help` for the format to specify
concrete values.


<!-- TODO(??): deployment options -->

<!-- TODO(??): measuring performance -->

<!-- TODO(??): troubleshooting -->

[android-cc]: ../building-from-source/android.md
[get-started]: ../building-from-source/getting-started.md
[llvm]: https://llvm.org/
[mlir]: https://mlir.llvm.org/
[pypi]: https://pypi.org/user/google-iree-pypi-deploy/
[python-bindings]: ../bindings/python.md
[tf-hub-mobilenetv2]: https://tfhub.dev/google/tf2-preview/mobilenet_v2/classification
[tf-import]: ../getting-started/tensorflow.md
[tflite-import]: ../getting-started/tensorflow-lite.md
