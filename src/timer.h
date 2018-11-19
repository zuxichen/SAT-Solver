
#ifndef SAT_TIMER_H
#define SAT_TIMER_H

#include <chrono>

namespace sat
{
    class Timer
    {
    public:
        Timer(const int timeLimitSeconds);
        Timer(const std::chrono::nanoseconds& timeLimit);
        void start();
        void stop();
        bool timeout() const;
        std::chrono::nanoseconds elapsed() const;
        std::chrono::nanoseconds limit() const;

    private:
        std::chrono::nanoseconds timeLimitNanos;
        std::chrono::nanoseconds elapsedNanos;
        std::chrono::nanoseconds currElapsedNanos;
    };
}

#endif // SAT_TIMER_H
