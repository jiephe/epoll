#pragma once

#include <sys/syscall.h>
#include <unistd.h>
#include <sys/prctl.h>
#include <pthread.h>

pid_t getCurrentThreadId();
