
#ifndef SAT_DEBUG_H
#define SAT_DEBUG_H

#include <iostream>

#ifdef DEBUG
#define ENABLE_DBGPRINT 1
#else
#define ENABLE_DBGPRINT 0
#endif

#define DBGPRINT(x) do { if (ENABLE_DBGPRINT) std::cerr << x << std::endl; } while (0)

#define DBGPRINTC(x, c) do { if (ENABLE_DBGPRINT) \
            sat::DBG_printIterable(std::cerr << x, c.begin(), c.end()) \
            << std::endl; } while (0)

namespace sat
{
    template <class Iterator>
    std::ostream& DBG_printIterable(std::ostream& out, Iterator begin, Iterator end)
    {
        out << "[";
        if (begin != end) {
            out << *begin;
            ++begin;
            for (; begin != end; ++begin) {
                out << " " << *begin;
            }
        }
        return out << "]";
    }
}

#endif // SAT_DEBUG_H
