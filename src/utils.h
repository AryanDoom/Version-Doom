#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *xstrdup(const char *s);
char *read_line(FILE *stream);
char *append_text(char *base, const char *extra);

#endif
