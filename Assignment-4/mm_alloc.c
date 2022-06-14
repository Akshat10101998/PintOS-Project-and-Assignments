/*
 * mm_alloc.c
 */

#include "mm_alloc.h"

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

struct mem_block *ptrHead = NULL;

struct mem_block *expand_heap (struct mem_block *previous , size_t sz){
	void* temp = sbrk(sz + sizeof(mem_block));
	if(!(temp==(void *)-1)){
		struct mem_block *n = (struct mem_block *) temp;
		if(previous){
			previous->next = n;
    }
    else{
			ptrHead = n;
		}
		n->prev = previous;
		n->next = NULL;
		n->size = sz;
		n->b = temp + sizeof(mem_block);
		n->empty_status = 0;
		return n;
	}
	return NULL;
}

void* mm_malloc(size_t size) {
  //TODO: Implement malloc

  /*
  
  LinkedList Implementation: Each node hold information about the memory block.

  struct mem_block {
    size_t size; 
    struct mem_block *next; 
    struct mem_block *prev; 
    int empty_status; 
    void *b;       
 };

  */
  
  struct mem_block *head=ptrHead;     // initializing the head pointer to the head of the linked list.
	struct mem_block *prev=NULL;        // prev pointer is initialized to NULL
	while(head){
    // Find the memory block in the linkedlist which is empty (empty_status==1) and which can accomodate the required size
		if(head->empty_status == 1 && head->size >= size){
			mm_realloc(head->b,size); // allocated the memory space
			head->empty_status = 0;   // the block is occupied
			return head->b;           // return the pointer pointing to the memory block
      }
    else{
			prev = head;
			head = head->next;
		}
	}
	//If we exited the loop, we know that we didnt find a suitable space, and so we need to move the break pt.
	head=expand_heap(prev,size);
	if(!head){
		exit(EXIT_FAILURE);
	}
	return head->b;
  return NULL;
}

struct mem_block *block_to_get (void *a){
	struct mem_block *head=ptrHead;
	while(head){
		if(head->b == a){
			return head;
		}
		head=head->next;
	}
	//if we didn't able to find anything then return NULL
	return NULL;
}

void* copy_memory(struct mem_block *old_block, struct mem_block *new_block){
	if( old_block && new_block){
		char * o= (char *) old_block->b;
		char * n=(char *) new_block->b;
    int i=0;
    while (i<old_block->size){
      *(n+i) = *(o+i);
      i++;
    }
		return new_block->b;
	}
	return NULL;
}

void block_split (struct mem_block *block, size_t sz){
	if(block && sz >= sizeof(void *)){
		if(  block->size - sz  >= sizeof(mem_block) + sizeof(void*) ){
			struct mem_block *temp = (struct mem_block*) (block->b + sz);
			temp->next = block->next;
			(temp->next)->prev = temp;
			block->next = temp;
			temp->prev = block;
			temp->size = block->size - sz - sizeof(mem_block);
			block->size = sz;
			temp->b = block->b + sz + sizeof(mem_block);
			mm_free(temp->b);
		}
	}
}

void* mm_realloc(void* ptr, size_t size) {
  //TODO: Implement reallocation
  struct mem_block *current=block_to_get(ptr);
		if(current){
			if(size > current->size){
				void* temp_ptr=mm_malloc(size);
				struct mem_block *temp=block_to_get(temp_ptr);
				if(temp){
					temp_ptr=copy_memory(current,temp);
					mm_free(current->b);
					return temp_ptr;
				}
			}
      else if(size < current->size){
				block_split(current,size);
				return current->b;
			}
      else{
				return current->b;
			}
		}
		return NULL; 
}

struct mem_block *join(struct mem_block* block){
	if( (block->next)->empty_status == 1 ){
		block->size=block->size+sizeof(mem_block)+(block->next)->size;
		block->next=(block->next)->next;
		(block->next)->prev=block;
	}
	if( (block->prev)->empty_status ==1 ){
		(block->prev)->next = block->next;
		(block->next)->prev = block->prev;
		(block->prev)->empty_status = block->empty_status;
		(block->prev)->size = (block->prev)->size + sizeof(mem_block) + block->size;
		return block->prev;
	}
	return block;
}

void mm_free(void* ptr) {
  //TODO: Implement free
  struct mem_block *current=block_to_get(ptr);
	if(current){
		if(current->next==NULL){
			if(current->prev){
				(current->prev)->next=NULL;
			}else{
				ptrHead=NULL;
			}
			sbrk(-(current->size + sizeof(mem_block)));
		}else{
			current->empty_status=1;
			join(current);
		}
	}
}