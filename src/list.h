#ifndef LIST_H
#define LIST_H

#include <stdio.h>
#include <stdlib.h>

typedef struct Node
{
    int value;
    struct Node *next;
} Node;

typedef struct List
{
    Node *head;
} List;

List *init_list();
Node *find_node(List *, int);
void insert_tail(List *, int);
void remove_node(List *, Node *);
void print_list(List *);
#endif