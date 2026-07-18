#include <stdio.h>// all headers
#include <stdlib.h>
#include <string.h>

#include "commit.h"
#include "object.h"
#include "repository.h"
#include "utils.h"

char *extract_snapshot(const char *data) { //getting info from the snap shot
    char *snapshot = strstr(data, "snapshot=");
    if (!snapshot) {
        return NULL;
    }
    snapshot += 9;
    char *end = strchr(snapshot, '\0');
    if (end) {
        *end = '\0';
    }
    return snapshot; //now snaphot is this (phele it was a pointer)
}

int cmd_checkout(const char *commit_hash) { //checkout cmd
    if (!repo_exists(".")) { //check if repo exists
        fprintf(stderr, "not a repository\n");
        return 1;
    }
    if (!commit_hash || strlen(commit_hash) != 64) { //jus check if the shi is right or not
        fprintf(stderr, "invalid commit hash\n");
        return 1;
    }

    size_t len = 0; 
    char *data = read_object(commit_hash, &len); //get content (basically all the hash info or all the data in that file)
    if (!data) { //the data is basically the commit object so parent message and all
        fprintf(stderr, "commit not found\n");
        return 1;
    }

    char *snapshot = extract_snapshot(data); //get snapshot info
    if (!snapshot) { //snapshot is js all the file name and thier hash at that point
        free(data);
        fprintf(stderr, "commit has no snapshot\n");
        return 1;
    }

    char *line = strtok(snapshot, "\n"); //splits the snapshots into lines
    while (line) { //as long as it isnt empty
        char *space = strchr(line, ' ');
        if (space) {
            *space = '\0'; //split the line
            char *hash = space + 1;
            char *path = line;
            size_t object_len = 0;
            char *object_data = read_object(hash, &object_len); //now get teh data from hash
            if (!object_data) {
                free(data);
                fprintf(stderr, "missing object for %s\n", path);
                return 1;
            }
            FILE *out = fopen(path, "wb");
            if (!out) {
                free(object_data);
                free(data);
                perror("fopen checkout target");
                return 1;
            }
            fwrite(object_data, 1, object_len, out); //restore the file
            fclose(out);
            free(object_data);
        }
        line = strtok(NULL, "\n"); //for the loop
    }

    free(data);
    return 0;
}
