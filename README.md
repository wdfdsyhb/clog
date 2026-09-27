# clog

A tiny embedded-style logging library for C99. No dynamic memory, no
dependencies, ~200 lines of C.

```c
#include "clog.h"

int main(void)
{
    clog_set_prefix("MOD");
    LOG_INFO("boot ok, firmware %d.%d", 1, 7);
    LOG_WARN("battery low: %d%%", 12);
    LOG_DEBUG("tick=%d", get_tick());   /* compiles to nothing with -DCLOG_DISABLE_DEBUG */
    return 0;
}
```

Output (stderr, colored when it is a terminal):

```
[14:03:22] [I] [MOD] boot ok, firmware 1.7 (main.c:5)
[14:03:22] [W] [MOD] battery low: 12% (main.c:6)
```

## Features

- **Runtime filter**: `clog_set_level()` - drop DEBUG/INFO/WARN at runtime
- **Compile-time stripping**: `-DCLOG_DISABLE_DEBUG` compiles `LOG_DEBUG`
  to `((void)0)` (zero size, zero runtime); `-DCLOG_DISABLE_LOG` strips
  everything - for release firmware
- **Colors**: per-level ANSI colors, auto-detected terminal, forced via
  `clog_set_colors()`; on Windows the console VT mode is enabled for you
- **Module prefix**: `clog_set_prefix("MOD")` - identify subsystems
- **Location**: every line ends with `(file:line)` via `__FILE__/__LINE__`
- **No heap**: fixed stack buffers only; safe for constrained targets
  (v0.1 is not locked/thread-safe yet - single-threaded or protect calls)

## Build

CMake:

```bash
cmake -B build && cmake --build build && ctest --test-dir build
```

or directly:

```bash
sh build.sh          # gcc/clang
build_vs.bat         # MSVC (VS Developer Prompt)
```

## Limits (v0.1)

- Not thread-safe (single call-site locking is up to you)
- `__FILE__` may print absolute paths depending on your build system
- Timestamp is wall-clock `HH:MM:SS`; no uptime/monotonic variant yet

## License

MIT
