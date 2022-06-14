#include <stdbool.h>

#ifndef VM_SWAP_H
#define VM_SWAP_H 1

struct page;
void initialize_swap (void);
void swap_page_in (struct virtual_page *);
bool swap_page_out (struct virtual_page *);

#endif /* vm/swap.h */
