# Ce Makefile est là pour vous aider 
# Vous pouvez le modifier, ajouter des règles, en enlever ...
# Vous pouvez ne pas vous en servir, mais ce serait un tort

# Compilateur a utilliser
CC=gcc 

# Fichier à contruire
EXE=minishell

# Quelles options pour le compilateur ? 
CFLAGS=-Wall -Wextra -pedantic -O3 -g

# Options pour l'édition de liens
LDFLAGS=

# Les fichiers .o nécessaires pour contruire le fichier EXE :
# Ils sont obtenus à partir de tous les fichiers .c du répertoire auquel on enlève le programme de test
OBJECTS = libcmd.o minishell.o

all: $(EXE)

libcmd.o: SAVE/libcmd.o
	cp $^ .

$(EXE): $(OBJECTS)

clean:
	\rm -f *.o *~
	\rm -f $(EXE)
	\rm -f $(TEST_FILE)

archive: clean
	(cd .. ; tar cvf minishell-`whoami`.tar minishell)

help:
	@echo "Makefile for minishell."
	@echo "Targets:"
	@echo " all             Build the minishell"
	@echo " archive	 Archive the minishell"
	@echo " clean           Clean artifacts"
