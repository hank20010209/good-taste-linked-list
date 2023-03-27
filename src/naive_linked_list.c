#include "list.h"


List *init_list()
{
    List *l = (List *)malloc(sizeof(List));
    l->head = NULL;
    return l;
}

Node *find_node(List *list, int target)
{
    Node *temp = list->head;
    while(temp && temp->value != target) {
        temp = temp->next;
    }
    return temp;
}

void insert_tail(List *list, int value)
{
    Node *curr = list->head;
    Node *prev = NULL;

    Node* new = (Node *)malloc(sizeof(Node));
    new->next = NULL;
    new->value = value;

    while(curr) {
        prev = curr;
        curr = curr->next;
    }
    if(!prev)
        list->head = new;
    else
        prev->next = new;
}

void remove_node(List *list, Node *target)
{
    if(!target) return;
    Node *prev = NULL;
    Node *curr = list->head;

    while(curr != target) {
        prev = curr;
        curr = curr->next;
    }
    
    if(!prev)
        list->head = target->next;
    else
        prev->next = target->next;
}

void print_list(List *list)
{
    Node *temp = list->head;
    while(temp) {
        printf("%d ", temp->value);
        temp = temp->next;
    }
}