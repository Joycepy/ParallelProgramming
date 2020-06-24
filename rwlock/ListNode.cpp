#include "ListNode.h"
#include <stdlib.h>
#include <stdio.h>
Node* findNode(PNode pHeader, int k);
int insertNode(PNode pHeader, PNode pNew, int k);
int deleteNode(PNode pHeader, int k);

bool rw_Insert(PNode head, rwl_t* rwlock, int value, int pos){
    rwl_wrlock(rwlock);
    PNode tmp = (PNode) malloc(sizeof(Node));
    tmp->data = value;
    int ans = insertNode(head, tmp, pos);
    rwl_unlock(rwlock);
    return (bool)ans;
}
bool rw_Find(PNode head, rwl_t* rwlock, int pos){
    rwl_rdlock(rwlock);
    Node* ans = findNode(head, pos);
    rwl_unlock(rwlock);
    if(ans == NULL) return false;
    return true;
}
bool rw_Delete(PNode head, rwl_t* rwlock, int pos){
    rwl_wrlock(rwlock);
    int ans = deleteNode(head, pos);
    rwl_unlock(rwlock);
    return (bool)ans;
}

Node* findNode(PNode pHeader, int k)
{
  PNode p = pHeader;
  int i = 0;
  //printf("findNode\n");
  while(p && (i < k))
  {
    p = p->next;
    i++;
  }
  if (p && (i == k))
    return p;
  // 1 >= k && pHeader->next!=NULL
  return NULL;
}

int insertNode(PNode pHeader, PNode pNew, int k)
{
  PNode p = NULL;
  //printf("insert\n");
  if ( 1 == k )
    p = pHeader;
  else
    p = findNode(pHeader, k-1);
  if (p)
  {
    pNew->next = p->next;
    p->next = pNew;
    return 1;
  }
  return 0;
}

int deleteNode(PNode pHeader, int k)
{
  PNode p = NULL;
  //printf("deleteNode\n");
  if (1 == k)
    p = pHeader;
  else
    p = findNode(pHeader, k-1);
  if (p && p->next)
  {
    PNode temp = p->next;
    p->next = temp->next;
    free(temp);
    return 1;
  }
  else
  {
    //printf("Not Found\n");
    return 0;
  }
}
