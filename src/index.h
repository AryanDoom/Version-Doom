#ifndef INDEX_H
#define INDEX_H

#include <stdbool.h>

bool index_add(const char *path, const char *hash);
bool index_load(const char *path, char ***out_entries, size_t *out_count);
bool index_write(const char *path, char **entries, size_t count);
int cmd_add(const char *path);
int cmd_status(void);

#endif
