#pragma once

/* Logging and debug messages */
#include <cstdlib>
#include <iostream>
#include <sstream>

#include <iosfwd>
#include <set>
#include <stdarg.h>
#include <string>

namespace crab {

#define CRAB_LOG(TAG, CODE)                                      \
    do {                                                         \
        if (crab::CrabLogFlag && crab::CrabLog.count(TAG) > 0) { \
            CODE;                                                \
        }                                                        \
    } while (0)
extern bool CrabLogFlag;
extern std::set<std::string> CrabLog;

extern unsigned CrabVerbosity;
#define CRAB_VERBOSE_IF(LEVEL, CODE)        \
    do {                                    \
        if (crab::CrabVerbosity >= LEVEL) { \
            CODE;                           \
        }                                   \
    } while (0)

template <typename... ArgTypes>
inline void ___print___(ArgTypes... args) {
    // trick to expand variadic argument pack without recursion
    using expand_variadic_pack = int[];
    // first zero is to prevent empty braced-init-list
    // void() is to prevent overloaded operator, messing things up
    // trick is to use the side effect of list-initializer to call a function
    // on every argument.
    // (void) is to suppress "statement has no effect" warnings
    (void)expand_variadic_pack{0, ((std::cerr << args), void(), 0)...};
}

#define CRAB_ERROR(...)              \
    do {                             \
        std::cerr << "CRAB ERROR: "; \
        ___print___(__VA_ARGS__);    \
        std::cerr << "\n";           \
        std::exit(EXIT_FAILURE);     \
    } while (0)

extern bool CrabWarningFlag;
void CrabEnableWarningMsg(bool b);

#define CRAB_WARN(...)                     \
    do {                                   \
        if (crab::CrabWarningFlag) {       \
            std::cerr << "CRAB WARNING: "; \
            ___print___(__VA_ARGS__);      \
            std::cerr << "\n";             \
        }                                  \
    } while (0)

constexpr bool CrabSanityCheckFlag = false;

} // end namespace crab
