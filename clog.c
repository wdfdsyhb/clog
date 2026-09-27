/* clog.c - implementation of the clog logging library. See clog.h. */

/* expose localtime_r on strict -std=c99 glibc builds (must precede includes) */
#if !defined(_WIN32)
#define _POSIX_C_SOURCE 200112L
#endif

#include "clog.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <unistd.h> /* isatty */
#endif

static int g_level = CLOG_DEBUG;
static int g_colors = -1; /* -1 = auto */
static const char *g_prefix = NULL;

void clog_set_level(int level)
{
    if (level >= CLOG_DEBUG && level <= CLOG_NONE)
        g_level = level;
}

int clog_get_level(void)
{
    return g_level;
}

int clog_level_enabled(int level)
{
    return level >= g_level && level < CLOG_NONE;
}

void clog_set_prefix(const char *prefix)
{
    g_prefix = prefix;
}

#if defined(_WIN32)
/* Enable ANSI escape processing on classic conhost; Windows Terminal
 * already supports it but enabling again is harmless. */
static void enable_vt_once(void)
{
    static int done = 0;
    HANDLE h;
    DWORD mode = 0;
    if (done)
        return;
    done = 1;
    h = GetStdHandle(STD_ERROR_HANDLE);
    if (h != INVALID_HANDLE_VALUE && GetConsoleMode(h, &mode))
        SetConsoleMode(h, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
}
#else
static void enable_vt_once(void)
{
}
#endif

static int colors_enabled(void)
{
#if defined(_WIN32)
    if (g_colors >= 0)
        return g_colors;
    /* color only when stderr is a real console, not a redirected file */
    {
        HANDLE h = GetStdHandle(STD_ERROR_HANDLE);
        DWORD mode = 0;
        return (h != INVALID_HANDLE_VALUE && GetConsoleMode(h, &mode)) ? 1 : 0;
    }
#else
    return (g_colors < 0) ? (isatty(2) ? 1 : 0) : g_colors;
#endif
}

void clog_set_colors(int enabled)
{
    if (enabled == -1)
        g_colors = -1; /* back to auto */
    else
        g_colors = enabled ? 1 : 0;
}

/* Append `fmt` to buf at pos; returns the new position (clamped, always
 * null-terminated). No dynamic allocation. */
static size_t appendf(char *buf, size_t cap, size_t pos, const char *fmt, ...)
{
    va_list ap;
    int n;
    if (pos + 1 >= cap)
        return pos;
    va_start(ap, fmt);
    n = vsnprintf(buf + pos, cap - pos, fmt, ap);
    va_end(ap);
    if (n < 0)
        return pos;
    return pos + ((size_t)n > cap - pos - 1 ? cap - pos - 1 : (size_t)n);
}

void clog_log(int level, const char *file, int line, const char *fmt, ...)
{
    static const char *const tag[] = { "D", "I", "W", "E" };
    static const char *const color[] = { "\x1b[90m", "\x1b[32m", "\x1b[33m", "\x1b[31m" };
    char stamp[16];
    char buf[512];
    size_t pos = 0;
    int colored;
    time_t now;
    struct tm tmv = {0};
    va_list ap;

    if (level < CLOG_DEBUG || level >= CLOG_NONE)
        return;

    enable_vt_once();
    colored = colors_enabled();
    now = time(NULL);
#if defined(_WIN32)
    localtime_s(&tmv, &now);
#else
    localtime_r(&now, &tmv);
#endif
    strftime(stamp, sizeof(stamp), "%H:%M:%S", &tmv);

    pos = appendf(buf, sizeof(buf), pos, "[%s] ", stamp);
    if (colored)
        pos = appendf(buf, sizeof(buf), pos, "%s", color[level]);
    pos = appendf(buf, sizeof(buf), pos, "[%s] ", tag[level]);
    if (g_prefix && *g_prefix)
        pos = appendf(buf, sizeof(buf), pos, "[%s] ", g_prefix);
    va_start(ap, fmt);
    {
        char msg[384]; /* message part: capped so head/tail always fit */
        vsnprintf(msg, sizeof(msg), fmt, ap);
        pos = appendf(buf, sizeof(buf), pos, "%s", msg);
    }
    va_end(ap);
    pos = appendf(buf, sizeof(buf), pos, " (%s:%d)", file, line);
    if (colored)
        pos = appendf(buf, sizeof(buf), pos, "%s", "\x1b[0m");
    pos = appendf(buf, sizeof(buf), pos, "%s", "\n");
    fwrite(buf, 1, pos, stderr);
}
