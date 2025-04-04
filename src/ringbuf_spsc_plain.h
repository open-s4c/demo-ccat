/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#ifndef RINGBUF_H
#define RINGBUF_H

#define RINGBUF_OK 0
#define RINGBUF_EMPTY 1
#define RINGBUF_FULL 2
#define RINGBUF_AGAIN 3

typedef struct {
    void **buf;
    unsigned int head;
    unsigned int tail;
    unsigned int size;
} ringbuf_t;

static inline void
ringbuf_init(ringbuf_t *q, void **b, unsigned int s)
{
    q->buf = b;
    q->size = s;
    q->head = 0;
    q->tail = 0;
}

static inline int
ringbuf_enq(ringbuf_t *q, void *v)
{
#if 0
    if (q->tail - q->head == q->size)
        return RINGBUF_FULL;
#else
    // if these are actually loops
    // the compiler might optimize that and the program hangs
    // unless tail and head are "volatile"
    // see ringbuf_spsc_volatile.h
    while (q->tail - q->head == q->size)
        ;
#endif

    q->buf[q->tail % q->size] = v;
    q->tail++;

    return RINGBUF_OK;
}

static inline int
ringbuf_deq(ringbuf_t *q, void **v)
{
    // same as condition in ringbuf_enq.
    if (q->tail - q->head == 0)
        return RINGBUF_EMPTY;

    *v = q->buf[q->head % q->size];
    q->head++;

    return RINGBUF_OK;
}
#endif
