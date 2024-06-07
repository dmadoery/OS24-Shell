#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define URL_PREFIX "xdg-open http://"
#define DEFAULT_PAGE "www.google.com"

/*
receives input in the format
	command = "./browse" = argv[0]
	flag = "-" = argv[1]
	inpu1 = "[<web_page>]" = argv[2]
	input2 = "-" = argv[3]
*/
int main(int argc, char **argv) {
	if (argc != 4 || *argv[1] != '-' || *argv[3] != '-') {
		printf("Syntax error - expected format: browse [<web_page>]\n"); // note: user input format differs from input format receivde from shell
		return -1;
	}
	char *page;
	if (*argv[2] != '-') {
		page = argv[2];
	} else {
		page = DEFAULT_PAGE;
	}
	int m = strlen(URL_PREFIX);
	int n = strlen(page);
	char url[m + n + 1];
	for (int i = 0; i < m; i++) {
		url[i] = URL_PREFIX[i];
	}
	for (int i = 0; i < m; i++) {
		url[m + i] = page[i];
	}
	url[m + n] = '\0';
	system(url);
	return 0;
}
