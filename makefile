# To run, enter: make all
# To force a recompilation (e.g., when changing cmd.h), enter make -B

all: browse calc cd cp ls md mf pwd pwd2 shell to_file view

browse: browse.c
	gcc browse.c -o browse

calc: calc.c
	gcc calc.c -o calc

cd: cd.c
	gcc cd.c -o cd

cp: cp.c
	gcc cp.c -o cp

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

to_file: to_file.c
	gcc to_file.c -o to_file

view: view.c
	gcc view.c -o view

