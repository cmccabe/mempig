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
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int g_am_daemonized;

void daemonize_me(void)
{
    pid_t pid;
    int sid, err, devnull_fd;

    pid = fork();
    if (pid < 0) {
        err = errno;
        mempig_log("fork failed: error %d (%s)\n", err, strerror(err));
        exit(EXIT_FAILURE);
    }
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }
    mempig_log_set_use_syslog(1);
    devnull_fd = open("/dev/null", O_RDONLY);
    if (devnull_fd < 0) {
        err = errno;
        mempig_log("daemonize_me: failed to open /dev/null: error %d (%s)\n",
                err, strerror(err));
        exit(EXIT_FAILURE);
    }
    umask(0);
    sid = setsid();
    if (sid < 0) {
        err = errno;
        mempig_log("daemonize_me: setsid failed: error %d (%s)\n",
                   err, strerror(err));
        exit(EXIT_FAILURE);
    }
    if ((chdir("/")) < 0) {
        err = errno;
        mempig_log("daemonize_me: chdir(/) failed: error %d (%s)\n",
                   err, strerror(err));
        exit(EXIT_FAILURE);
    }
    if (dup2(devnull_fd, STDIN_FILENO) < 0) {
        err = errno;
        mempig_log("daemonize_me: failed to dup /dev/null to stdin: "
                "error %d (%s)\n", err, strerror(err));
        exit(EXIT_FAILURE);
    }
    if (dup2(devnull_fd, STDOUT_FILENO) < 0) {
        err = errno;
        mempig_log("daemonize_me: failed to dup /dev/null to stdout: "
                "error %d (%s)\n", err, strerror(err));
        exit(EXIT_FAILURE);
    }
    if (dup2(devnull_fd, STDERR_FILENO) < 0) {
        err = errno;
        mempig_log("daemonize_me: failed to dup /dev/null to stderr: "
                "error %d (%s)\n", err, strerror(err));
        exit(EXIT_FAILURE);
    }
    close(devnull_fd);
}
