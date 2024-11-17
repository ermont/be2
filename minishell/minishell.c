#include <stdio.h>
#include <stdlib.h>
#include "readcmd.h"
#include <stdbool.h>
#include <string.h>

int main(void) {
    bool fini= false;

    while (!fini) {
        printf("> ");
        string *cmd;
        if ((cmd= readcmd())) {
            if (cmd[0]) {
                if (strcmp(cmd[0], "exit") == 0) {
                    fini= true;
                    printf("Au revoir ...\n");
                } else {
                    // On peut traiter les commandes entrees au clavier
                    printf("commande : ");
                    printf("%s", cmd[0]);
                    (isBackground())?printf(" is backgrounded\n"):printf(" is foregrounded\n");
                }
            }
        }
    }
    return EXIT_SUCCESS;
}
