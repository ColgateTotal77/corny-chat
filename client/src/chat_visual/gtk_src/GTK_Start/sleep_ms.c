#include "GTK.h"

// Convert milliseconds to seconds
void sleep_ms(int milliseconds)
{
    struct timespec req, rem;
    req.tv_sec = 0;
    req.tv_nsec = milliseconds * 1000000;
    nanosleep(&req, &rem);
}
