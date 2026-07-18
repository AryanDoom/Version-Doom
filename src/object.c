#include <errno.h> //all the headers we need
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
 //if windows file then we need to change the mkdir func
#ifdef _WIN32
#include <direct.h>
#define mkdir(path, mode) _mkdir(path)
#endif

#include "hash.h" //all the headers from the proj
#include "object.h"
#include "repository.h"
#include "utils.h"

char *object_dir(void) { // the part we will use here is the one inside the object folder (which we already made)
    return repo_path_join(".", ".vcs/objects");
}

bool object_exists(const char *hash) {
    if (!hash || strlen(hash) != 64) { //here hash of the file contents (if its not 64 or js doesnt exist then get out)
        return false;
    }
    char *dir = object_dir(); //dir path of objects folder
    if (!dir) {
        return false; //no path
    }
    char *path = malloc(strlen(dir) + 65);  //65 is hardcoded cuz hash is 64 and \0
    if (!path) { //no path
        free(dir);
        return false;
    }
    snprintf(path, strlen(dir) + 65, "%s/%s", dir, hash); //this syntaz is snprintf(destination,maxsize,formal,values)
    struct stat st; //metadata struct
    int rc = stat(path, &st);
    free(path);
    free(dir);
    return rc == 0 && S_ISREG(st.st_mode); // S_ISREG is to check if its a file (this shld return true normally)

}

bool write_object_file(const char *path, const char *content, size_t len, char **out_hash) { //path to the main file 
    if (!path || !content || !out_hash) {//content is the pointer to the bytes of the file in memry , len is
        return false; //out hash is the shi we return the thing in
    }

    char *hash = sha256_hex(content, len); //making the hash
    if (!hash) { //if it fails (shi is null)
        return false;
    }

    if (object_exists(hash)) { //see if the object file already exists (if there is ever a hash collision this is where it will happen)
        *out_hash = hash;
        return true; // basically adding twice
    }

    char *dir = object_dir(); 
    if (!dir) { //object dir dont exist 
        free(hash);
        return false;
    }

    FILE *f = fopen(dir, "r"); //check if dir is there (objects dir) 
    if (!f) {
        if (mkdir(dir, 0777) != 0 && errno != EEXIST) { //if not then create it 
            perror("mkdir objects dir");
            free(dir);
            free(hash);
            return false;
        }
    } else {
        fclose(f);
    }

    char *obj_path = malloc(strlen(dir) + 65); //hash + 1 + path
    if (!obj_path) {
        free(dir);
        free(hash);
        return false;
    }
    snprintf(obj_path, strlen(dir) + 65, "%s/%s", dir, hash);

    FILE *out = fopen(obj_path, "wb"); // making the file where we weill write the file contents into 
    if (!out) {
        perror("fopen object file");
        free(obj_path);
        free(dir);
        free(hash);
        return false;
    }

    if (fwrite(content, 1, len, out) != len) { //if the number things written isnt the same as amt to be written
        free(obj_path);
        free(dir);
        free(hash);
        return false;
    }
    fclose(out);

    free(obj_path);
    free(dir);
    *out_hash = hash; // making the hash (impt thing)
    return true;
}

char *read_object(const char *hash, size_t *out_len) { //hash is the 64 char , out len is the len we gotta nake a pointer to
    if (!hash || strlen(hash) != 64) {
        return NULL;
    }
    char *dir = object_dir();//folder path
    if (!dir) {
        return NULL;
    }
    char *path = malloc(strlen(dir) + 65);
    if (!path) {
        free(dir);
        return NULL;
    }
    snprintf(path, strlen(dir) + 65, "%s/%s", dir, hash);

    FILE *f = fopen(path, "rb"); 
    if (!f) {
        free(path);
        free(dir);
        return NULL;
    }

    fseek(f, 0, SEEK_END); //to find file size
    long size = ftell(f);
    rewind(f);

    char *data = malloc(size + 1); //allocate memory for the file 
    if (!data) {
        fclose(f);
        free(path);
        free(dir);
        return NULL;
    }
    size_t read_len = fread(data, 1, size, f); //its suprisingly returns how many things he read
    fclose(f);
    free(path);
    free(dir);
    if (out_len) {
        *out_len = read_len;
    }
    return data; //returning the data we read
}
//basically the same function but it does not save the thing (it can) to objects folder (it cannot committ it tho)
int cmd_hash_object(const char *path, int write_object) { //path to the file and a flag if one shld save the object or nah
    FILE *f = fopen(path, "rb");
    if (!f) {
        perror("fopen");
        return 1;
    }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    rewind(f);
    char *buf = malloc(size + 1);
    if (!buf) {
        fclose(f);
        return 1;
    }
    size_t read_len = fread(buf, 1, size, f);
    fclose(f);

    char *hash = sha256_hex(buf, read_len);
    if (!hash) {
        free(buf);
        return 1;
    }

    if (write_object) {
        char *stored_hash = NULL;
        if (!write_object_file(path, buf, read_len, &stored_hash)) {
            free(hash);
            free(buf);
            return 1;
        }
        printf("%s\n", stored_hash);
        free(stored_hash);
    } else {
        printf("%s\n", hash);
    }

    free(hash);
    free(buf);
    return 0;
}
