/*
 * clog.h - a tiny embedded-style logging library for C99.
 *
 * Design goals:
 *   - no dynamic memory, no third-party deps
 *   - compile-time stripping: define CLOG_DISABLE_DEBUG (or CLOG_DISABLE_LOG)
 *     to compile selected levels to no-ops
 *   - runtime level filter, colored output, optional module prefix
 *   - output goes to stderr, log lines end with (file:line)
 *
 * Format: [HH:MM:SS] [I] [module] message (file.c:42)
 */
#ifndef CLOG_H
#define CLOG_H

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

enum clog_level {
    CLOG_DEBUG = 0,
    CLOG_INFO = 1,
    CLOG_WARN = 2,
    CLOG_ERROR = 3,
    CLOG_NONE = 4 /* nothing is logged at or above this */
};

/* Runtime filter: messages below `level` are dropped. Default CLOG_DEBUG. */
void clog_set_level(int level);
int  clog_get_level(void);

/* ANSI colors on/off (auto-detected default: on for terminals, off when not). */
void clog_set_colors(int enabled);

/* Optional module tag, printed as [MOD]. NULL clears it. */
void clog_set_prefix(const char *prefix);

/* True if a message at `level` would currently be emitted. */
int  clog_level_enabled(int level);

/* Core function; use the macros below instead of calling this directly. */
void clog_log(int level, const char *file, int line, const char *fmt, ...);

#define CLOG_LOG(level, ...) \
    do { \
        if (clog_level_enabled(level)) \
            clog_log((level), __FILE__, __LINE__, __VA_ARGS__); \
    } while (0)

#ifdef CLOG_DISABLE_DEBUG
#define LOG_DEBUG(...) ((void)0)
#else
#define LOG_DEBUG(...) CLOG_LOG(CLOG_DEBUG, __VA_ARGS__)
#endif

#ifdef CLOG_DISABLE_LOG
#define LOG_INFO(...)  ((void)0)
#define LOG_WARN(...)  ((void)0)
#define LOG_ERROR(...) ((void)0)
#else
#define LOG_INFO(...)  CLOG_LOG(CLOG_INFO, __VA_ARGS__)
#define LOG_WARN(...)  CLOG_LOG(CLOG_WARN, __VA_ARGS__)
#define LOG_ERROR(...) CLOG_LOG(CLOG_ERROR, __VA_ARGS__)
#endif

#ifdef __cplusplus
}
#endif

#endif /* CLOG_H */
