/*
  Copyright (C) <2003> <Alexandre Xavier Falcão>
  Copyright (C) <2012> <Cris Luengo>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

  please see full copyright in COPYING file.
  -------------------------------------------------------------------------
  written by Cris Luengo <cris@cb.uu.se>, August 22, 2012
  based on code by A.X. Falcão <afalcao@ic.unicamp.br>, May 13th 2007

  This program is a collection of functions to create, destroy, and
  manipulate a priority queue. This particular priority queue is not
  stable.

  A priority queue Q consists of two data structures: a circular
  queue C and a table H that encodes all possible heaps.

  Q requires that the maximum possible increment along the paths be a
  non-negative value less than the number of buckets in C.

  Q->C.first[i] gives the top element of heap i.
  Q->C.last[i]  gives the bottom element of heap i.
  Q->C.nbuckets gives the number of buckets in C.
  Q->C.minvalue gives the minimum value of a node in queue.
  Q->C.maxvalue gives the maximum value of a node in queue.
  Q->C.removal_policy gives the MINVALUE or MAXVALUE removal policy

  All possible heaps are represented in H. Each bucket i contains a heap
  for the priority values >=i and <i+1.

  Q->H.elem[i].parent: the parent element of i
  Q->H.elem[i].left:   the left child of i
  Q->H.elem[i].right:  the right child of i
  Q->H.elem[i].color:  the color of i (WHITE=never inserted, GRAY=inserted,
  BLACK=removed)
  Q->H.nelems: gives the total number of elements that can be
  inserted in Q (It is usually the number of pixels in a given image
  or the number of nodes in a graph)
  Q->H.value[i]: gives the priority of element i in the graph.

  Insertions and updates are done in O(log(N/K)), where K is the number
  of buckets.
  Removal may take O(K+log(N/K))
*/

#include "rgqueue.h"
#include <assert.h>

RGQueue *CreateRGQueue(int nbuckets, int nelems, float *value)
{
    RGQueue *Q=NULL;

    Q = (RGQueue *) malloc(1*sizeof(RGQueue));
    if (Q == NULL)
        Error(MSG1,"CreateRGQueue");

    Q->C.first    = (int *)malloc((nbuckets) * sizeof(int));
    Q->C.heapsize = (int *)malloc((nbuckets) * sizeof(int));
    if ( (Q->C.first == NULL) || (Q->C.heapsize == NULL) )
        Error(MSG1,"CreateRGQueue");
    Q->C.nbuckets = nbuckets;

    Q->H.elem   = (RGQNode *)malloc(nelems*sizeof(RGQNode));
    if (Q->H.elem == NULL)
        Error(MSG1,"CreateRGQueue");
    Q->H.nelems = nelems;
    Q->H.value  = value;

    ResetRGQueue(Q);

    return(Q);
}

void ResetRGQueue(RGQueue *Q)
{
    int i;

    Q->C.minvalue = INT_MAX;
    Q->C.maxvalue = INT_MIN;
    SetRemovalPolicy(Q,MINVALUE);
    for (i=0; i < Q->C.nbuckets; i++)
    {
        Q->C.first[i]=NIL;
        Q->C.heapsize[i]=0;
    }
    for (i=0; i < Q->H.nelems; i++)
    {
        Q->H.elem[i].parent = Q->H.elem[i].left = Q->H.elem[i].right = NIL;
        Q->H.elem[i].color = WHITE;
    }

}

void DestroyRGQueue(RGQueue **Q)
{
    RGQueue *aux;

    aux = *Q;
    if (aux != NULL)
    {
        if (aux->C.first    != NULL) free(aux->C.first);
        if (aux->C.heapsize != NULL) free(aux->C.heapsize);
        if (aux->H.elem     != NULL) free(aux->H.elem);
        free(aux);
        *Q = NULL;
    }
}

int FindHeapElem(RGQueue *Q, int root, int elem_pos)
{
   int elem, parent, parent_pos;
   if (elem_pos==0)
      elem = root;
   else {
      parent_pos = (elem_pos-1)/2;
      parent = FindHeapElem(Q, root, parent_pos);
      if (2*parent_pos+1 == elem_pos)
         elem = Q->H.elem[parent].left;
      else
         elem = Q->H.elem[parent].right;
   }
   return elem;
}

void SwapParentChildInHeap(RGQueue *Q, int bucket, int parent, int child)
{
   int parent_left, parent_right, parent_parent,    parent_isleftchild=0;
   int child_left,  child_right,  /*child_parent,*/ child_isleftchild =0;

   assert (HeapIsCorrect(Q, bucket));

   /* get all pointers for safekeeping */
   parent_left     = Q->H.elem[parent].left;
   parent_right    = Q->H.elem[parent].right;
   parent_parent   = Q->H.elem[parent].parent;
   if ((parent_parent!=NIL) && (Q->H.elem[parent_parent].left==parent))
      parent_isleftchild = 1;
   child_left      = Q->H.elem[child].left;
   child_right     = Q->H.elem[child].right;
   assert (Q->H.elem[child].parent==parent);
   if (Q->H.elem[parent].left==child)
      child_isleftchild = 1;

   /* swap pointers between parent and child */
   Q->H.elem[parent].parent = child;
   Q->H.elem[parent].left   = child_left;
   Q->H.elem[parent].right  = child_right;
   Q->H.elem[child].parent  = parent_parent;
   if (child_isleftchild)
   {
      Q->H.elem[child].left    = parent;
      Q->H.elem[child].right   = parent_right;
      if (parent_right!=NIL)
         Q->H.elem[parent_right].parent = child;
   }
   else
   {
      Q->H.elem[child].left    = parent_left;
      Q->H.elem[child].right   = parent;
      assert(parent_left!=NIL);
         Q->H.elem[parent_left].parent  = child;
   }

   /* update parent pointers for the new children of parent (become child) */
   if (child_left!=NIL)
      Q->H.elem[child_left].parent  = parent;
   if (child_right!=NIL)
      Q->H.elem[child_right].parent = parent;

   /* update child pointer for the new parent of child (became parent) */
   if (parent_parent==NIL)
      Q->C.first[bucket] = child;
   else
      if (parent_isleftchild)
         Q->H.elem[parent_parent].left  = child;
      else
         Q->H.elem[parent_parent].right = child;

   assert (HeapIsCorrect(Q, bucket));

}

void MoveUpHeap(RGQueue *Q, int bucket, int elem)
{
   int parent = Q->H.elem[elem].parent;
   if (Q->C.removal_policy == MINVALUE)
   {
      while ((parent != NIL) && (Q->H.value[elem] < Q->H.value[parent]))
      {
         SwapParentChildInHeap(Q, bucket, parent, elem);
         parent = Q->H.elem[elem].parent;
      }
   }
   else /* removal_policy == MAXVALUE */
   {
      while ((parent != NIL) && (Q->H.value[elem] > Q->H.value[parent]))
      {
         SwapParentChildInHeap(Q, bucket, parent, elem);
         parent = Q->H.elem[elem].parent;
      }
   }
}

void MoveDownHeap(RGQueue *Q, int bucket, int elem)
{
   int child, left, right;
   if (Q->C.removal_policy == MINVALUE)
   {
      left = Q->H.elem[elem].left;
      right = Q->H.elem[elem].right;
      while (left != NIL)
      {
         child = left;
         if ((right != NIL) && (Q->H.value[right] < Q->H.value[left]))
            child = right;
         if (Q->H.value[child] < Q->H.value[elem])
         {
            SwapParentChildInHeap(Q, bucket, elem, child);
            left = Q->H.elem[elem].left;
            right = Q->H.elem[elem].right;
         }
         else break;
      }
   }
   else
   {
      left = Q->H.elem[elem].left;
      right = Q->H.elem[elem].right;
      while (left != NIL)
      {
         child = left;
         if ((right != NIL) && (Q->H.value[right] > Q->H.value[left]))
            child = right;
         if (Q->H.value[child] > Q->H.value[elem])
         {
            SwapParentChildInHeap(Q, bucket, elem, child);
            left = Q->H.elem[elem].left;
            right = Q->H.elem[elem].right;
         }
         else break;
      }
   }
}

void InsertRGQueue(RGQueue **Q, int elem)
{
    int bucket, parent;
    float minvalue=(*Q)->C.minvalue;
    float maxvalue=(*Q)->C.maxvalue;

    if ((*Q)->H.value[elem] < minvalue)
        minvalue = (*Q)->H.value[elem];
    if ((*Q)->H.value[elem] > maxvalue)
        maxvalue = (*Q)->H.value[elem];
    if ((int)(maxvalue-minvalue) >= ((*Q)->C.nbuckets+1))
    {
        Error("Inserted value exceeds maximum range of queue.","InsertRGQueue");;
    }
    if ((*Q)->C.removal_policy==MINVALUE)
    {
        bucket=((int)(*Q)->H.value[elem])%(*Q)->C.nbuckets;
    }
    else
    {
        bucket=(*Q)->C.nbuckets-1-(((int)(*Q)->H.value[elem])%(*Q)->C.nbuckets);
    }
    assert((bucket>=0) && (bucket<(*Q)->C.nbuckets));
    (*Q)->C.minvalue = minvalue;
    (*Q)->C.maxvalue = maxvalue;

    (*Q)->H.elem[elem].left   = NIL;
    (*Q)->H.elem[elem].right  = NIL;
    (*Q)->C.heapsize[bucket]++;
    (*Q)->H.elem[elem].color = GRAY;
    if ((*Q)->C.first[bucket] == NIL)
    {
        (*Q)->C.first[bucket]     = elem;
        (*Q)->H.elem[elem].parent = NIL;
    }
    else
    {
        parent = FindHeapElem(*Q, (*Q)->C.first[bucket], ((*Q)->C.heapsize[bucket]-2)/2);
        assert(parent>=0);
        if ((*Q)->H.elem[parent].left == NIL)
            (*Q)->H.elem[parent].left = elem;
        else
            (*Q)->H.elem[parent].right = elem;
        (*Q)->H.elem[elem].parent = parent;
        MoveUpHeap(*Q, bucket, elem);
    }
}

int RemoveRGQueue(RGQueue *Q)
{
    int elem, last, current, parent;

    if (Q->C.removal_policy==MINVALUE)
        current=((int)Q->C.minvalue)%Q->C.nbuckets;
    else
        current=Q->C.nbuckets-1-(((int)Q->C.maxvalue)%Q->C.nbuckets);
    assert((current>=0) && (current<Q->C.nbuckets));

    if (Q->C.first[current] == NIL)
    {
        last = current;

        current = (current + 1) % (Q->C.nbuckets);

        while ((Q->C.first[current] == NIL) && (current != last))
        {
            current = (current + 1) % (Q->C.nbuckets);
        }

        if (Q->C.first[current] != NIL)
        {
            if (Q->C.removal_policy==MINVALUE)
                Q->C.minvalue = Q->H.value[Q->C.first[current]];
            else
                Q->C.maxvalue = Q->H.value[Q->C.first[current]];
        }
        else
        {
            Error("RGQueue is empty\n","RemoveRGQueue");
        }
    }

    elem = Q->C.first[current];
    Q->C.heapsize[current]--;
    Q->H.elem[elem].color = BLACK;
    if (Q->C.heapsize[current]==0) /* there was a single element in the heap */
    {
        Q->C.first[current] = NIL;
    }
    else
    {
        last = FindHeapElem(Q, Q->C.first[current], Q->C.heapsize[current]);
        assert(last>=0);
        parent = Q->H.elem[last].parent;
        assert(parent>=0);
        if (Q->H.elem[parent].left == last)
            Q->H.elem[parent].left = NIL;
        else
            Q->H.elem[parent].right = NIL;
        Q->H.elem[last].parent = NIL;
        Q->H.elem[last].left = Q->H.elem[elem].left;
        Q->H.elem[last].right = Q->H.elem[elem].right;
        if (Q->H.elem[last].left != NIL)
            Q->H.elem[Q->H.elem[last].left].parent = last;
        if (Q->H.elem[last].right != NIL)
            Q->H.elem[Q->H.elem[last].right].parent = last;
        Q->C.first[current] = last;
        MoveDownHeap(Q, current, last);
    }

    return elem;
}

void RemoveRGQueueElem(RGQueue *Q, int elem)
{
    int bucket, parent, last;

    if (Q->H.elem[elem].color != GRAY)
        return; /* this element is not in queue! */

    if (Q->C.removal_policy == MINVALUE)
        bucket = ((int)Q->H.value[elem])%Q->C.nbuckets;
    else
        bucket = Q->C.nbuckets-1-(((int)Q->H.value[elem])%Q->C.nbuckets);
    assert((bucket>=0) && (bucket<Q->C.nbuckets));

    Q->C.heapsize[bucket]--;
    Q->H.elem[elem].color = BLACK;
    if (Q->C.heapsize[bucket] == 0)
    {
        Q->C.first[bucket] = NIL;
    }
    else
    {
        last = FindHeapElem(Q, Q->C.first[bucket], Q->C.heapsize[bucket]);
        assert(last>=0);
        if (last == elem)
        {
            parent = Q->H.elem[elem].parent;
            if (parent != NIL)
            {
                if (Q->H.elem[parent].left == elem)
                    Q->H.elem[parent].left = NIL;
                else
                    Q->H.elem[parent].right = NIL;
            }
            else
            {
                Q->C.first[bucket] = NIL; /* shouldn't happen! */
            }
        }
        else
        {
            parent = Q->H.elem[last].parent;
            assert(parent>=0);
            if (Q->H.elem[parent].left == last)
                Q->H.elem[parent].left = NIL;
            else
                Q->H.elem[parent].right = NIL;
            Q->H.elem[last].parent = Q->H.elem[elem].parent;
            Q->H.elem[last].left   = Q->H.elem[elem].left;
            Q->H.elem[last].right  = Q->H.elem[elem].right;
            parent = Q->H.elem[last].parent;
            if (parent != NIL)
            {
                if (Q->H.elem[parent].left == elem)
                    Q->H.elem[parent].left = last;
                else
                    Q->H.elem[parent].right = last;
            }
            else
            {
                Q->C.first[bucket] = last;
            }
            if (Q->H.elem[last].left != NIL)
                Q->H.elem[Q->H.elem[last].left].parent = last;
            if (Q->H.elem[last].right != NIL)
                Q->H.elem[Q->H.elem[last].right].parent = last;
            MoveDownHeap(Q, bucket, last);
        }
    }
}

void UpdateRGQueue(RGQueue **Q, int elem, float newvalue)
{
    RemoveRGQueueElem(*Q, elem);
    (*Q)->H.value[elem] = newvalue;
    InsertRGQueue(Q, elem);
}

int EmptyRGQueue(RGQueue *Q)
{
    int last,current;

    if (Q->C.removal_policy==MINVALUE)
        current=((int)Q->C.minvalue)%Q->C.nbuckets;
    else
        current=Q->C.nbuckets-1-(((int)Q->C.maxvalue)%Q->C.nbuckets);

    if (Q->C.first[current] != NIL)
        return 0;

    last = current;

    current = (current + 1) % (Q->C.nbuckets);

    while ((Q->C.first[current] == NIL) && (current != last))
    {
        current = (current + 1) % (Q->C.nbuckets);
    }

    if (Q->C.first[current] == NIL)
    {
        return(1);
    }

    return (0);
}

int SubHeapIsCorrect(RGQueue *Q, int bucket, int elem)
{
   int left  = Q->H.elem[elem].left;
   int right = Q->H.elem[elem].right;
   if (left != NIL)
   {
      if (Q->H.elem[left].parent != elem)
      {
         printf ("[left's parent is someone else!]");
         return 0;
      }
      if (!SubHeapIsCorrect(Q, bucket, left)) return 0;
   }
   if (right != NIL)
   {
      if (left == NIL)
      {
         printf ("[node has a right but no left child!]");
         return 0;
      }
      if (Q->H.elem[right].parent != elem)
      {
         printf ("[right's parent is someone else!]");
         return 0;
      }
      if (!SubHeapIsCorrect(Q, bucket, right)) return 0;
   }
   return 1;
}

int HeapIsCorrect(RGQueue *Q, int bucket)
{
   int first = Q->C.first[bucket];
   if (first != NIL)
   {
      if (Q->H.elem[first].parent != NIL)
      {
         printf ("[first has a parent!]");
         return 0;
      }
      if (!SubHeapIsCorrect(Q, bucket, first)) return 0;
   }
   return 1;
}
