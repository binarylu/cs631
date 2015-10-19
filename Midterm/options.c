#include <fts.h>

#include "options.h"

unsigned int
getFtsOptions(unsigned int flag) {
    unsigned int fts_options;
    fts_options = FTS_PHYSICAL;
    if (ISFLAG_a(flag)) {
        fts_options |= FTS_SEEDOT;
    }
    /*
    if (!ISFLAG_i(flag) && !ISFLAG_l(flag) &&
            !ISFLAG_S(flag) && !ISFLAG_s(flag) &&
            !ISFLAG_t(flag) && !ISFLAG_n(flag)) {
        fts_options |= FTS_NOSTAT;
    }
    */
    if (!ISFLAG_l(flag) && !ISFLAG_n(flag) &&
            !ISFLAG_d(flag) && !ISFLAG_F(flag)) {
        fts_options |= FTS_COMFOLLOW;
    }
    return fts_options;
}