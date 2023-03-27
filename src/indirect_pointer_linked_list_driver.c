#include "indirect_pointer_linked_list.c"

int main(void)
{
    List *l = init_list();
    insert_tail(l,1);
    insert_tail(l,2);
    insert_tail(l,3);
    remove_node(l, find_node(l, 3));
    printf("%s", "This is indirect pointer linked list test\n");
    print_list(l);
}
