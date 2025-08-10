#include <stdio.h>
#include <string.h>
#include "../../sqlite3.h"

static int print_row(void *unused, int argc, char **argv, char **azColName) {
    for (int i = 0; i < argc; i++) {
        printf("%s%s", argv[i] ? argv[i] : "NULL", (i < argc - 1) ? " | " : "");
    }
    printf("\n");
    return 0;
}

static void rstrip_newline(char *s) {
    if (!s) return;
    size_t n = strlen(s);
    while (n && (s[n-1] == '\n' || s[n-1] == '\r')) s[--n] = '\0';
}

int main(int argc, char **argv) {
    sqlite3 *db = NULL;
    char *err = NULL;
    char sql[2048];

    if (argc < 2) {
        fprintf(stderr, "Usage: %s DB_FILE\n", argv[0]);
        return 1;
    }
    if (sqlite3_open(argv[1], &db) != SQLITE_OK) {
        fprintf(stderr, "DB open failed: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }

    printf("Opened DB: %s\n", argv[1]);
    printf("Enter SQL (end with ';', type 'exit;' to quit)\n");

    while (1) {
        printf("sqlite> ");
        if (!fgets(sql, sizeof(sql), stdin)) break;
        rstrip_newline(sql);
        if (sql[0] == '\0') continue;      
        if (strcmp(sql, "exit;") == 0) break;

        int rc = sqlite3_exec(db, sql, print_row, NULL, &err);
        if (rc != SQLITE_OK) {
            fprintf(stderr, "SQL error: %s\n", err ? err : "(unknown)");
            sqlite3_free(err); err = NULL;
        } else {

            int changes = sqlite3_changes(db);
            if (changes > 0) {
                printf("(%d row%s affected)\n", changes, changes>1?"s":"");
            }
        }
    }

    sqlite3_close(db);
    return 0;
}
