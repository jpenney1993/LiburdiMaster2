#include "CoreTime.h"

#include <sys/time.h>
#include <time.h>


namespace Vreo
{



uint64_t timeMonotonic()
{
    struct timespec now;

    if(::clock_gettime(CLOCK_MONOTONIC, &now) != 0)
    {
        return 0;
    }

    return ((uint64_t)now.tv_sec * 1000000000LL + (uint64_t)now.tv_nsec);
}


}
