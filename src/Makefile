CC := gcc
CFLAGS := -Wall -O2

all: list.o

run_naive_linked_list: naive_linked_list_driver
	./naive_linked_list_driver

run_indirect_pointer_linked_list: indirect_pointer_linked_list_driver
	./indirect_pointer_linked_list_driver

list.o: naive_linked_list.c list.h
	$(CC) $(CFLAGS) -c $< -o $@

naive_linked_list: naive_linked_list.c 
	$(CC) $(CFLAGS) $^ -o $@

indirect_pointer_linked_list: indirect_pointer_linked_list.c 
	$(CC) $(CFLAGS) $^ -o $@

clean:
	$(RM) naive_linked_list indirect_pointer_linked_list_driver naive_linked_list_driver list.o