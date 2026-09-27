/* test_clog.c - compile-time and runtime self-checks for clog.
 *
 * Runtime filter checks use clog_level_enabled() assertions; the visible
 * output exercises every level so colors/format can be verified by eye.
 */
#include "clog.h"

#include <assert.h>

int main(void)
{
    /* default: everything below CLOG_NONE enabled */
    assert(clog_get_level() == CLOG_DEBUG);
    assert(clog_level_enabled(CLOG_DEBUG));
    assert(clog_level_enabled(CLOG_ERROR));

    /* runtime filter */
    clog_set_level(CLOG_WARN);
    assert(!clog_level_enabled(CLOG_DEBUG));
    assert(!clog_level_enabled(CLOG_INFO));
    assert(clog_level_enabled(CLOG_WARN));
    assert(clog_get_level() == CLOG_WARN);

    /* invalid level set is ignored */
    clog_set_level(99);
    assert(clog_get_level() == CLOG_WARN);
    clog_set_level(-1);
    assert(clog_get_level() == CLOG_WARN);

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

    printf("clog self-test passed\n");
    return 0;
}
