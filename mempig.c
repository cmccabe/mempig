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

#include "daemon.h"
#include "log.h"

#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

static void usage(void)
{
    mempig_log("mempig: a program which consumes memory.\n");
    mempig_log("options:\n");
    mempig_log("-a [amount]:         amount of memory to consume in bytes\n");
    mempig_log("-d:                  daemonize\n");
    mempig_log("-h:                  this help message\n");
    mempig_log("-n:                  skip populate stage\n");
}

int main(int argc, char **argv)
{
    int c, err, populate = 1, daemonize = 0;
    long long ll;
    size_t i, amt = 0;
    uint32_t *addr;

    while ((c = getopt(argc, argv, "a:dhn")) != -1) {
        switch (c) {
        case 'a':
            ll = atoll(optarg);
            if (ll <= 0) {
                mempig_log("invalid amount of memory specified: %s\n", optarg);
                exit(EXIT_FAILURE);
            } else if (ll % 4 != 0) {
                mempig_log("amount must be a multiple of 4.  -h for help.\n");
                exit(EXIT_FAILURE);
            } else if ((sizeof(long long) > sizeof(size_t)) &&
                ((long long)((size_t)ll) < ll)) {
                mempig_log("error: amt = %lld is greater than the maximum range "
                        "of size_t, which is %lld\n", ll, (long long)((size_t)-1));
                exit(EXIT_FAILURE);
            }
            amt = ll;
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
            mempig_log("Options parsing error.\n\n");
            usage();
            exit(EXIT_FAILURE);
        }
    }
    if (amt == 0) {
        mempig_log("you must specify how much memory to lock.  -h for help.\n");
        exit(EXIT_FAILURE);
    }
    addr = mmap(NULL, amt, PROT_READ | PROT_WRITE,
                MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (addr == MAP_FAILED) {
        err = errno;
        mempig_log("mmap failed: error %d (%s)\n", err, strerror(err));
        exit(EXIT_FAILURE);
    }
    mempig_log("successfully mmap'ed %"PRId64" bytes.\n", amt);
    if (populate) {
        /* touch all pages */
        for (i = 0; i < (amt / 4); i++) {
            addr[i] = i % 50;
        }
        mempig_log("successfully touched %"PRId64" bytes.\n", amt);
    }
    if (daemonize) {
        mempig_log("daemonizing...\n");
        errno = 0;
        daemonize_me();
    }
    /* lock the pages into memory */
    if (mlock(addr, amt)) {
        err = errno;
        mempig_log("mlock error: %d (%s)\n", err, strerror(err));
        exit(EXIT_FAILURE);
    }
    mempig_log("successfully locked %"PRId64" bytes.\n", amt);
    while (1) {
        sleep(100);
    }
    return EXIT_SUCCESS;
}
