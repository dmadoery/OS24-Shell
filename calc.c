#include <ctype.h> // isdigit()
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LOWEST_PRECEDENCE 4
/*
Precedence rules: https://en.cppreference.com/w/c/language/operator_precedence
Highest
1: ()
2: unary + and unary -
3: *, /
4: binary + and binary -
Lowest
*/

// maybe use long double instead of float. Is there atof-like function for (long) double?

// Math
float comp(char *operation);
float mult(char *operand1, char *operand2);
float divi(char *operand1, char *operand2);
float add(char *operand1, char *operand2);
float sub(char *operand1, char *operand2);

// Helper
char * substr(char *str, int start, int length);

float comp(char *operation) {
	int n = strlen(operation);
	for (int p = LOWEST_PRECEDENCE; p > 1; p--) {	// ignore () for now
		switch(p) {
			case 3:
				for (int i = n - 1; i >= 0; i--) {
					if(operation[i] == '*') {
						return mult(substr(operation, 0, i), substr(operation, i + 1, n - i - 1));
					} else if (operation[i] == '/') {
						return divi(substr(operation, 0, i), substr(operation, i + 1, n - i - 1));
					}
				}
				break;
			case 4: // must distinguish from unary + and unary -
				for (int i = n - 1; i >= 0; i--) {
					if(i > 0 && operation[i] == '+' && isdigit(operation[i-1])) { // distiguish from unary + (e.g., "+3*4")
						return add(substr(operation, 0, i), substr(operation, i + 1, n - i - 1));
					} else if (i > 0 && operation[i] == '-' && isdigit(operation[i-1])) { // distiguish from unary - (e.g., "-3*4")
						//return sub(substr(operation, 0, i), substr(operation, i, n - i)); // exception
						return sub(substr(operation, 0, i), substr(operation, i + 1, n - i - 1));
					}
				}
				break;
		}
	}
	// end of recursion if [+-]digit (e.g., "3.14", "+1", "-1" etc.
	float f = atof(operation);
	//printf("[comp] f: %f\n", f);
	return atof(operation);
}

float mult(char *operand1, char *operand2) {
	float result = comp(operand1) * comp(operand2);
	free(operand1);
	free(operand2);
	return result;
}

float divi(char *operand1, char *operand2) {
	float result = comp(operand1) / comp(operand2);
	free(operand1);
	free(operand2);
	return result;
}

float add(char *operand1, char *operand2) {
	float result = comp(operand1) + comp(operand2);
	free(operand1);
	free(operand2);
	return result;
}

float sub(char *operand1, char *operand2) {
	float result = comp(operand1) - comp(operand2);	// exception: comp("2-3-4) = comp("2") + comp("-3-4") = 2 + comp("-3") + comp("-4") = 2 + -3 + -4 = -5
	free(operand1);
	free(operand2);	
	return result;
}

// Helper functions (maybe move to separate helper.h file

//char *trim(char *str)

/* Returns a substring from str*/
char * substr(char *str, int start, int length) {
	char *substr_buffer;
	substr_buffer = (char *) malloc((length + 1) * sizeof(char));
	memcpy(substr_buffer, &str[start], length);
	substr_buffer[length] = '\0';
	//printf("[substr] substr_buffer: %s\n", substr_buffer);
	return 	substr_buffer;
}

/* Validates input */
int is_valid(char *input) {
	int n = strlen(input);
	for (int i = 0; i < n; i++) {
		if (!isdigit(input[i]) && input[i] != '+' && input[i] != '-' && input[i] != '*' && input[i] != '/' && input[i] != '.') {
			perror("ERROR[Calc is_valid]: invalid input\n");
			return 0;
		}
	}
	return 1;
}

/* remove space-characters from the input */
char * trim(char *input, const char *trim_char) {
	int n = strlen(input);
	char *output;
	output = (char *) malloc((n + 1) * sizeof(char));
	int j = 0;
	for (int i = 0; i < n; i++) {
		if (input[i] != *trim_char) {
			output[j++] = input[i];
		}
	}
	output[j] = '\0';
	return output;
}

/*
receives input in the format
	command = "./calc" = argv[0]
	flag = "-" = argv[1]
	inpu1 = "<operation>" = argv[2] e.g., 3+4*5 (no spaces because shell uses the space symbol to separate inputs)
	input2 = "-" = argv[3]
*/
int main(int argc, char **argv) {
	// verify input
	if (argc != 4 || *argv[1] != '-' || (*argv[2] == '-' && strlen(argv[2]) == 1) || *argv[3] != '-') {
		printf("Syntax error - expected format: calc <operation>\n"); // note: user input format differs from input format receivde from shell
		return -1;
	}
	char *operation = trim(argv[2], " "); // trim space-characters
	if (!is_valid(operation)) {
		return -1;
	}
	
	printf("%f\n", comp(argv[2]));
	free(operation);

	// testing	
	/*
	char* o1 = "3+4*5";
	float r1 = comp(o1);
	printf("%s = %f\n", o1, r1);
	char* o2 = "3*4+5";
	float r2 = comp(o2);
	printf("%s = %f\n", o2, r2);
	char* o3 = "32/8/4";
	float r3 = comp(o3);
	printf("%s = %f\n", o3, r3);
	char* o4 = "-1-2-3-4";
	float r4 = comp(o4);
	printf("%s = %f\n", o4, r4);
	char* o5 = "3*-4*5";
	float r5 = comp(o5);
	printf("%s = %f\n", o5, r5);
	char* o6 = "2*1000*3.14";
	float r6 = comp(o6);
	printf("%s = %f\n", o6, r6);
	char* o7 = "16*16*16/16/16/16";
	float r7 = comp(o7);
	printf("%s = %f\n", o7, r7);
	*/
}
