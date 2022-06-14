/*
 * Implementation of the word_count interface using Pintos lists.
 *
 * You may modify this file, and are expected to modify it.
 */

/*
 * Copyright © 2021 University of California, Berkeley
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef PINTOS_LIST
#error "PINTOS_LIST must be #define'd when compiling word_count_l.c"
#endif

#include "word_count.h"

void init_words(word_count_list_t* wclist) { /* TODO */

/*
From list.h, we find out about List initialization.

Example of how a list can be initialized:

A list may be initialized by calling list_init():

       struct list my_list;
       list_init (&my_list);
*/

list_init(wclist);

}

size_t len_words(word_count_list_t* wclist) { /* TODO */
  
  /* 
  size_t is a type (unsigned long long int) used to repesent size of objects in bytes.
  it is guaranteed to big enough to contain  the size of the biggest object the
  host system can handle
  
  Reference : https://www.geeksforgeeks.org/size_t-data-type-c-language/
  */
  
  size_t num_of_words = 0;   // Initializing the num_of_words to store the count of words in the wclist.
  struct list_elem *pointer; // initialized a pointer to iterate over the doubly linked-list elements
                            // from list.h, struct list_elem represents the doubly linked-list elements
  struct list_elem *tail; //similarly, initialized the a pointer to the tail to the doubly linked-list elements of wclist
                          // so that when we reached the tail of the wclist we stop the iteration.
  
  /*
  wclist - it is a pointer to the doubly linked list
  From list.h, we obtained list_begin(), in which if we pass wclist, we will get the pointer to the head node of doubly linked list.
  similarly, list_end() will give the tail node of double linked list and,
  on passing the pointer to current element in list_next(), it's going to return a pointer to the next element.
  */
  pointer = list_begin(wclist);   //Obtained the head node
  tail = list_end(wclist);   //Obtained the tail node
  
  while (pointer != tail) //Run the loop till the end. Or till when pointer reaches the end of the tail
  {
    num_of_words++; 
    pointer = list_next(pointer); // updating the current pointer to the next pointer. Passed the previous pointer to list_next to obtain the next pointer
  }
  return num_of_words;
}

word_count_t* find_word(word_count_list_t* wclist, char* word) { /* TODO */
  /*
  Definitions of word_count_t and word_count_list_t:
  ==================================================
  
  #ifdef PINTOS_LIST

        #include "list.h"
        typedef struct word_count {
          char* word;
          int count;
          struct list_elem elem;
        } word_count_t;

      #ifdef PTHREADS

        #include <pthread.h>
        typedef struct word_count_list {
          struct list lst;
          pthread_mutex_t lock;
        } word_count_list_t;
      
      #else  
          typedef struct list word_count_list_t;
      #endif 

  #else  

      typedef struct word_count {
        char* word;
        int count;
        struct word_count* next;
      } word_count_t;

      typedef word_count_t* word_count_list_t;
  
  #endif 


  Explanation of list_entry():
  ============================
  
  #define list_entry(LIST_ELEM, STRUCT, MEMBER) ((STRUCT*)((uint8_t*)&(LIST_ELEM)->next - offsetof(STRUCT, MEMBER.next)))

    1. Here first it finds the address of (LIST_ELE)->next
    2. Then, the size of items till MEMBER.next is substracted from the above found adress using offset function in c
    3. adress is then type casted into unint8_t
    4. A struct pointer points to the adress which is the adress of the parent struct.
  */

  struct list_elem *pointer;
  struct list_elem *tail;
  pointer = list_begin(wclist);
  tail = list_end(wclist);
  while (pointer!=tail){
    word_count_t *wc = list_entry(pointer, word_count_t, elem);  // elem is the item in the struct word_count of type struct list_elem
                                                               // we have used offset function in C to determine the address of the parent.
    if (strcmp(word, wc->word)==0){                     //Comparing the word in the current word_count_t item to the given word using strcmp.
      return wc;
    }
    pointer = list_next(pointer);
  }
  return NULL;
}

word_count_t* add_word(word_count_list_t* wclist, char* word) {
  /* Explanations
  =================================================================================
  Explanation of list_insert():
  ==============================

      Function Definiton:
        void list_insert(struct list_elem* before, struct list_elem* elem) {
                  ASSERT(is_interior(before) || is_tail(before));
                  ASSERT(elem != NULL);

                  elem->prev = before->prev;
                  elem->next = before;
                  before->prev->next = elem;
                  before->prev = elem;
                }

    
      A list with two elements in it looks like this:


        +------+     +-------+     +-------+     +------+
    <---| head |<--->|   1   |<--->|   2   |<--->| tail |<--->
        +------+     +-------+     +-------+     +------+
  
  
    Inserting a new element:
      
      else if ((wc = malloc(sizeof(word_count_t))) != NULL) 
        {
          wc->word = word;
          wc->count = 1;
          struct list_elem *e = list_begin(wclist);
          e = list_next(e);
          list_insert(e, &wc->elem); 
                             
        }

        1. We need to pass the new element created and a before element
           Here, the new element created is the one which pointer wc points at and before element is head.next.next (item 2 in the diagram)
        
        2. Necessary rearrangements of prev and next pointer as shown in the above code, to insert the new element between 1 and 2.

  
        +------+     +-------+     +------------+      +-------+     +-------+
    <---| head |<--->|   1   |<--->|new element |<---> |   2   |<--->| tail  |<--->
        +------+     +-------+     +------------+      +-------+     +-------+
  ====================================================================================
  */

  word_count_t *wc = find_word(wclist, word);     //finding the pointer to the word_count_t struct which contains that particular word.
                  
  if (wc != NULL)  // if that word is present.
  { 
    wc->count += 1; // then increase the word count by 1
  } 
  else if ((wc = malloc(sizeof(word_count_t))) != NULL)  // else dynamically allocating memory for the new word_count_t struct
  {
    wc->word = word;
    wc->count = 1;
    struct list_elem *e = list_begin(wclist);
    list_insert(e, &wc->elem); // list insert function insert an element between the head and the second element. For more explanation,
                               // check the comments in the begginning of this function.
  }
  return wc;
}

void fprint_words(word_count_list_t* wclist, FILE* outfile) { /* TODO */
  struct list_elem *pointer;
  struct list_elem *tail;
  pointer = list_begin(wclist); //head pointer of the doubly linked list wclist
  tail = list_end(wclist);      //tail pointer of the doubly linked list wclist  
  // Traversing the doubly linked list
  while (pointer!=tail){
    word_count_t *wc = list_entry(pointer, word_count_t, elem); //getting the parent struct of the pointer of type list_elem
    fprintf(outfile, "%8d\t%s\n", wc->count, wc->word);   // printing out the details from the struct word_count_t. Word count and the char word inside that respective struct.

    pointer = list_next(pointer); //moving on to the next pointer
  }
}

static bool less_list(const struct list_elem* ewc1, const struct list_elem* ewc2, void* aux) { /* TODO */
  /*
  // Auxiliary data structure is a kind of helper data structure which we might use to 
  // solve a given problem and is terminated after the problem is solved.
  // It's taking up the extra space for temporary period of time

  // Reference: https://stackoverflow.com/questions/48615697/what-are-auxiliary-data-structures


   from list.h file, the definiton of less_list,
  Compares the value of two list elements A and B, given
   auxiliary data AUX.  Returns true if A is less than B, or
   false if A is greater than or equal to B.

   And this function is of return type static, so that the function inside is accessible to other functions.
  */
  
  bool (*less)(const word_count_t *, const word_count_t *) = aux;
  word_count_t *wc1 = list_entry(ewc1, word_count_t, elem);
  word_count_t *wc2 = list_entry(ewc2, word_count_t, elem);
  return less(wc1, wc2);

}

void wordcount_sort(word_count_list_t* wclist,
                    bool less(const word_count_t*, const word_count_t*)) {
  
  list_sort(wclist, less_list, less);
}
