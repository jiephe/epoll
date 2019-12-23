#include "err_string.h"

#include <errno.h>
#include <string.h>

__thread char t_errnobuf[512];
const char* strerror_tl(int savedErrno)
{
	return strerror_r(savedErrno, t_errnobuf, sizeof t_errnobuf);
}
