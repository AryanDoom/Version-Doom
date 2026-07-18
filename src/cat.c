#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "object.h"
#include "repository.h"
#include "utils.h"

int cmd_cat_object(const char *hash) {
    if (!repo_exists(".")) {
        fprintf(stderr, "not a repository\n");
        return 1;
    }
    if (!hash || strlen(hash) != 64) {
        fprintf(stderr, "invalid object hash\n");
        return 1;
    }

    size_t len = 0;
    char *data = read_object(hash, &len);
    if (!data) {
        fprintf(stderr, "object not found\n");
        return 1;
    }

    fwrite(data, 1, len, stdout);
    free(data);
    return 0;
}
