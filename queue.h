#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct node {
	char *dir;
	struct node *next;
	struct node *prev;
};

struct node *head = NULL;
struct node *tail = NULL;
int nr_of_elements = 0;

struct node * make_node(char *dir) {
	struct node *n = malloc(sizeof(struct node));
	n -> dir = strdup(dir);
	n -> next = NULL;
	n -> prev = NULL;
}

/* frees memory of both the node and dir (as strdup() also allocates memory) */
void destroy_node(struct node *n) {
	free(n -> dir);
	free(n);
}

/* adds n at tail */
void enqueue(struct node *n) {
	if (nr_of_elements == 0) {
		head = n;
	} else {
		n -> next = tail;
		n -> next -> prev = n;
	}
	tail = n;
	nr_of_elements++;
}

/* removes node at head (if any) */
struct node * dequeue() {
	if (nr_of_elements == 0) {
		return NULL;
	} else {
		struct node *r;
		r = head;
		if (nr_of_elements == 1) {
			head = NULL;
			tail = NULL;
		} else {
			head -> prev -> next = NULL;
			head = head -> prev;
		}
		nr_of_elements--;
		return r;
	}
}

int size() {
	return nr_of_elements;
}

/*
int main(int arc, char ***argv) {
	struct node *n1 = make_node("/home/");
	struct node *n2 = make_node("/home/huch/");
	printf("size: %d\n", size());
	enqueue(n1);
	enqueue(n2);
	printf("size: %d\n", size());
	struct node *r1 = dequeue();
	printf("dir: %s\n", r1 -> dir);
	struct node *r2 = dequeue();
	printf("dir: %s\n", r2 -> dir);
	struct node *r3 = dequeue();
	//printf("dir: %s\n", r3 -> dir);
	struct node *n3 = make_node("/root/");
	enqueue(n3);
	printf("size: %d\n", size());
	return 0;
}
*/
