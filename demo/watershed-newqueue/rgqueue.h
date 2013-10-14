#ifndef _RGQUEUE_H_
#define _RGQUEUE_H_

#include "common.h"
#include "gqueue.h"

/* Defines:
   MINVALUE   0    // define queue to remove node with minimum value
   MAXVALUE   1    // define queue to remove node with maximum value
   FIFOBREAK 0     // define queue to solve ambiguity by FIFO
   LIFOBREAK 1     // define queue to solve ambiguity by LIFO
   QSIZE     32768
   
   SetRemovalPolicy(a,b) a->C.removal_policy=b 
*/

typedef struct _rgqnode { 
  int  parent; /* parent node */
  int  left;   /* left child node */
  int  right;  /* right child node */
  char color;  /* WHITE=0, GRAY=1, BLACK=2 */ 
} RGQNode;

typedef struct _rgheaps {
  RGQNode *elem;   /* all possible heaps of the circular queue */
  int      nelems; /* total number of elements */
  float   *value;  /* the value of the nodes in the graph */
} RGHeaps;

typedef struct _rgcircularqueue { 
  int  *first;     /* list of the first element of each heap */
  int  *last;      /* list of the last  element of each heap */
  int   nbuckets;  /* number of buckets in the circular queue */
  float minvalue;  /* minimum value of a node in queue */
  float maxvalue;  /* maximum value of a node in queue */
  char  removal_policy; /* 0 is MINVALUE and 1 is MAXVALUE */
} RGCircularQueue;

typedef struct _rgqueue { /* Priority queue with real-valued priorities as
                           proposed by A. Falcao and C. Luengo */
  RGCircularQueue C;
  RGHeaps H;
} RGQueue;

RGQueue *CreateRGQueue(int nbuckets, int nelems, float *value);
void     DestroyRGQueue(RGQueue **Q);
void     ResetRGQueue(RGQueue *Q);
int      EmptyRGQueue(RGQueue *Q);
void     InsertRGQueue(RGQueue *Q, int elem);
int      RemoveRGQueue(RGQueue *Q);
void     UpdateRGQueueElem(RGQueue *Q, int elem, float value);
void     PrintHeap(RGQueue *Q, int elem);


#endif
