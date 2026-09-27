/* test_clog.c - compile-time and runtime self-checks for clog.
 *
 * Runtime filter checks use CHECK() (not assert(): NDEBUG builds must
 * still verify something); the visible output exercises every level so
 * colors/format can be verified by eye.
 */
#include "clog.h"

#include <stdio.h>

#define CHECK(cond) \
    do { \
        if (!(cond)) { \
            fprintf(stderr, "CHECK failed at %s:%d: %s\n", __FILE__, __LINE__, #cond); \
            return 1; \
        } \
    } while (0)

int main(void)
{
    /* default: everything below CLOG_NONE enabled */
    CHECK(clog_get_level() == CLOG_DEBUG);
    CHECK(clog_level_enabled(CLOG_DEBUG));
    CHECK(clog_level_enabled(CLOG_ERROR));

    /* runtime filter */
    clog_set_level(CLOG_WARN);
    CHECK(!clog_level_enabled(CLOG_DEBUG));
    CHECK(!clog_level_enabled(CLOG_INFO));
    CHECK(clog_level_enabled(CLOG_WARN));
    CHECK(clog_get_level() == CLOG_WARN);

    /* invalid level set is ignored */
    clog_set_level(99);
    CHECK(clog_get_level() == CLOG_WARN);
    clog_set_level(-1);
    CHECK(clog_get_level() == CLOG_WARN);

    /* these two are dropped at runtime */
    LOG_DEBUG("you should NOT see this (dropped at WARN level)");
    LOG_INFO("you should NOT see this either");

    /* back to full verbosity for the visible part */
    clog_set_level(CLOG_DEBUG);
    clog_set_prefix("MOD");
    LOG_DEBUG("debug line with %d + %d", 1, 2);
    LOG_INFO("info line, chars '%s'", "ok");
    LOG_WARN("warn line, %.2f uses floats too", 3.14);
    clog_set_prefix(NULL);
    LOG_ERROR("error line, no module prefix: code=%#06x", 0xC0DE);

    /* colors can be forced (handy when piping through tee/colortail) */
    clog_set_colors(1);
    LOG_INFO("forced-color line");
    clog_set_colors(0);
    LOG_INFO("forced-plain line");
    clog_set_colors(-1);
    LOG_INFO("back to auto-detection");

    printf("clog self-test passed\n");
    return 0;
}
