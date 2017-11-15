#ifndef TYPE_FLOAT_H
#define TYPE_FLOAT_H

#ifdef LIBSRATE2_FLOAT
typedef float sr2_float_t;
#define SR2_(name) name ## _f
#else
#ifdef LIBSRATE2_DOUBLE
typedef double sr2_float_t;
#define SR2_(name) name ## _
#else
#ifdef LIBSRATE2_LDOUBLE
typedef long double sr2_float_t;
#define SR2_(name) name ## _l
#else
#error "No type was defined."
#endif
#endif
#endif

#endif
