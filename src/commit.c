#include <stdio.h> //all the headers 
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "commit.h"
#include "index.h"
#include "object.h"
#include "repository.h"
#include "utils.h"

char *read_head() { 
    char *head_path = repo_path_join(".", ".vcs/HEAD"); //path to the header file 
    if (!head_path) { //if head path is emp
        return NULL;
    }
    FILE *f = fopen(head_path, "r");
    free(head_path);
    if (!f) {
        return NULL;
    }
    char *line = read_line(f);
    fclose(f);
    return line; //the line is ref:refs/heads/master
}

int write_head(const char *hash) { //ur file hash
    char *head_path = repo_path_join(".", ".vcs/HEAD");
    if (!head_path) {
        return 1;
    }
    FILE *f = fopen(head_path, "w");
    free(head_path);
    if (!f) {
        return 1;
    }
    fprintf(f, "%s\n", hash); //write the commit file hash into the file
    fclose(f);
    return 0;
}

int cmd_commit(const char *message) { //*message is ur commit msg
    if (!repo_exists(".")) { //check for repo
        fprintf(stderr, "not a repository\n");
        return 1;
    }

    char *parent = NULL; //null rn but it will be  the hash of the last commit
    char *head = read_head();
    if (head && strncmp(head, "ref: ", 5) == 0) { //if head begins with "ref :" which is 5 chars
        char *vcs_path = repo_path_join(".", ".vcs"); 
        if (!vcs_path) {
            free(head);
            return 1;
        }
        char *full_ref_path = repo_path_join(vcs_path, head + 5); //master file path 
        free(vcs_path);
        if (!full_ref_path) {
            free(head);
            return 1;
        }
        FILE *f = fopen(full_ref_path, "r");
        free(full_ref_path);
        if (f) {
            char buf[66] = {0};  //the hash is 64 chars so we keep 2 more if for \0 and sum bs
            if (fgets(buf, sizeof(buf), f)) { //this is there to take the last hash and store it in buf
                buf[strcspn(buf, "\r\n")] = '\0'; //this seems like sum regex thing i had to ask ai
                parent = xstrdup(buf); //parent now points to a copy of buf
            }
            fclose(f);
        }
    } else if (head) {
        parent = xstrdup(head); //now parents also points to a copy of head 
    }

    char *index_file = repo_path_join(".", ".vcs/index");
    if (!index_file) {
        free(head);
        free(parent);
        return 1;
    }

    FILE *f = fopen(index_file, "r");
    free(index_file);
    if (!f) {
        fprintf(stderr, "index is missing\n"); //no index cond
        free(head);
        free(parent);
        return 1;
    }

    char *snapshot = NULL; //null rn
    char *line = NULL;
    while ((line = read_line(f)) != NULL) { 
        snapshot = append_text(snapshot, line); //read the index file till its empty
        snapshot = append_text(snapshot, "\n"); //append to the snapshot type shi
        free(line);
    }
    fclose(f);

    char *commit_content = NULL; 
    size_t commit_len = snprintf(NULL, 0, "parent=%s\nmessage=%s\ntime=%ld\nsnapshot=%s", parent ? parent : "", message ? message : "", (long)time(NULL), snapshot ? snapshot : ""); //elnght of the commit text
    commit_content = malloc(commit_len + 1); //alloc that much memry
    if (!commit_content) {
        free(head);
        free(parent);
        free(snapshot);
        return 1;
    }
    sprintf(commit_content, "parent=%s\nmessage=%s\ntime=%ld\nsnapshot=%s",
            parent ? parent : "", message ? message : "", (long)time(NULL), snapshot ? snapshot : "");
    //keeps that string in commit_content
    char *hash = NULL;
    if (!write_object_file("commit", commit_content, strlen(commit_content), &hash)) { //writes commit content in the file names after the hash
        free(commit_content);  //this works for both adding func and commiting func 
        free(head); //it takes the commit content hashes it and makes that the name and makes a file with said name and stores the commit content in it 
        free(parent);
        free(snapshot);
        return 1;
    }

    // after commit object is written, decide whether to update HEAD
    if (head && strncmp(head, "ref: ", 5) == 0) {
        // attached HEAD – update branch ref
        char *vcs_path = repo_path_join(".", ".vcs");
        if (vcs_path) {
            char *ref_path = repo_path_join(vcs_path, head + 5);
            free(vcs_path);
            if (ref_path) {
                FILE *ref = fopen(ref_path, "w");
                free(ref_path);
                if (ref) {
                    fprintf(ref, "%s\n", hash);
                    fclose(ref);
                }
            }
        }
    } else {
        // detached HEAD – write the new commit hash into HEAD
        if (write_head(hash) != 0) {
            free(hash);
            free(commit_content);
            free(head);
            free(parent);
            free(snapshot);
            return 1;
        }
    }

    printf("%s\n", hash);
    free(hash);
    free(commit_content);
    free(head);
    free(parent);
    free(snapshot);
    //call clear index so we can clear it for next staging func
    if (!clear_index()) {
        fprintf(stderr, "didnt work for usm reason(the index didnt get cleared)\n");
    } 
    return 0;
}

int cmd_log(void) { //it prints the log func, basically a linked list to print all the hashs and commits before the latest commit
    char *head = read_head();
    if (!head) {
        return 1;
    }
    char hash[67] = {0}; //hash will be 65 long 
    if (strncmp(head, "ref: ", 5) == 0) {
        char *vcs_path = repo_path_join(".", ".vcs");
        if (!vcs_path) {
            free(head);
            return 1;
        }
        char *full_ref_path = repo_path_join(vcs_path, head + 5);
        free(vcs_path);
        if (!full_ref_path) {
            free(head);
            return 1;
        }
        FILE *f = fopen(full_ref_path, "r");
        free(full_ref_path);
        if (!f) {
            free(head);
            return 0; // No commits yet
        }
        if (!fgets(hash, sizeof(hash), f)) {
            fclose(f);
            free(head);
            return 1;
        }
        fclose(f);
    } else {
        strncpy(hash, head, sizeof(hash) - 1);
    }
    free(head);
    hash[strcspn(hash, "\r\n")] = '\0'; //clean up the hash string of newline and return carriage charac

    while (hash[0]) { //as long as the string aint enpty
        size_t len = 0; 
        char *data = read_object(hash, &len); //data contains the info about that commit (that is hash with 67 len)
        if (!data) {
            break;
        }
        //take the values from the string
        //basically parsing the commit object so we can cleanly present the logs
        char *parent = strstr(data, "parent="); 
        char *message = strstr(data, "message=");
        char *time = strstr(data, "time="); 
        if (parent) { //if parent is present (ishan reference) 
            parent += 7; //cuz parent= is 7 characters so make the pointer point to the exact starting of the parent string
            char *end = strchr(parent, '\n');
            if (end) {
                *end = '\0';
            }
        }
        if (message) {
            message += 8;
            char *end = strchr(message, '\n');
            if (end) {
                *end = '\0';
            }
        }
        if (time) {
            time += 5;
            char *end = strchr(time, '\n');
            if (end) {
                *end = '\0';
            }
        }
        printf("%s %s %s\n", hash, time ? time : "", message ? message : ""); //printing the log 
        if (!parent) { //check if parent exists
            free(data);
            break;
        }
        strcpy(hash, parent); //this is the main part
        free(data);
    }
    return 0;
}
