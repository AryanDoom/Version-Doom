#include "utils.h"

char *xstrdup(const char *s) { //makes a duplicate of a sstring on the heap and returna  pointer to the copu
    size_t len = strlen(s) + 1;
    char *out = malloc(len);
    if (!out) {
        return NULL;
    }
    memcpy(out, s, len); //copys a specified number of bytes from one location to another
    return out;
}
// takes the file and retusn teh like
char *read_line(FILE *stream) {
    char buffer[4096];
    size_t len = 0;
    int ch;

    while ((ch = fgetc(stream)) != EOF) {
        if (ch == '\n') {
            break;
        }
        if (len + 1 >= sizeof(buffer)) {
            break;
        }
        buffer[len++] = (char)ch;
    }

    if (len == 0 && ch == EOF) { //nun was in the line
        return NULL;
    }

    buffer[len] = '\0';
    return xstrdup(buffer); //return it
}

char *append_text(char *base, const char *extra) { //appends a string to another one and returns new string 
    size_t base_len = base ? strlen(base) : 0;
    size_t extra_len = extra ? strlen(extra) : 0;
    char *out = malloc(base_len + extra_len + 1);
    if (!out) {
        return NULL;
    }
    if (base_len > 0) {
        memcpy(out, base, base_len);
    }
    if (extra_len > 0) {
        memcpy(out + base_len, extra, extra_len);
    }
    out[base_len + extra_len] = '\0';
    free(base);
    return out;
}
