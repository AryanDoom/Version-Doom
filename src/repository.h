#ifndef REPOSITORY_H
#define REPOSITORY_H

#include <stdbool.h>

bool repo_init(const char *repo_path);
bool repo_exists(const char *repo_path);
char *repo_path_join(const char *base, const char *child);
int cmd_init(void);

#endif
