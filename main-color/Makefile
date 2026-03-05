# Generated Makefile, Just a template. You can modify me

Var0 = clang

.DEFAULT_GOAL := Link
run:
	@echo "[INFO] Running commands..."

	@echo Causa-san
	
src/main.c.out: src/main.c
	@echo -e "[\e[1;32m Compiling file src/main.c 0.0%... \e[0m]"
	@ccache ${Var0} -c src/main.c -o build/main.c.out -Iinclude
Link: src/main.c.out
	@echo -e "[\e[1;32m Linking 100%... \e[0m]"

	@clang build/main.c.out -o build/main-color -lpng -lm
all:	run	src/main.c.out	Link
install:
	sudo mv build/main-color /usr/bin