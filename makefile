# To run, enter: make all

all: cd ls md pwd pwd2 shell

cd: cd.c
	gcc cd.c -o cd

ls: ls.c
	gcc ls.c -o ls

md: md.c
	gcc md.c -o md

pwd: pwd.c
	gcc pwd.c -o pwd

pwd2: pwd2.c
	gcc pwd2.c -o pwd2

shell: shell.c
	gcc shell.c -o shell
