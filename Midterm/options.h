#ifndef __OPTIONS_H__
#define __OPTIONS_H__

#define FLAG_A            0x00000001
#define FLAG_a            0x00000002
#define FLAG_C            0x00000004
#define FLAG_c            0x00000008
#define FLAG_d            0x00000010
#define FLAG_F            0x00000020
#define FLAG_f            0x00000040
#define FLAG_h            0x00000080
#define FLAG_i            0x00000100
#define FLAG_k            0x00000200
#define FLAG_l            0x00000400
#define FLAG_n            0x00000800
#define FLAG_q            0x00001000
#define FLAG_R            0x00002000
#define FLAG_r            0x00004000
#define FLAG_S            0x00008000
#define FLAG_s            0x00010000
#define FLAG_t            0x00020000
#define FLAG_u            0x00040000
#define FLAG_w            0x00080000
#define FLAG_x            0x00100000
#define FLAG_1            0x00200000


#define ISFLAG_A(f)       ((f) & FLAG_A)
#define ISFLAG_a(f)       ((f) & FLAG_a)
#define ISFLAG_C(f)       ((f) & FLAG_C)
#define ISFLAG_c(f)       ((f) & FLAG_c)
#define ISFLAG_d(f)       ((f) & FLAG_d)
#define ISFLAG_F(f)       ((f) & FLAG_F)
#define ISFLAG_f(f)       ((f) & FLAG_f)
#define ISFLAG_h(f)       ((f) & FLAG_h)
#define ISFLAG_i(f)       ((f) & FLAG_i)
#define ISFLAG_k(f)       ((f) & FLAG_k)
#define ISFLAG_l(f)       ((f) & FLAG_l)
#define ISFLAG_n(f)       ((f) & FLAG_n)
#define ISFLAG_q(f)       ((f) & FLAG_q)
#define ISFLAG_R(f)       ((f) & FLAG_R)
#define ISFLAG_r(f)       ((f) & FLAG_r)
#define ISFLAG_S(f)       ((f) & FLAG_S)
#define ISFLAG_s(f)       ((f) & FLAG_s)
#define ISFLAG_t(f)       ((f) & FLAG_t)
#define ISFLAG_u(f)       ((f) & FLAG_u)
#define ISFLAG_w(f)       ((f) & FLAG_w)
#define ISFLAG_x(f)       ((f) & FLAG_x)
#define ISFLAG_1(f)       ((f) & FLAG_1)


#define SETFLAG_A(f) do { (f) |= FLAG_A; } while(0)
#define SETFLAG_a(f) do { (f) |= FLAG_a; } while(0)
#define SETFLAG_C(f) do { \
    (f) &= (~FLAG_1); \
    (f) &= (~FLAG_l); \
    (f) &= (~FLAG_n); \
    (f) &= (~FLAG_x); \
    (f) |= FLAG_C; \
} while(0)
#define SETFLAG_c(f) do { \
    (f) &= (~FLAG_u); \
    (f) |= FLAG_c; \
} while(0)
#define SETFLAG_d(f) do { \
    (f) &= (~FLAG_R); \
    (f) |= FLAG_d; \
} while(0)
#define SETFLAG_F(f) do { (f) |= FLAG_F; } while(0)
#define SETFLAG_f(f) do { (f) |= FLAG_f; } while(0)
#define SETFLAG_h(f) do { \
    (f) &= (~FLAG_k); \
    (f) |= FLAG_h; \
} while(0)
#define SETFLAG_i(f) do { (f) |= FLAG_i; } while(0)
#define SETFLAG_k(f) do { \
    (f) &= (~FLAG_h); \
    (f) |= FLAG_k; \
} while(0)
#define SETFLAG_l(f) do { \
    (f) &= (~FLAG_1); \
    (f) &= (~FLAG_C); \
    (f) &= (~FLAG_n); \
    (f) &= (~FLAG_x); \
    (f) |= FLAG_l; \
} while(0)
#define SETFLAG_n(f) do { \
    (f) &= (~FLAG_1); \
    (f) &= (~FLAG_C); \
    (f) &= (~FLAG_l); \
    (f) &= (~FLAG_x); \
    (f) |= FLAG_n; \
} while(0)
#define SETFLAG_q(f) do { \
    (f) &= (~FLAG_w); \
    (f) |= FLAG_q; \
} while(0)
#define SETFLAG_R(f) do { (f) |= FLAG_R; } while(0)
#define SETFLAG_r(f) do { (f) |= FLAG_r; } while(0)
#define SETFLAG_S(f) do { (f) |= FLAG_S; } while(0)
#define SETFLAG_s(f) do { (f) |= FLAG_s; } while(0)
#define SETFLAG_t(f) do { (f) |= FLAG_t; } while(0)
#define SETFLAG_u(f) do { \
    (f) &= (~FLAG_c); \
    (f) |= FLAG_u; \
} while(0)
#define SETFLAG_w(f) do { \
    (f) &= (~FLAG_q); \
    (f) |= FLAG_w; \
} while(0)
#define SETFLAG_x(f) do { \
    (f) &= (~FLAG_1); \
    (f) &= (~FLAG_C); \
    (f) &= (~FLAG_l); \
    (f) &= (~FLAG_n); \
    (f) |= FLAG_x; \
} while(0)
#define SETFLAG_1(f) do { \
    (f) &= (~FLAG_C); \
    (f) &= (~FLAG_l); \
    (f) &= (~FLAG_n); \
    (f) &= (~FLAG_x); \
    (f) |= FLAG_1; \
} while(0)

#define NEEDSTATS(f) (ISFLAG_i(f) || ISFLAG_l(f) || \
        ISFLAG_n(f) || ISFLAG_s(f) || ISFLAG_F(f))

#define NO_PRINT -1

unsigned int getFtsOptions(unsigned int flag);

#endif /* __OPTIONS_H__ */
