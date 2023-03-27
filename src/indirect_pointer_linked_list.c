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
    Node **indirect = &list->head;
    
    while (*indirect != target)
        indirect = &(*indirect)->next;
    *indirect = target->next;
}

void print_list(List *list)
{
    Node *temp = list->head;
    while(temp) {
        printf("%d ", temp->value);
        temp = temp->next;
    }
}

// int main(void)
// {
//     List *l = init_list();
//     insert_tail(l,1);
//     insert_tail(l,2);
//     insert_tail(l,3);
//     remove_node(l, find_node(l, 3));
//     print_list(l);
// }