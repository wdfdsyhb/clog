/* clog.c - implementation of the clog logging library. See clog.h. */
#include "clog.h"

#include <stdarg.h>
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
    g_colors = enabled ? 1 : 0;
}

void clog_log(int level, const char *file, int line, const char *fmt, ...)
{
    static const char *const tag[] = { "D", "I", "W", "E" };
    static const char *const color[] = { "\x1b[90m", "\x1b[32m", "\x1b[33m", "\x1b[31m" };
    char stamp[16];
    time_t now;
    struct tm tmv;
    va_list ap;

    if (level < CLOG_DEBUG || level >= CLOG_NONE)
        return;

    enable_vt_once();
    now = time(NULL);
#if defined(_WIN32)
    localtime_s(&tmv, &now);
#else
    localtime_r(&now, &tmv);
#endif
    strftime(stamp, sizeof(stamp), "%H:%M:%S", &tmv);

    fputs("[", stderr);
    fputs(stamp, stderr);
    fputs("] ", stderr);
    if (colors_enabled())
        fputs(color[level], stderr);
    fputs("[", stderr);
    fputs(tag[level], stderr);
    fputs("] ", stderr);
    if (g_prefix && *g_prefix) {
        fputs("[", stderr);
        fputs(g_prefix, stderr);
        fputs("] ", stderr);
    }

    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);

    fputs(" (", stderr);
    fputs(file, stderr);
    fprintf(stderr, ":%d)", line);
    if (colors_enabled())
        fputs("\x1b[0m", stderr);
    fputc('\n', stderr);
}
