#ifndef COMMIT_H
#define COMMIT_H

int cmd_commit(const char *message);
int cmd_log(void);
int cmd_checkout(const char *commit_hash);
int write_head(const char *hash);

#endif
