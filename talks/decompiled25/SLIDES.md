---
title: A CONCURRENT cat ON A RASPBERRY PI
sub_title: DECOMPILED'25
author: Diogo Behrens <diogo.behrens@huawei.com>
#theme:
#    name: db7-2
---
# A CONCURRENT `cat` ON A RASPBERRY PI

## GOAL

- Demonstrate how **weak memory consistency can
break** a concurrent program.

- Raised your interest on tooling support.

- Motivate you to **come talk with me** later.

## WEAK MEMORY WHAT?

- **Weak memory consistency** is a hardware optimization technique
- Heavily used on Arm (including Apple M*) and RISC-V
- Cores aggressively reorder (non-dependent) memory accesses
- **Good for sequential** code, **dangerous for concurrent** code

## DISCLAIMER

This presentation was given 5 days ago in FOSDEM'25.

<!-- end_slide -->

# MAKING `ccat`

We want to create `ccat`, a simple concurrent `cat` program with
the three active threads: a reader, a mediator, and a writer.

The threads form a chain passing memory chunks from input
and, finally, writing to `stdout` their content, *one-by-one, in
FIFO order*.

<!-- column_layout: [12,8] -->
<!-- column: 0 -->

## READER THREAD
- reads from file page-sized buffers
- breaks the pages in smaller *chunks*
- pushes chunks into `used` ringbuffer

## MEDIATOR THREAD
- consumes chunks from `used`
- could *in theory* massage the data
- pushes  chunks into `ready` rbuf

## WRITER THREAD
- consumes chunks from `ready`
- prints them on the screen
- pushes chunks back to `free` rbuf


<!-- column: 1 -->
```
╭──────╮   ┌────────┐
│ FILE │══>│ Reader │<──┐
╰──────╯   └────────┘   │
               │        │
              used      │
               │        │
               ∨        │
           ┌────────┐   │
           │Mediator│   │
           └────────┘   │
               │        │
             ready      │
               │        │
               ∨        │
╭──────╮   ┌────────┐   │
│STDOUT│<══│ Writer │──free
╰──────╯   └────────┘
```

<!-- column_layout: [2,18,2] -->
<!-- column: 1 -->

>

**CHALLENGE**: construct a FIFO ringbuffer that supports concurrent access of single producer and single consumer, ie, a SPSC queue.

<!-- end_slide -->
<!-- column_layout: [1,14,1] -->
<!-- column: 1 -->

# THE BIG PICTURE -- THREAD LOOPS

## CHUNKS

```c
struct chunk {
    char payload[CHUNK_SIZE];
    size_t len;
};
```

## READER LOOP

```c
do { /* read large portion of data */
    r = fread(&data, 1, PAGE_SIZE, fp);

    for (size_t i = 0; i < r;) {
        /* get a free chunk */
        while (ringbuf_deq(&free_chunks, &c) != OK);

        /* calculate data length and copy */
        c->len = (r - i > CHUNK_SIZE) ?
            CHUNK_SIZE: r - i;
        memcpy(&c->payload, data + i, c->len);
        i += c->len;

        /* pass ownership of c to mediator */
        while (ringbuf_enq(&used_chunks, c) != OK);
    }
} while (r != 0);
```

<!-- end_slide -->
<!-- column_layout: [1,14,1] -->
<!-- column: 1 -->

# THE RINGBUFFER!

## THREE OPTIONS
1. use big lock, but that allows *no concurrency*
2. *carefully think* and implement a concurrent algorithm
3. hope for the best and *just use try it*!

<!-- pause -->

## LET'S GO WITH OPTION 3!

```c
typedef struct {
    void** buf;
    unsigned int head, tail, size;
} ringbuf_t;

int ringbuf_enq(ringbuf_t* q, void* v) {
    if (q->tail - q->head == q->size)
        return FULL;
    unsigned int tail = q->tail++;
    q->buf[tail % q->size] = v;
    return OK;
}
int ringbuf_deq(ringbuf_t* q, void** v) {
    if (q->tail - q->head == 0)
        return EMPTY;
    unsigned int head = q->head++;
    *v = q->buf[head % q->size];
    return OK;
}
```

<!-- pause -->

## WILL THIS WORK?

<!-- end_slide -->
<!-- column_layout: [1,16,1] -->
<!-- column: 1 -->

# FIRST TRY: `./ccat assets/monalisa.jpg | viu -`

Let's use our `ccat` implemenation to pipe into `viu` an image of Monalisa:

<!-- column_layout: [1,8,8,1] -->
<!-- column: 1 -->

## EXPECTED RESULT

![image:width:80%](../../assets/monalisa.jpg)

<!-- column: 2 -->
## ACTUAL RESULT

![image:width:80%](../../assets/broken.jpg)
<!-- column_layout: [1,14,1] -->
<!-- column: 1 -->

- `viu`: https://github.com/atanunq/viu
- https://upload.wikimedia.org/wikipedia/commons/e/ec/Mona_Lisa%2C_by_Leonardo_da_Vinci%2C_from_C2RMF_retouched.jpg

<!-- end_slide -->
## WHAT'S THE PROBLEM?

--> Let's figure out.

<!-- end_slide -->

# ISSUE 1: NO MULTITHREAD SUPPORT YET
>

- `ccat` is using single-threaded ringbuffer
- Let's inspect `ringbuf.h` and try to fix it!
- What can go wrong with 2 threads?

>
If it works, *don't forget optimizing* it. (see `Makefile`)

<!-- end_slide -->

# ISSUE 2: RINGBUFFER FAILS WITH `-O3`?

Try running this in `gdb`:

```
gdb --args ./ccat assets/monalisa.jpg
```

Use `layout asm` to see where the threads are stuck. You should
see a thread at a `jmp` instruction.

Let's **DECOMPILE**:

```asm
│   0x400d4c <mediator+108> test   %r10,%r10
│   0x400d4f <mediator+111> je     0x400d62 <mediator+130>
│   0x400d51 <mediator+113> cmp    %r11d,%ecx
│   0x400d54 <mediator+116> jne    0x400d17 <mediator+55>
+-->0x400d56 <mediator+118> jmp    0x400d56 <mediator+118>
│   0x400d58 <mediator+120> nopl   0x0(%rax,%rax,1)
```


<!-- column_layout: [1,1] -->
<!-- column: 0 -->

## KEYWORD **volatile**

> Tells the compiler not to "cache" the value because a device (**or** another thread) might change it meanwhile.

<!-- column: 1 -->
## KEYWORK **atomic**

> We **should not use** volatile on shared memory accesses, but instead **atomic variables**. Still, people insist...

<!-- end_slide -->
<!-- column_layout: [1,6,1] -->
<!-- column: 1 -->
>

>

>

>

>

## WILL IT WORK ON A RASPBERRY PI?

<!-- end_slide -->

# ISSUE 3: RINGBUFFER FAILS ON WEAK MEMORY

On weak memory, `ccat` might corrupt `monalisa.jpg` in similar ways as in the first experiment.

<!-- column_layout: [1,1] -->
<!-- column: 0 -->

## REASON
> The CPU might optimize the execution by reordering memory
accesses, practically reverting the fixes we introduced.

<!-- column: 1 -->
## SOLUTION

> On **every racy access**, we must use **atomic operations** and **memory
barriers** to disable hardware (and compiler) optimizations.

<!-- reset_layout -->

We will atomics from https://github.com/open-s4c/libvsync.

By default, atomic operations have the strongest memory barrier (seq_cst),
which disables all optimizations, but **can be slow**.

**Note**: These issues are reproducible on several Arm-based smartphones as well as Apple M* and RISC-V processors.

<!-- end_slide -->

# ISSUE 4: ARE BARRIERS IN RINGBUFFER TOO STRONG?

>

Let's build a microbenchmark with 2 threads!

<!-- column_layout: [1,1] -->
<!-- column: 0 -->
### WRITER THREAD

- picks chunk from `free` rbuf
- writes some bytes in the chunk's payload
- pushes chunk into `used` rbuf

### READER THREAD

- consumes chunk from `used` rbuf
- does nothing with it
- pushes chunk back into `free` rbuf

<!-- column: 1 -->
### SETUP

- Rpi 4, NetBSD 10.1, 10s runs
- `bench.sc`: strongest barriers
- `bench.opt`: **maximally relaxed**

### RESULTS

| Variant     | Throughput  |
| ----------- | ----------- |
| `bench.sc`  |  5.5M op/s  |
| `bendh.opt` |  6.5M op/s  |

<!-- reset_layout -->

## TOO STRONG 💪 BARRIERS ----> SLOW 🐢 CODE

<!-- end_slide -->

>

>

>

>


## THANK YOU 🙏

>

-> Come talk with me, *we are hiring*!

>


### CONTANT

- 📭 diogo.behrens@huawei.com
- 🌐 https://github.com/open-s4c

<!--
```


          █▀▀▀▀▀█ █▀▀█  █ ▄ █▀▀▀▀▀█
          █ ███ █  ▀ ▄ █ ▄  █ ██ █
          █ ▀▀▀ █ █▄▄▀▀▄█ ▄ █ ▀▀▀ █
          ▀▀▀▀▀▀▀ █▄▀▄▀ █▄▀ ▀▀▀▀▀▀▀
          ▀█▀▄ █▀ ▀  ▀▄ ██▀██▀▄ ██
           ▀█▄  ▀ ▀▀██  ▄███ ▄ █▀ ▀
           ▄ █ █▀▄▀▀███ ▄▄▀▄██   ▄█
          ▀ █▀▄▀█▀ ▀▀█▀▀█▀ ▀▄▄█▀ ▀
          ▀▀▀   ▀▀█ ▀█   ▀█▀▀▀█  ▀▄
          █▀▀▀▀▀█ ▄▀▄█ ▀▄▄█  █   ▀
          █ ███ █  █ █▄▄ █▀█▀▀█ ▄▄
          █ ▀▀▀ █ █▀ ▀▄  ▄█▀▄██▀ ▀▀
          ▀▀▀▀▀▀▀ ▀▀  ▀ ▀ ▀▀▀  ▀  ▀


```
-->

