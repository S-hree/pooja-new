#include <stdlib.h>

#include <platform/common.h>

#ifdef __CRATON__
#include <craton/debug.h>
#define bug_printf(...) debug_printf(__VA_ARGS__)

#else /* __CRATON__ */
#include <stdio.h>
#define bug_printf(...) fprintf(stderr, __VA_ARGS__)

#endif /* __CRATON__ */

void
bug_handler(const char *file,
            int line,
            const char *condition)
{
  bug_printf("BUG_ON(%s) @ %s:%d\n", condition, file, line);
  abort();
}
