#include "common.h"

pid_t getCurrentThreadId()
{
    return static_cast<pid_t>(::syscall(SYS_gettid));
}