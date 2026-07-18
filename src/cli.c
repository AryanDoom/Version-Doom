#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cli.h" //contains all the functions/commands that we have right 
#include "repository.h"
#include "object.h"
#include "index.h"
#include "commit.h"
#include "hash.h"
#include "utils.h"
#include "status.h"
#include "cat.h"
#include "checkout.h"

int ensure_repo_context() { // func to make sure tthis directory is a repo
    if (!repo_exists(".")) {
        fprintf(stderr, "not a repository\n");
        return 1;
    }
    return 0;
}

void print_usage(const char *prog) { //func to print the usage of the cli commands
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "  %s init\n", prog);
    fprintf(stderr, "  %s hash-object [options] <file_name>\n", prog);
    fprintf(stderr, "  %s add <file_name>\n", prog);
    fprintf(stderr, "  %s commit -m <commit_message>\n", prog);
    fprintf(stderr, "  %s log\n", prog);
    fprintf(stderr, "  %s cat-object <hash_info>\n", prog);
    fprintf(stderr, "  %s status\n", prog);
    fprintf(stderr, "  %s checkout <commit-hash>\n", prog);
}

int run_cli(int argc, char **argv) {  //very simple command parser that check the args and does accordingly 
    if (argc < 2) { //basically the help command 
        print_usage(argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "init") == 0) {//run init 
        return cmd_init(); 
    }

    if (strcmp(argv[1], "hash-object") == 0) { // this is for hashin the object 
        if (argc < 3) {
            print_usage(argv[0]);
            return 1;
        }
        int write_object = 0; //all these ifs are js to find where the file name is and if -w is present or not
        int arg_index = 2;
        if (argc >= 4 && strcmp(argv[2], "-w") == 0) { //-w is for writing the object to the object store
            write_object = 1;
            arg_index = 3;
        }
        if (arg_index >= argc) { 
            print_usage(argv[0]);
            return 1;
        } //if no -w then the object is js hashed and printed to stdout
        return cmd_hash_object(argv[arg_index], write_object);
    }

    if (strcmp(argv[1], "add") == 0) { //add cmd
        if (argc < 3) { //cmd is wrong (no file name)
            print_usage(argv[0]);
            return 1;
        }
        if (ensure_repo_context() != 0) { //js check if file is there
            return 1;
        }
        return cmd_add(argv[2]); //returns the file name to the add func
    }

    if (strcmp(argv[1], "commit") == 0) { //commit cmd
        if (ensure_repo_context() != 0) {
            return 1;
        }
        const char *message = NULL; //make sure the reference to the commit msg is null
        for (int i = 2; i < argc; ++i) {
            if (strcmp(argv[i], "-m") == 0 && i + 1 < argc) {
                message = argv[++i];
                break; //take the message 
            }
        }
        if (!message) { //no msg
            print_usage(argv[0]);
            return 1;
        }
        return cmd_commit(message); //return with teh message 
    }

    if (strcmp(argv[1], "log") == 0) { //log cmd
        if (ensure_repo_context() != 0) { //this is to make sure the repo exist
            return 1;
        }
        return cmd_log();
    }

    if (strcmp(argv[1], "cat-object") == 0) { //cat object cmd
        if (argc < 3) { //complete the cmd with the has
            print_usage(argv[0]);
            return 1;
        }
        if (ensure_repo_context() != 0) {
            return 1;
        }
        return cmd_cat_object(argv[2]); // func call 
    }

    if (strcmp(argv[1], "status") == 0) { //status cmd 
        if (ensure_repo_context() != 0) {
            return 1;
        }
        return cmd_status();
    }

    if (strcmp(argv[1], "checkout") == 0) { //checkout cmd
        if (argc < 3) {
            print_usage(argv[0]);
            return 1;
        }
        if (ensure_repo_context() != 0) {
            return 1;
        }
        return cmd_checkout(argv[2]);
    }

    print_usage(argv[0]);
    return 1;
}
