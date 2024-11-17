#ifndef __READCMD_H
#define __READCMD_H

#include <stdbool.h>

typedef char* string;
/**
 * @brief Lit une ligne de commande depuis l'entrée standard.
 * 
 */
string* readcmd(void);

/**
 * @brief Retourne si la commande est en arrière plan 
 */
bool isBackground(void);

#endif
