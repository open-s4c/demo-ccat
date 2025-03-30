/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2024-2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */
#include <assert.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PAGE_SIZE 4096
#define CHUNK_SIZE 256
#define FREE_LEN 64
#define RBUF_LEN 16
#define pause()

#include "ringbuf.h"

struct chunk {
    char payload[CHUNK_SIZE];
    size_t len;
};

/* ring buffers */
ringbuf_t free_chunks;
ringbuf_t used_chunks;
ringbuf_t ready_chunks;

/* reader thread reads from the input file chunks */
void *
reader(void *arg)
{
    FILE *fp = fopen((const char *)arg, "r");
    if (fp == NULL) {
        perror("could not open file");
        exit(EXIT_FAILURE);
    }

    char data[PAGE_SIZE];
    struct chunk *c;
    size_t r;

    do {
        /* read large portion of data */
        r = fread(&data, 1, PAGE_SIZE, fp);

        /* split read data in chunks */
        for (size_t i = 0; i < r;) {
            /* get a free chunk */
            while (ringbuf_deq(&free_chunks, (void **)&c) != RINGBUF_OK)
                pause();

            /* calculate available data length and copy */
            c->len = r - i > CHUNK_SIZE ? CHUNK_SIZE : r - i;
            memcpy(&c->payload, data + i, c->len);
            i += c->len;

            /* pass ownership of c to mediator */
            while (ringbuf_enq(&used_chunks, c) != RINGBUF_OK)
                pause();
        }

    } while (r != 0);

    fclose(fp);

    /* send empty chunk to mark end of file */
    while (ringbuf_deq(&free_chunks, (void **)&c) != RINGBUF_OK)
        pause();
    c->len = 0;

    while (ringbuf_enq(&used_chunks, c) != RINGBUF_OK)
        pause();
    return 0;
}

/* consumes read chunks, maybe does some magic, and passes chunk to write */
void *
mediator(void *arg)
{
    struct chunk *c = NULL;
    bool stop = false;

    while (!stop) {
        /* get chunk from reader */
        while (ringbuf_deq(&used_chunks, (void **)&c) != RINGBUF_OK)
            pause();
        /* end of file marker */
        if (c->len == 0)
            stop = true;

        /* pass chunk ownership to writer */
        while (ringbuf_enq(&ready_chunks, c) != RINGBUF_OK)
            pause();
    }
    return 0;
}

/* consumes ready chunks, writes them to stdout, gives them back to reader */
void *
writer(void *arg)
{
    struct chunk *c = NULL;
    bool stop = false;

    while (!stop) {
        /* get chunk ready to be written */
        while (ringbuf_deq(&ready_chunks, (void **)&c) != RINGBUF_OK)
            pause();

        /* end of file? */
        if (c->len == 0)
            stop = true;
        else /* write chunk out */
            fwrite(c->payload, c->len, 1, stdout);

        /* give chunk ownership back to reader */
        while (ringbuf_enq(&free_chunks, c) != RINGBUF_OK)
            pause();
    }
    return 0;
}

int
main(int argc, char *argv[])
{
    if (argc != 2) {
        printf("usage: %s <filename>\n", argv[0]);
        return 1;
    }

    void *buf1 = malloc(sizeof(void *) * FREE_LEN);
    void *buf2 = malloc(sizeof(void *) * RBUF_LEN);
    void *buf3 = malloc(sizeof(void *) * RBUF_LEN);
    if (!buf1 || !buf2 || !buf3) {
        perror("buffer malloc");
        exit(EXIT_FAILURE);
    }

    ringbuf_init(&free_chunks, buf1, FREE_LEN);
    ringbuf_init(&used_chunks, buf2, RBUF_LEN);
    ringbuf_init(&ready_chunks, buf3, RBUF_LEN);

    for (int i = 0; i < FREE_LEN; i++) {
        struct chunk *c = (struct chunk *)malloc(sizeof(struct chunk));
        memset(c, 0, sizeof(struct chunk));
        if (ringbuf_enq(&free_chunks, c) != RINGBUF_OK) {
            perror("could not create chunks");
            exit(EXIT_FAILURE);
        }
    }

    pthread_t tr, tw, tm;
    pthread_create(&tr, 0, reader, argv[1]);
    pthread_create(&tw, 0, writer, 0);
    pthread_create(&tm, 0, mediator, 0);
    pthread_join(tr, 0);
    pthread_join(tw, 0);
    pthread_join(tm, 0);

    fflush(stdout);
    return 0;
}
