# To run, enter: make all

all: calc cd ls md mf pwd pwd2 shell

calc: calc.c
	gcc calc.c -o calc

cd: cd.c
	gcc cd.c -o cd

ls: ls.c
	gcc ls.c -o ls

md: md.c
	gcc md.c -o md

mf: mf.c
	gcc mf.c -o mf

pwd: pwd.c
	gcc pwd.c -o pwd

pwd2: pwd2.c
	gcc pwd2.c -o pwd2

shell: shell.c
	gcc shell.c -o shell
