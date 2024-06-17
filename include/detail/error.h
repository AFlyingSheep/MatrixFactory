#pragma once

#include <cstdio>
#include <cstring>

static char *shorten_path(const char *path) {
  char substr[] = "src";
  char *ptr = (char *)strstr(path, substr);
  return (ptr != NULL) ? ptr : (char *)path;
}

template <typename S, typename... Args>
void assertion_failure(const char *file, int line, const char *expression,
                       const S &comment, Args &&...args) {
  char buf[1024];
  snprintf(buf, sizeof(buf), comment, std::forward<Args>(args)...);
  fprintf(stderr, "Assertion '%s' failed at %s:%d, %s\n", expression,
          shorten_path(file), line, buf);
}

#define MATRIX_ASSERT(predicate, comment, ...)                                 \
  ((predicate) ? ((void)0)                                                     \
               : assertion_failure(__FILE__, __LINE__, #predicate, (comment),  \
                                   ##__VA_ARGS__))
