#include "rwlock.h"
#include <stdbool.h>
typedef struct node
{
  int data;
  struct node* next;
} Node, *PNode;

bool rw_Insert(PNode head, rwl_t* rwlock, int value, int pos);
bool rw_Find(PNode head, rwl_t* rwlock, int pos);
bool rw_Delete(PNode head, rwl_t* rwlock, int pos);
