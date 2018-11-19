
#include "timer.h"

using Nanos = std::chrono::nanoseconds;
using Sec = std::chrono::seconds;
using Clock = std::chrono::high_resolution_clock;

sat::Timer::Timer(const int timeLimitSeconds):
    Timer(Sec(timeLimitSeconds)) {}

sat::Timer::Timer(const std::chrono::nanoseconds& timeLimit):
    timeLimitNanos(timeLimit), elapsedNanos(Nanos(0)),
    currElapsedNanos(Nanos(0)) {}

void sat::Timer::start()
{
    currElapsedNanos -= Clock::now().time_since_epoch();
}

void sat::Timer::stop()
{
    currElapsedNanos += Clock::now().time_since_epoch();
    elapsedNanos += currElapsedNanos;
    currElapsedNanos = Nanos(0);
}

bool sat::Timer::timeout() const
{
    return elapsedNanos >= timeLimitNanos;
}

std::chrono::nanoseconds sat::Timer::elapsed() const
{
    return elapsedNanos;
}

std::chrono::nanoseconds sat::Timer::limit() const
{
    return timeLimitNanos;
}
