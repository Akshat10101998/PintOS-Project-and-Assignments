#ifndef VM_FRAME_H
#define VM_FRAME_H
#include "threads/synch.h"
#include <stdbool.h>

void initialize_frame (void);
void free_frame (struct frame *);
void unlock_frame (struct frame *);
struct frame *frame_allocation_lock (struct virtual_page *);
void lock_frame (struct virtual_page *);

struct frame 
  {
    struct lock lock;          
    void *base;               
    struct virtual_page *virtual_page;         
  };

#endif 