#ifndef MRUBY_FIND_INTERNALS_H
#define MRUBY_FIND_INTERNALS_H 1

#include <mruby.h>
#include <mruby/array.h>
#include <mruby/class.h>
#include <mruby/data.h>
#include <mruby/error.h>
#include <mruby/hash.h>
#include <mruby/string.h>
#include <mruby/value.h>
#include <mruby/variable.h>
#include <stdlib.h>

#if MRUBY_RELEASE_NO >= 30000
# include <mruby/presym.h>
#else
# define MRB_CV_SYM(SYM)        mrb_intern_lit(mrb, "@@" #SYM)
# define MRB_IV_SYM(SYM)        mrb_intern_lit(mrb, "@" #SYM)
# define MRB_SYM_B(SYM)         mrb_intern_lit(mrb, #SYM "!")
# define MRB_SYM_E(SYM)         mrb_intern_lit(mrb, #SYM "=")
# define MRB_SYM_Q(SYM)         mrb_intern_lit(mrb, #SYM "?")
# define MRB_SYM(SYM)           mrb_intern_lit(mrb, #SYM)
#endif

#endif /* MRUBY_FIND_INTERNALS_H */
