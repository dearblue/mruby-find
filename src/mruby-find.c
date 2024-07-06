#include <mruby.h>
#include "internals.h"

#if (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199900L) || (defined(__cplusplus) && __cplusplus >= 201100L)
# define TODO(MESG)             _Pragma(TODO_01(message("TODO: " MESG)))
# define TODO_01(ENTITY)        TODO_02(ENTITY)
# define TODO_02(ENTITY)        #ENTITY
#else
# define TODO(MESG)
#endif

#define LOG()                   do { fprintf(stderr, "[%s:%d:%s]\n", __FILE__, __LINE__, __func__); } while (0)
#define LOGS(MESG)              do { fprintf(stderr, "[%s:%d:%s] %s\n", __FILE__, __LINE__, __func__, MESG); } while (0)
#define LOGF(FORMAT, ...)       do { fprintf(stderr, "[%s:%d:%s] " FORMAT "\n", __FILE__, __LINE__, __func__, __VA_ARGS__); } while (0)

#include <errno.h>
#include <sys/stat.h>

static mrb_value
dir_error(mrb_state *mrb, const char *path, mrb_bool ignore_err)
{
  if (ignore_err) {
    switch (errno) {
#ifdef ELOOP
    case ELOOP:
#endif
    case EACCES:
    case EINVAL:
    case ENAMETOOLONG:
    case ENOENT:
    case ENOTDIR:
      return mrb_nil_value();
    default:
      break;
    }
  }

  mrb_sys_fail(mrb, path);

  return mrb_nil_value(); // not reached
}

#if !(defined(_WIN32) || defined(_WIN64))
# include <dirent.h>
#else
# include "win32imp.inc"
#endif // _WIN32, _WIN64

static mrb_value
prepare_prefix(mrb_state *mrb, const char path[])
{
  mrb_value prefix = mrb_str_new_cstr(mrb, path);

  if (path[0] != '\0') {
#if !(defined(_WIN32) || defined(_WIN64))
    if (RSTRING_PTR(prefix)[RSTRING_LEN(prefix) - 1] != '/') {
      mrb_str_cat_cstr(mrb, prefix, "/");
    }
#else
    prefix = prepare_prefix_win32(mrb, path, prefix);
#endif // _WIN32
  }

  return prefix;
}

static mrb_value
file_directory_p(mrb_state *mrb, const char path[], mrb_bool ignore_err)
{
#if !(defined(_WIN32) || defined(_WIN64))
# if defined(MRUBY_FIND_NO_SYMBOLIC_LINK)
  struct stat st;
  if (stat(path, &st)) {
    return dir_error(mrb, path, ignore_err);
  }

  return mrb_bool_value(S_ISDIR(st.st_mode));
# else
  struct stat st;
  if (lstat(path, &st)) {
    return dir_error(mrb, path, ignore_err);
  }

  return mrb_bool_value(S_ISDIR(st.st_mode));
# endif // MRUBY_FIND_NO_SYMBOLIC_LINK
#else
  return file_directory_p_win32(mrb, path, ignore_err);
#endif // _WIN32
}

struct file_children_body
{
  DIR *dirp;
  mrb_value ent;
};

static mrb_value
file_children_body(mrb_state *mrb, void *userdata)
{
  struct file_children_body *ap = (struct file_children_body *)userdata;

  mrb_ary_modify(mrb, mrb_ary_ptr(ap->ent));
  ARY_SET_LEN(mrb_ary_ptr(ap->ent), 0);

  int ai = mrb_gc_arena_save(mrb);

  struct dirent *dp;
  errno = 0;
  while ((dp = readdir(ap->dirp))) {
    const char *p = dp->d_name;
    if (!(p[0] == '.' && (dp->d_namlen == 1 || (dp->d_namlen == 2 && p[1] == '.')))) {
      mrb_value path = mrb_str_new(mrb, p, dp->d_namlen);
      mrb_ary_push(mrb, ap->ent, path);
      mrb_gc_arena_restore(mrb, ai);
    }
  }

  return mrb_bool_value(errno == 0);
}

static mrb_value
file_children(mrb_state *mrb, const char path[], mrb_value ary, mrb_bool ignore_err)
{
  struct file_children_body apack = { opendir(path), ary };
  if (!apack.dirp) {
    return dir_error(mrb, path, ignore_err);
  }

  mrb_bool err;
  mrb_value result = mrb_protect_error(mrb, file_children_body, &apack, &err);
  closedir(apack.dirp);

  if (err) {
    mrb_exc_raise(mrb, result);
  }

  if (mrb_nil_p(result)) {
    return dir_error(mrb, path, ignore_err);
  } else {
    return prepare_prefix(mrb, path);
  }
}

static mrb_value
find_internals(mrb_state *mrb, mrb_value mod)
{
  (void)mod;

  //TODO("メソッド名隠しは過去の mruby でも正当か？")
  mrb->c->ci->mid = 0;

  mrb_sym func;
  const mrb_value *argv;
  mrb_int argc;
  mrb_get_args(mrb, "n*!", &func, &argv, &argc);

  if (func == MRB_SYM_Q(directory)) {
    const char *path;
    mrb_bool ignore_err;
    mrb_get_args(mrb, "nzb", &func, &path, &ignore_err);

    return file_directory_p(mrb, path, ignore_err);
  } else if (func == MRB_SYM(children)) {
    const char *path;
    mrb_value ary;
    mrb_bool ignore_err;
    mrb_get_args(mrb, "nzAb", &func, &path, &ary, &ignore_err);

    return file_children(mrb, path, ary, ignore_err);
  }

  mrb_raise(mrb, E_NOTIMP_ERROR, "INVALID FUNCTION NAME");

  return mrb_nil_value();
}

void
mrb_mruby_find_gem_init(mrb_state *mrb)
{
  struct RClass *find_module = mrb_define_module(mrb, "Find");
  mrb_define_class_method(mrb, find_module, "__find_internals__", find_internals, MRB_ARGS_ANY());
  //setlocale(LC_ALL, "");
}

void
mrb_mruby_find_gem_final(mrb_state *mrb)
{
  (void)mrb;
}
