# Concurrent `cat` on Raspberry

Demo and slides used in [FOSDEM'25][], [DECOMPILED'25][], and [OH@ASPLOS'25][].
You can find the slides in Markdown format in `talks` directory.

[FOSDEM'25]:
https://fosdem.org/2025/schedule/event/fosdem-2025-6757-multicore-concurrency-algorithms-performance-correctness/
[DECOMPILED'25]: https://decompiled.de/schedule
[OH@ASPLOS'25]: https://openharmonyos.org/tutorial/

In this demo, we construct `ccat`, a simple concurrent `cat` program that, as
the original `cat` program, reads a file from the filesystem and writes its
contents to `stdout`.  However, `ccat` just contains enough features to allow
us to show the issues with weak memory consistency; it cannot open multiple
files, it does not read `stdin`.

The program consists of a reader thread, a mediator thread, and a writer thread
connected by ring buffers.  The reader opens a file (we will use an image of
Monalisa from Wikipedia), reads the file page-by-page, cuts each page in small
chunks, and enqueues the pointer to each chunk into the `used` ringbuffer.  The
mediator thread dequeues chunks from `used` and simply enqueues them again into
the `ready` ringbuffer. Finally, the writer thread dequeues the chunks from
`ready` and writes them their content to `stdout`, one-by-one, in FIFO order.
After that, the writer thread enqueues the chunks into the `free` ringbuffer,
which is used by the reader to (re)allocate new chunks.

To show a failure, we calculate the checksum of the input image and the output
image. When they differ we can also visualize both images to look for visible
differences; often the differences are obvious.

### Setup

To compile the program `ccat`, you'll need `clang` or `gcc` and `make`. To view
the image files, open them with your favorite image viewer.

We provide a script that repeatedly calls `./ccat assets/monalisa.jpg >
output.jpg`, compares the checksum of both files (computed with `md5sum`), and
aborts in case the checksums differ.  If your system has ImageMagick's `convert`
and [viu](https://github.com/atanunq/viu) in the executable path, the script
will show the mismatching figures side-by-side.

### Compiling

To compile `ccat`, call `make`.  You can test the program with with `./ccat
README.md`.  We will use the `assets/monalisa.jpg` as a running example. The
Monalisa file is a resized version of [this file][] from Wikipedia.

[this file]:
https://upload.wikimedia.org/wikipedia/commons/e/ec/Mona_Lisa%2C_by_Leonardo_da_Vinci%2C_from_C2RMF_retouched.jpg

Now, you can run `scripts/run.sh assets/monalisa.jpg`. In a few seconds, a
different pair of figures should appear.

## Issue 1: Ring buffer does not support multiple threads

Initially, `ccat` is using a version of the ring buffer that is implemented for
single threads. One can inspect the file `ringbuf.h` and try to fix it. On every
fix iteration, do not forget to recompile `ccat` and run the script again.

One possible outcome of "fixing" `ringbuf.h` might be what is in
`ringbuf_spsc_plain.h`.  You can change the include path inside `ccat.c` to use
this file instead of `ringbuf.h`. This implementation should work on x86
machines, and the script will terminate after a number of iterations without
reporting errors.

## Issue 2: Ring buffer does not work with `-O3`

Running our script, you most likely won't see any issues. Now, change the
`Makefile` to use `-O3`. This time, when running the script, you are very likely
to observe the program hanging. Try running `ccat` directly on some file, eg
`./ccat README.md`

The reason is that the compiler does not know the head and tail variables in
the ring buffer are going to be accessed by other threads. At this point, many
compiler might find possible optimizations that when applied cause the
concurrent program to hang.

One possible (non-)solution is to use `volatile` annotation on the head and tail
variables. See `ringbuf_spsc_volatile.h`. Nevertheless, this is not recommended.
As we will see in the next issue, the correct solution is to use atomic
variables.

## Issue 3: Ring buffer does not work on weak memory

If you have access to some machine with weak memory consistency such as a
Raspberry Pi 4, you can try running `ccat` with `ringbuf_spsc_plain.h`.

> Note: These issues should also be reproducible on several Arm-based
smartphones as well as on Apple M* processors.

When running on such system, `ccat` might corrupt `monalisa.jpg` in similar ways
as in the first experimet. The reason is that the CPU can optimize the execution
by reordering memory accesses, practically reverting the fixes introduced in
software when going from `ringbuf.h` to `ringbuf_spsc_plain.h`.

To limit the hardware optimizations (as well as compiler optimization) that may
reorder memory accesses of synchronization, we must use atomic operations on
every racy access and possibly memory barriers. The file `ringbuf_spsc.h`
contains the same implementation but now using atomic operations from
[libvsync](https://github.com/open-s4c/libvsync) on every racy access.  Each
atomic operation has the strongest memory ordering (seq_cst) by default, which
disables all relevant hardware optimizations.

## Issue 4: The barriers are too strong

The resulting code uses the strongest barriers on every racy access. How to
optimize this code (relaxing barriers), without breaking its correctness?

You can try running `bench.sc` and `bench.opt` to compare the performance of
the queues on your hardware.

## Verifying the code

Checkout our [vsyncer][] project to perform this optimization automatically
while verifying the code correctness.

To try yourself, install [Dartagnan][] and start by verifying `verify-spsc.c`:

```
source verify/env.sh
dartagnan -cat vmm verify/ringbuf_spsc-check.c
```

[vsyncer]: https://github.com/open-s4c/vsyncer
[Dartagnan]: https://github.com/hernanponcedeleon/Dat3M
