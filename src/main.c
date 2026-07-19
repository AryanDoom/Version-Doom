#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cli.h"

int main(int argc, char **argv) {
    return run_cli(argc, argv); //cli function call witht the give args
}



/*
TODO
duplicate index entries
read_object() null termination
HEAD semantics
checkout deleting removed files
checkout dirty-working-tree protection
real status
object typing
nested directory support
object integrity verification
also clear index after commiting 
*/