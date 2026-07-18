#include <errno.h> // these are all the funcs we need 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

//it the system is windows 
#ifdef _WIN32 //defining mfdir for wind (i did not know this was a thing but it is apparently a thing)
#include <direct.h> //for _mkdir
#define mkdir(path, mode) _mkdir(path)
#endif

#include "repository.h"
#include "utils.h"

int ensure_directory(const char *path) {
    if (mkdir(path, 0777) == 0) { //linux mkdir method is 0777 
        return 0;
    }
    if (errno == EEXIST) { //directory is alreayd there
        return 0;
    }
    perror("mkdir"); //if both didnt happen then we have an error and we print it out
    return -1;
}

bool repo_init(const char *repo_path) {//initializes the repo and makes the .vcs directory and all the other directories and files that are needed for the repo to work
    if (!repo_path) {
        repo_path = "."; //ull be inside the proj folder so .vcs ka path will be ./.vcs 
    }

    char *dot_vcs = repo_path_join(repo_path, ".vcs");  //path to .vcs direc
    if (!dot_vcs) {
        return false;
    }

    if (ensure_directory(dot_vcs) != 0) {
        free(dot_vcs); //free my guy max b
        return false;
    }

    char *objects = repo_path_join(dot_vcs, "objects"); //pointers to the direcs and files we needd, the pointers are the paths 
    char *refs = repo_path_join(dot_vcs, "refs");
    char *head = repo_path_join(dot_vcs, "HEAD");
    char *index = repo_path_join(dot_vcs, "index");
    char *config = repo_path_join(dot_vcs, "config");

    if (!objects || !refs || !head || !index || !config) { //if any of these are null then exit
        free(dot_vcs);
        free(objects);
        free(refs);
        free(head);
        free(index);
        free(config);
        return false;
    }

    char *heads = repo_path_join(refs, "heads"); //head is to store curr branch name,very leetcode esque
    if (!heads) { //again free everything if its null
        free(dot_vcs);
        free(objects);
        free(refs);
        free(head);
        free(index);
        free(config);
        return false;
    }

    int ok = 0; //this is to keep track of if any of the direcs didnt get created 
    ok |= ensure_directory(objects);
    ok |= ensure_directory(refs);
    ok |= ensure_directory(heads);

    FILE *f = fopen(head, "w"); //now to take care of head
    if (!f) {
        perror("fopen HEAD");
        ok = -1;
    } else {
        fprintf(f, "ref: refs/heads/master\n"); //curr branch path 
        fclose(f); //lowk useless rn as we dont have branches 
    }

    f = fopen(index, "w"); //empty index file, this is where one will store the files that are staged for commit
    if (!f) {
        perror("fopen index");
        ok = -1;
    } else {
        fclose(f);
    }

    f = fopen(config, "w");
    if (!f) {
        perror("fopen config");
        ok = -1;
    } else {
        fprintf(f, "[core]\n\trepositoryformatversion = 0\n\tfilemode = true\n\tbare = false\n\tDOOM = true\n"); //doom is always tru
        fclose(f);
    }

    free(dot_vcs); //again free everything, we dont need them anymore
    free(objects);
    free(refs);
    free(heads);
    free(head);
    free(index);
    free(config);
    return ok == 0;
}

bool repo_exists(const char *repo_path) { //to check if repo exists by checking if .vcs direc is tehre or not
    char *dot_vcs = repo_path_join(repo_path ? repo_path : ".", ".vcs");
    if (!dot_vcs) {
        return false;
    }
    struct stat st; //stat contains info bout the file or direc (use it to check if .vcs is there or not)
    int rc = stat(dot_vcs, &st);
    free(dot_vcs);
    return rc == 0 && S_ISDIR(st.st_mode); //s_isdirec checks if the file is a direc or not, if it is then repo exists
}

char *repo_path_join(const char *base, const char *child) { //join paths together, base is proj ka path and child is file that is passed ti it
    size_t len = strlen(base) + strlen(child) + 2;
    char *out = malloc(len);
    if (!out) { 
        return NULL;
    }
#ifdef _WIN32
    snprintf(out, len, "%s\\%s", base, child); //prints it to a string 
#else
    snprintf(out, len, "%s/%s", base, child);
#endif
    return out;
}

int cmd_init(void) { //func call when vcs init is ran
    if (!repo_init(".")) { // this main func call which initialized it
        fprintf(stderr, "failed to initialize repository\n");
        return 1;
    }
    printf("Initialized empty VCS repository in .vcs\n"); //type shi
    return 0;
}
