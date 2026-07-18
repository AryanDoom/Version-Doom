#ifndef OBJECT_H
#define OBJECT_H

#include <stdbool.h>

bool write_object_file(const char *path, const char *content, size_t len, char **out_hash);
bool object_exists(const char *hash);
char *read_object(const char *hash, size_t *out_len);
int cmd_hash_object(const char *path, int write_object);
int cmd_cat_object(const char *hash);

#endif
