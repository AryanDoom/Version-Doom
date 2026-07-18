#include <stdio.h> //all the headers
#include <stdlib.h>
#include <string.h>

#include "index.h"
#include "repository.h"
#include "utils.h"

int cmd_status(void) { //status 
    if (!repo_exists(".")) {
        fprintf(stderr, "not a repository\n");
        return 1;
    }

    char **entries = NULL;
    size_t count = 0;
    if (!index_load(".", &entries, &count)) { //load the index file 
        fprintf(stderr, "failed to load index\n");
        return 1;
    }

    for (size_t i = 0; i < count; ++i) { //go through each line
        char *line = entries[i];// get one line
        char *space = strchr(line, ' '); //find space
        if (!space) {
            continue;
        }
        *space = '\0';
        printf("%s\n", line); //print the line
    }

    for (size_t i = 0; i < count; ++i) {
        free(entries[i]); 
    }
    free(entries);
    return 0;
}
