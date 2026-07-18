#include <stdio.h> //all the headers
#include <stdlib.h>
#include <string.h>

#include "index.h"
#include "object.h"
#include "repository.h"
#include "utils.h"

char *index_path(void) { //path for index dir
    return repo_path_join(".", ".vcs/index");
}

char *read_file_bytes(const char *path, size_t *out_len) {// allocates memry reads the file 
    FILE *f = fopen(path, "rb");
    if (!f) {
        return NULL;
    }
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    rewind(f);
    char *buf = malloc(size + 1);//give buf all the memry it needs
    if (!buf) { 
        fclose(f);
        return NULL;
    }
    size_t read_len = fread(buf, 1, size, f); //gives buf the contents of the file
    fclose(f);
    if (out_len) {
        *out_len = read_len;
    }
    return buf;
}
//appending into the index
bool index_add(const char *path, const char *hash) {
    char *index_file = index_path(); //check if index path is there or not
    if (!index_file) {
        return false;
    }

    FILE *f = fopen(index_file, "a"); //opens the index file 
    if (!f) {
        perror("fopen index");
        free(index_file);
        return false;
    }

    fprintf(f, "%s %s\n", path, hash); //writes the path and hash of the file inside the index file
    fclose(f);
    free(index_file);
    return true;
}


bool index_load(const char *path, char ***out_entries, size_t *out_count) { //out entries is what we shld update and same as out count
    (void)path;// this to avoid the warning (i did not know)
    char *index_file = index_path();
    if (!index_file) {
        return false;
    }
    FILE *f = fopen(index_file, "r");
    if (!f) {
        free(index_file);
        return false;
    }

    size_t count = 0;
    char **entries = NULL;
    char *line;
    while ((line = read_line(f)) != NULL) { //until the line is null
        if (line[0] == '\0') { //if the line is empty (null term )
            free(line);
            continue;
        }
        entries = realloc(entries, sizeof(char *) * (count + 1)); //keep reallocing till its the correct len
        if (!entries) {
            free(line);
            fclose(f);
            free(index_file);
            return false;
        }
        entries[count++] = line;
    }
    fclose(f);
    free(index_file);
    if (out_entries) {
        *out_entries = entries; //returning shi
    }
    if (out_count) {
        *out_count = count;
    }
    return true;
}

bool index_write(const char *path, char **entries, size_t count) {  //writing in index
    (void)path;
    char *index_file = index_path(); //index file path
    if (!index_file) {
        return false;
    }
    FILE *f = fopen(index_file, "w");
    if (!f) {
        perror("fopen index");
        free(index_file);
        return false;
    }
    for (size_t i = 0; i < count; ++i) {
        fprintf(f, "%s\n", entries[i]); //write in the file 
    }
    fclose(f);
    free(index_file);
    return true;
}

int cmd_add(const char *path) { //add cmd  
    if (!path) { //check path
        return 1;
    }
    if (!repo_exists(".")) { //check repo
        fprintf(stderr, "not a repository\n");
        return 1;
    }

    FILE *f = fopen(path, "rb"); //check file 
    if (!f) {
        perror("fopen");
        return 1;
    }
    fclose(f);

    size_t len = 0;
    char *content = read_file_bytes(path, &len); //take the content
    if (!content) {
        return 1;
    }

    char *hash = NULL; //write the file in the object dir
    if (!write_object_file(path, content, len, &hash)) {
        free(content);
        return 1;
    }

    if (!index_add(path, hash)) { //add the hash and path to the index file                 
        free(hash);
        free(content);
        return 1;
    }

    free(hash);
    free(content);
    return 0; //done
}
