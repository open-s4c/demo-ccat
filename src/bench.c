/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#if defined(__linux__) && !defined(_GNU_SOURCE)
#define _GNU_SOURCE
#endif
#include <assert.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <vsync/atomic.h>

#include "now.h"

#ifdef OPTIMIZED
    #include "ringbuf_spsc_opt.h"
#else
    #include "ringbuf_spsc_sc.h"
#endif

#define CHUNK_SIZE 4
#define RBUF_SIZE  16
#define pause()                                                                \
    if (vatomic32_read_rlx(&stop))                                             \
        return 0;

void set_cpu(int ci);

struct chunk {
    char payload[CHUNK_SIZE];
    size_t len;
};

/* ring buffers */
ringbuf_t free_chunks;
ringbuf_t used_chunks;

/* termination control */
vatomic32_t stop;

/* work count */
unsigned long consumed;

void *
producer(void *arg)
{
    struct chunk *c;
    set_cpu(0);
    char data[CHUNK_SIZE];
    int produced = 0;

    while (!vatomic32_read_rlx(&stop)) {
        *(int *)data = produced++;

        while (ringbuf_deq(&free_chunks, (void **)&c) != RINGBUF_OK)
            pause();

        c->len = CHUNK_SIZE;
        memcpy(&c->payload, data, c->len);

        while (ringbuf_enq(&used_chunks, c) != RINGBUF_OK)
            pause();
    }

    return 0;
}

void *
consumer(void *arg)
{
    struct chunk *c = NULL;

    set_cpu(2);

    while (!vatomic32_read_rlx(&stop)) {
        while (ringbuf_deq(&used_chunks, (void **)&c) != RINGBUF_OK)
            pause();

        consumed++;

        while (ringbuf_enq(&free_chunks, c) != RINGBUF_OK)
            pause();
    }
    return 0;
}

int
main(int argc, char *argv[])
{
    set_cpu(3);
    int period   = 10;
    size_t bsize = sizeof(void *) * RBUF_SIZE;
    void *buf1   = malloc(bsize);
    void *buf2   = malloc(bsize);
    if (!buf1 || !buf2) {
        perror("buffer malloc");
        exit(EXIT_FAILURE);
    }

    ringbuf_init(&free_chunks, buf1, RBUF_SIZE);
    ringbuf_init(&used_chunks, buf2, RBUF_SIZE);

    for (int i = 0; i < RBUF_SIZE; i++) {
        struct chunk *c = (struct chunk *)malloc(sizeof(struct chunk));
        memset(c, 0, sizeof(struct chunk));
        if (ringbuf_enq(&free_chunks, c) != RINGBUF_OK) {
            perror("could not create chunks");
            exit(EXIT_FAILURE);
        }
    }

    pthread_t t1, t2;
    nanosec_t ts_start = now();
    pthread_create(&t1, 0, producer, 0);
    pthread_create(&t2, 0, consumer, 0);

    sleep(period);
    vatomic32_write_rlx(&stop, 1);

    pthread_join(t1, 0);
    pthread_join(t2, 0);

    double elapsed = in_sec(now() - ts_start);
    printf("%.2f op/s\t\t%.2fs\n", consumed / elapsed, elapsed);
    return 0;
}


#ifndef SET_CPU_AFFINITY

void
set_cpu(int id)
{
    (void)id;
}

#elif defined(__linux__)

    #if !defined(_GNU_SOURCE)
        #error add "-D_GNU_SOURCE" to your compile command
    #endif
    #include <sched.h>
    #include <sys/sysinfo.h>
void
set_cpu(int id)
{
    cpu_set_t set;
    CPU_ZERO(&set);
    CPU_SET(id, &set);

    if (sched_setaffinity(0, sizeof(set), &set) < 0) {
        perror("failed to set affinity");
        exit(EXIT_FAILURE);
    }
}

#else /* !__linux__*/

void
set_cpu(int id)
{
    pthread_t pth;
    cpuset_t *cset;
    cpuid_t ci = (cpuid_t)id;
    cset       = cpuset_create();
    if (cset == NULL) {
        perror("cpuset_create");
        exit(EXIT_FAILURE);
    }
    cpuset_set(ci, cset);
    pth = pthread_self();
    if (pthread_setaffinity_np(pth, cpuset_size(cset), cset) != 0) {
        perror("setaffinity");
        exit(EXIT_FAILURE);
    }
    cpuset_destroy(cset);
}

#endif
