/*
 * vim: ts=4:sw=4:tw=79:et
 *
 * Copyright (c) 2013, the Mempig authors.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 * 
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#define _GNU_SOURCE
#include <errno.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <string.h>
#include <unistd.h>

static void usage(void)
{
    fprintf(stderr, "mempig: a program which consumes memory.\n");
    fprintf(stderr, "options:\n");
    fprintf(stderr, "-a [amount]:         amount of memory to consume in bytes\n");
    fprintf(stderr, "-d:                  daemonize\n");
    fprintf(stderr, "-h:                  this help message\n");
    fprintf(stderr, "-n:                  skip populate stage\n");
}

int main(int argc, char **argv)
{
    int c, err, populate = 1, daemonize = 0;
    int64_t i, amt = -1;
    size_t mmap_len;
    uint32_t *addr;

    while ((c = getopt(argc, argv, "a:dhn")) != -1) {
        switch (c) {
        case 'a':
            amt = atoll(optarg);
            if (amt == 0) {
                fprintf(stderr, "invalid amount of memory specified: %s\n", optarg);
                exit(EXIT_FAILURE);
            }
            break;
        case 'd':
            daemonize = 1;
            break;
        case 'n':
            populate = 0;
            break;
        case 'h':
            usage();
            exit(EXIT_SUCCESS);
            break;
        default:
            fprintf(stderr, "Options parsing error.\n\n");
            usage();
            exit(EXIT_FAILURE);
        }
    }
    if (amt == -1) {
        fprintf(stderr, "you must specify how much memory to lock.  -h for help.\n");
        exit(EXIT_FAILURE);
    }
    if (amt % 4 != 0) {
        fprintf(stderr, "amount must be a multiple of 4.  -h for help.\n");
        exit(EXIT_FAILURE);
    }

    mmap_len = -1;
    if (mmap_len < amt) {
        fprintf(stderr, "error: amt = %"PRId64 " is greater than the maximum range "
                "of size_t, which is %zd\n", amt, mmap_len);
        exit(EXIT_FAILURE);
    }
    mmap_len = amt;
    addr = mmap(NULL, amt, PROT_READ | PROT_WRITE,
                MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (addr == MAP_FAILED) {
        err = errno;
        fprintf(stderr, "mmap failed: error %d (%s)\n", err, strerror(err));
        exit(EXIT_FAILURE);
    }
    fprintf(stderr, "successfully mmap'ed %"PRId64" bytes.\n", amt);
    if (populate) {
        /* touch all pages */
        for (i = 0; i < (amt / 4); i++) {
            addr[i] = i % 50;
        }
        fprintf(stderr, "successfully touched %"PRId64" bytes.\n", amt);
    }
    /* lock the pages into memory */
    if (mlock(addr, amt)) {
        err = errno;
        fprintf(stderr, "mlock error: %d (%s)\n", err, strerror(err));
        exit(EXIT_FAILURE);
    }
    fprintf(stderr, "successfully locked %"PRId64" bytes.\n", amt);
    if (daemonize) {
        fprintf(stderr, "daemonizing...\n");
        errno = 0;
        if (daemon(0, 0) == -1) {
            err = errno;
            fprintf(stderr, "attempt to daemonize failed: %d (%s)\n",
                    err, strerror(err));  
            exit(EXIT_FAILURE);
        }
    }
    while (1) {
        sleep(100);
    }
    return EXIT_SUCCESS;
}
