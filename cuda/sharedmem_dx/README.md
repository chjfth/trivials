This program shows a wrong case using `extern` to declare a piece of GPU sharedmem.

In order to use a piece of sharedmem `gpu_sbin[256]` in a sharedmem_dx.cu, we cannot just 

* `extern` declare that array in a .h(libcode.h), 
* and use gpu_sbin[] from sharedmem_dx.cu's `__global__` function.

If we do that, the kernel function may fail with `cudaErrorIllegalAddress`, like this:

```
chj@GBCHJWIN10VHD [2023-09-28 22:36:02.44] d:\temp
> D:\barn\Win32\__Release\sharedmem_dx.exe
Hello, sharedmem_dx!
GPU: GeForce GTX 870M, CUDA ver: 8000
CUDA kernel call error(77): cudaErrorIllegalAddress
```

The problem may be hidden on other GPU model. For example, running it on RTX 3050 will not 
report error.

What's more, on GTX 870M, if we run with `sharedmem_dx.exe 64`, no error is reported either.

So it seems to show that:
* GTX 780M's sharedmem minimum size is 64 ints.
* RTX 3050's sharedmem minimum size is 256 ints (at least).
