# Travail préliminaire

Les fichiers nécessaires au déroulement du BE se trouvent sous Github.
Pour les récupérer, exécutez la commande :

    > git clone git@github.com:ermont/be2.git

Vous devriez obtenir un répertoire `be2` dont l’arborescence est la
suivante :
```bash
be2
├── README.md
├── exo1.c
└── minishell
    ├── Makefile
    ├── SAVE
    │   └── libcmd.o
    ├── minishell.c
    └── readcmd.h
```

L’exercice de la
section <a href="#sec:gestion" data-reference-type="ref"
data-reference="sec:gestion">2</a> permet de comprendre les intéractions
entre un processus fils et son processus parent. L’exercice de la
section <a href="#sec:be" data-reference-type="ref"
data-reference="sec:be">3</a> contient le BE à proprement parler. Le
travail réalisé dans cette partie devra être remis sous Moodle et sera
noté.

# Gestion des processus

On considère le programme suivant :

    int main(int argc, char *argv[]) {
        int tempsPere, tempsFils;
        pid_t pid_fork;

        tempsPere= 120;
        tempsFils= 60;

        pid_fork= fork();
        // bonne pratique : tester systématiquement le retour des primitives
        if (pid_fork == -1) {
            printf("Erreur fork\n");
            exit(1);
            /* par convention, renvoyer une valeur > 0 en cas d'erreur,
             * différente pour chaque cause d'erreur, ici 1 = erreur fork
             */
        }
        if (pid_fork == 0) {        /* fils */
            printf("fils: processus %d, de père %d et code du fork %d\n", 
                        getpid(), getppid(), pid_fork);
            sleep(tempsFils);
            printf("fin du fils\n");
            exit(EXIT_SUCCESS); 
            /*  bonne pratique : 
                terminer les processus par un exit explicite */
        }
        else {      /* père */
            printf("père: processus %d, de père %d et code du fork %d\n", 
                        getpid(), getppid(), pid_fork);
            sleep(tempsPere);
            printf("fin du père\n");        
        }
        return EXIT_SUCCESS; /* -> exit(EXIT_SUCCESS); pour le père */
    }

## Exécution et état des processus

1.  Compilez le programme en utilisant `gcc` :

        > gcc exo1.c -o exo1   

    Exécutez le : `./exo1` (Pourquoi faut-il ajouter `./` pour que cela
    fonctionne ?)

2.  Utilisez la commande `ps -fg` dans un autre terminal et indiquez
    l’état des processus (`S` : `Sleeping`, en attente ; `R` :
    `Running`, actif ; `T` : `sTopped`, terminé ; `Z` : `Zombie`, ...) :

    - après le lancement du programme ;

    - après la fin du processus fils.

3.  Modifiez le code en échangeant les valeurs de `tempsFils` et
    `tempsPere` et exécutez le programme. En utilisant la commande
    `ps -fg`, que constatez-vous lorsque le processus père a terminé ?

## Héritage des données

Modifiez le programme précédent avec :

- `variable= 10;` au début du code du fils ;

- `variable= 100;` au début du code du père.

Affichez `variable` à la fin du code du fils et à la fin du code du
père. Que constatez-vous ?

## Attente de la terminaison du fils

Remplacez la ligne `sleep(tempsPere)` pour que le processus attende la
terminaison du fils.

Exemple d’appel de la primitive `wait()` :

    int status;
    pid_t pidFils;
    if ( (pidFils= wait(&status)) != -1 ) {
        if (WIFEXITED(status)) {
            printf("Le processus fils %d s'est terminé avec le code %i\n", 
            pidFils, WEXITSTATUS(status));
        } else if (WIFSIGNALED(status) {
            printf("Le processus fils %d s'est terminé par le signal %i\n",
            pidFils, WTERMSIG(status));
        }
    }

1.  Quel est l’affichage du programme lorsque le processus se termine
    normalement (exécution de `exit`) ?

2.  Quel est l’affichage du programme si on exécute dans un autre
    terminal la commande :

        kill -INT num_pid_fils

    où `num_pid_fils` est le pid du fils obtenu grâce à la commande
    `ps`.

# BE

L’objectif du bureau d’étude est d’utiliser les différents appels
système vus en cours pour réaliser un `minishell`. Les fichiers
nécessaires sont disponibles dans le répertoire `minishell`.

#### Etape 1 (Testez le programme)

Compilez le programme en tapant `make` dans le répertoire de l’archive
et lancez le en tapant `./minishell`. Quand une commande est tapée, le
programme affiche (pour le moment) la commande et ses arguments. Pour
sortir, tapez `exit`.

#### Etape 2 (Lancement d’une commande)

Modifiez le code de manière à exécuter la commande saisie dans un
processus fils en utilisant la primitive `exec` vue en cours,
<https://moodle-n7.inp-toulouse.fr/pluginfile.php/152804/mod_resource/content/1/API_UNIX.pdf>

A ce stade, lorsque la commande est lancée, `minishell` se met
immédiatement en attente d’une nouvelle commande, sans attendre la
terminaison.

#### Etape 3 (Enchaînement séquentiel des commandes)

L’exécution des commandes étant faites dans un processus fils, enchaîner
des commandes consistent tout simplement à créer les fils dans la
boucle. Pour chaque fils créé, le père exécute la commande `wait` pour
attendre la terminaison de la commande en cours. L’enchaînement des
commandes suit donc les étapes suivantes :

1.  Création d’un processus fils ;

2.  Le processus fils lance la commande à l’aide de la primitive `exec`;

3.  Le processus père attend la terminaison de la commande.

La 2ème commandes tapées au clavier peut se lancer à son tour en suivant
ces différentes étapes. Modifiez votre code afin qu’il attende la fin de
la dernière commande lancée avant de passer à la lecture de la ligne
suivante.

#### Etape 4 (Lancement de commandes en tâche de fond)

Lorsque l’utilisateur ajoute le caractère & après la commande, celle-ci
s’exécutera en tâche de fond, c’est-à-dire le processus père n’attend
pas sa terminaison :

    > sleep 10 &

Dans le programme, `isBackgrounded()` nous indique que & a été
positionné. La commande en avant-plan sera donc celle pour laquelle
`isBackgrounded` est faux. Dans ce cas-là, le processus père doit
attendre sa terminaison.

Exécutez la suite de commandes :

    > sleep 10 &
    > sleep 50

Dans `minishell.c`, le processus père exécute :

    if (!isBackgrounded()) {
        wait(NULL);  // on ne souhaite pas récupérer 
                     // le compte-rendu de terminaison du fils
    }

Compléter votre code pour offrir cette possibilité.  

<div class="question">

**Question 1**. *Pourquoi l’affichage du caractère $`>`$ s’effectue-t-il
après 10s ? Si vous avez ce genre de comportement, modifiez le code pour
l’éviter.*

</div>

#### Etape 5 (Attendre la terminaison du dernier fils lancé)

Nous allons utiliser la fonction `waitpid`.

    #include <sys/types.h>
    #include <sys/wait.h>

    pid_t waitpid(pid_t pid, int *status, int options);    

La primitive `waitpid` a un comportement similaire à celui de `wait`. Le
premier argument `pid_t pid` peut prendre l’une des valeurs suivantes :

- -1 : n’importe quel processus fils ;

- $`>0`$ : le numéro du processus fils attendu.

Comme pour `wait`, le code de retour est -1 en cas d’erreur ou le pid du
processus terminé. Le status est le même que celui obtenu avec la
primitive `wait`. Les options permettent de gérer les changements d’état
des processus. La valeur est un ou logique entre zéro ou plusieurs parmi
les constantes :

- `WNOHANG` : retour immédiat même si aucun changement d’état (dans ce
  cas le retour vaut 0) ;

- `WUNTRACED` : retour si un fils a été suspendu ;

- `WCONTINUED` : retour si un fils suspendu a été relancé par le signal
  `SIGCONT`.

#### Remarque

`waitpid(-1, &status, 0)` est équivalent à `wait(&status)`.

<div class="question">

**Question 2**. *Remplacez l’attente de processus fils réalisée avec
`wait` par une attente du processus en avant-plan avec la primitive
`waitpid` (si vous ne l’avez pas déjà fait ou déjà utilisé dans ce cas).
Que remarquez-vous sur l’état des processus exécutant les commandes en
arrière-plan lorsqu’elles se terminent (utilisez `ps -fg`) ?*

</div>

#### Etape 6 (Traitement du signal `SIGCHLD`)

Lorsqu’un processus fils change d’état, le processus père reçoit un
signal `SIGCHLD` de la part du système. Ce signal a pour but de
débloquer le processus père en attente de terminaison du fils via la
commande PID. Ajoutez un traitement à la réception du signal `SIGCHLD`
qui indique qu’un processus fils vient de terminer. Pour cela, utilisez
la primitive :  
`int sigaction(int sig, const struct sigaction *newaction, struct sigaction *oldaction);`  
Pour rappel, les différents champs de `struct sigaction` sont :

- `void (*sa_handler)(int)` est le traitement associé au signal. La
  procédure de traitement (le handler) est donc de la forme :
  `void traitement(int sig)` où `sig` est le signal qui a provoqué le
  lancement du traitement.

- `sigset_t sa_mask` est le masque des signaux lorsque le signal est
  reçu. Initialisation de cet ensemble via la primitive
  `void sigemptyset(sigset_t *set)`.

- `int sa_flags` est un ou logique de 0 ou plusieurs options. Ici, nous
  utiliserons l’option `SA_RESTART` (au lieu de 0).

Dans le fichier `Makefile`, enlevez l’option de compilation `-std=c11`
de la variable `CFLAGS`. Testez le programme en utilisant des processus
en avant-plan et en arrière-plan (par exemple `sleep 10 &`). A ce stade,
le `minishell` affiche un message lorsqu’un processus termine qu’il soit
en avant-plan ou en arrière plan. Il est alors possible d’attendre la
terminaison des fils à la fois pour les processus en avant-plan et en
arrière-plan.

#### Etape 7 (Utilisation de `SIGCHLD` pour traiter la terminaison des processus fils)

Le traitement du signal `SIGCHLD` doit être capable de récupérer le
`pid` ainsi que le `status` du processus qui vient de terminer. Pour
cela, il est possible d’utiliser `waitpid` avec les options
`WNOHANG|WUNTRACED|WCONTINUED` qui rendent la commande non bloquante.
Modifiez le programme pour traiter la terminaison de **tous** les
processus par l’utilisation de la primitive `waitpid` et affichez son
code de retour, qui correspond au `pid` du processus qui vient de
terminer. Pour rappel, `waitpid` retourne -1 si aucun processus
n’existe.

#### Etape 8 (Attendre un signal : `pause`)

Puisque la terminaison des processus se fait dans le traitement du
signal `SIGCHLD`, attendre la terminaison du fils en avant-plan revient
à attendre le signal `SIGCHLD`. Modifiez le programme pour utiliser
`void pause()` lorsque le processus est en avant-plan. Testez avec :

    > sleep 10 &
    > sleep 50

Que constatez-vous ?

#### Etape 9 (Suspension et reprise d’un processus en arrière-plan)

Testez l’envoi des signaux `SIGSTOP` et `SIGCONT` vers un processus en
arrière-plan. Dans quel état se trouve ce processus après lancement de
chaque signal ?

#### Etape 10 (Affichage d’un message indiquant le signal reçu)

Lorsqu’un processus fils change d’état, le processus père reçoit un
signal `SIGCHLD`. Modifiez le code pour afficher un message lorsque le
processus est terminé, suspendu ou repris. Pour identifier les
différents cas à étudier, utilisez les macros fournies par l’API Unix :
`WIFEXITED(status)`, `WIFSIGNALED(status` (que vous connaissez déjà),
`WIFSTOPPED(status)` qui vaut vrai si le processus a été suspendu par le
signal `SIGSTOP` et `SIGCONTINUED(status)` qui vaut vrai si le processus
a été repris en utilisant le signal `SIGCONT`. Bien entendu, testez ces
différents cas.

#### Etape 11 (Test de la frappe au clavier de `ctrl-C` et `ctrl-Z`)

Lorsqu’on appuie sur `ctrl-C` (respectivement `ctrl-Z`), le signal
`SIGINT` (respectivement `SIGTSTP`) est envoyé au processus en cours. La
terminaison (respectivement la suspension) du processus `minishell` est
alors demandée. Testez ce comportement.  
Que se passe-t-il lorsque le `minishell` a lancé :

- une commande en avant plan, par exemple `sleep 50` ?

- une commande en arrière plan, par exemple `sleep 50 &` ?

#### Etape 12 (Gestion de la frappe au clavier de `ctrl-C` et `ctrl-Z`)

Comme nous avons pu le remarquer, lorsqu’on frappe `ctrl-C` (ou
`ctrl-Z`), les signaux sont transmis à la fois à `minishell` et à
l’ensemble de ses fils. Nous allons alors traiter la non-terminaison ou
la non-suspension du processus `minishell`.

Ignorer un signal revient à associer le traitement `SIG_IGN` à ce
signal. Pour reprendre le traitement par défaut, il suffit d’associer le
traitement `SIG_DFL`. Modifiez le code de manière à ignorer la réception
des signaux `ctrl-C` et `ctrl-Z` dans le processus père. Testez ensuite
la frappe de `ctrl-C` puis de `ctrl-Z`, avec un processus en arrière
plan et un processus en avant plan.

#### Etape 13 (Détacher les processus fils en arrière plan)

A ce stade, le processus `minishell` n’est plus sensible à la frappe sur
les touches `ctrl-C` et `ctrl-Z` mais tous les processus fils reçoivent
les signaux `SIGINT` et `SIGDFL`. Or, lors de la frappe, seul le
processus en avant plan doit recevoir le signal. En réalité, les signaux
sont transmis à tous les processus du même groupe que le processus
`minishell`. La solution que nous proposons est de mettre les processus
en arrière plan dans un autre groupe de processus. Nous utiliserons ici
la primitive `int setpgrp()` qui associe une nouveau groupe au processus
appelant. Testez une dernière fois, la frappe de `ctrl-C` puis de
`ctrl-Z`, avec un processus en arrière plan et un processus en avant
plan.

#### Etape 14 (Rendu)

Archivez votre travail via la commande `make archive`. Le résultat est
un fichier nommé `minishell-votreidentifiant.tar`. Chargez ce fichier
dans la section rendu sous Moodle, dans la zone qui correspond à votre
groupe de TD.
