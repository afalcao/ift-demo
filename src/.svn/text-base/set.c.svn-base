/*
    Copyright (C) <2010> <Alexandre Xavier Falcão and Thiago Vallin Spina>

    This file is part of IFT-demo.

    IFT-demo is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    IFT-demo is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with IFT-demo.  If not, see <http://www.gnu.org/licenses/>.

    please see full copyright in COPYING file.
    -------------------------------------------------------------------------

    written by A.X. Falcão <afalcao@ic.unicamp.br> and by T.V. Spina
    <tvspina@liv.ic.unicamp.br>, 2010

*/

#include "set.h"

void InsertSet(Set **S, int elem)
{
  Set *p=NULL;

  p = (Set *) calloc(1,sizeof(Set));
  if (p == NULL) Error(MSG1,"InsertSet");
  if (*S == NULL){
    p->elem  = elem;
    p->next  = NULL;
  }else{
    p->elem  = elem;
    p->next  = *S;
  }
  *S = p;
}

int RemoveSet(Set **S)
{
  Set *p;
  int elem = NIL;

  if (*S != NULL){
    p    =  *S;
    elem = p->elem;
    *S   = p->next;
    //printf("RemoveSet before free");
    free(p);
    //printf(" RemoveSet after free: elem is %d\n",elem);
    //if(*S != NULL) printf(" *S->elem is %d\n",(*S)->elem);
  }

  return(elem);
}


void RemoveSetElem(Set **S, int elem){
  Set *tmp = NULL, *remove;

  tmp = *S;
  if ( tmp->elem == elem ) {
    *S = tmp->next;
    free( tmp );
  }
  else {
    while( tmp->next->elem != elem ) tmp = tmp->next;
    remove = tmp->next;
    tmp->next = remove->next;
    free( remove );
  }
}


void DestroySet(Set **S)
{
  Set *p;
  while(*S != NULL){
    p = *S;
    *S = p->next;
    free(p);
  }
}


//Set *CloneSet(Set *S){
//  Set *tmp = NULL;
//  Set *C = NULL;
//  int p;
//
//  tmp = S;
//  while(tmp!=NULL){
//    p = tmp->elem;
//    InsertSet(&C, p);
//    tmp = tmp->next;
//  }
//  return C;
//}

// Copies the set in an ordered fashion
Set *CloneSet(Set *S){
  Set *tmp = NULL;
  Set *C = NULL;
  Set **tail = NULL;
  int p;

  tmp = S;

  if(tmp != NULL)
  {
  	p = tmp->elem;
	C = (Set *) calloc(1,sizeof(Set));
	C->elem = p;
	C->next = NULL;
	tail = &(C->next);
	tmp = tmp->next;
  }

  while(tmp!=NULL)
  {
  	p = tmp->elem;
	*tail = (Set *) calloc(1,sizeof(Set));
	(*tail)->elem = p;
	(*tail)->next = NULL;
	tail = &((*tail)->next);
    tmp = tmp->next;
  }
  return C;
}

bool IsInSet(Set *S, int elem){
  bool flag = false;

  while(S!=NULL){
    if(S->elem == elem){
      flag = true;
      break;
    }
    S = S->next;
  }
  return flag;
}


int     MinimumSetValue(Set *S){
  Set *aux;
  int p,min=INT_MAX;

  aux = S;
  while(aux != NULL){
    p = aux->elem;
    if(p<min) min = p;
    aux = aux->next;
  }
  return min;
}


int     MaximumSetValue(Set *S){
  Set *aux;
  int p,max=INT_MIN;

  aux = S;
  while(aux != NULL){
    p = aux->elem;
    if(p>max) max = p;
    aux = aux->next;
  }
  return max;
}


void    Convert2DisjointSets(Set **S1,
			     Set **S2){
  BMap *bmap;
  Set **S[2];
  Set *Cur,*Prev;
  int elem,i,max,min,p,n;

  if(*S1==NULL || *S2==NULL)
    return;

  min = MIN(MinimumSetValue(*S1),
	    MinimumSetValue(*S2));

  max = MAX(MaximumSetValue(*S1),
	    MaximumSetValue(*S2));

  n = max-min+1;
  bmap = BMapNew(n);
  S[0] = S1;
  S[1] = S2;

  for(i=0; i<2; i++){
    Prev = Cur = *(S[i]);
    while(Cur != NULL){
      elem = Cur->elem;
      p = elem - min;
      if( _fast_BMapGet(bmap, p) ){
	if(Prev==Cur){
	  *(S[i]) = Cur->next;
	  free(Cur);
	  Prev = Cur = *(S[i]);
	}
	else{
	  Prev->next = Cur->next;
	  free(Cur);
	  Cur = Prev->next;
	}
      }
      else{
	_fast_BMapSet1(bmap, p);

	Prev = Cur;
	Cur = Cur->next;
      }
    }
  }

  BMapDestroy(bmap);
}


int  GetSetSize(Set *S){
  Set *aux;
  int size=0;

  aux = S;
  while(aux != NULL){
    size++;
    aux = aux->next;
  }
  return size;
}

// Merge sets. The next field of the last element of set S points to the first element of set T. T Does not change.
void MergeSets(Set **S, Set **T) {
  Set *aux;
  if ( *S == NULL ) *S = *T;
  else {
    for ( aux = *S; aux->next != NULL; aux = aux->next );
    aux->next = *T;
  }
}

/* BMap */

BMap * BMapNew(int n) {
  BMap *x;
  x= (BMap *) malloc(sizeof(BMap));
  if (!x) return 0;
  x->N    = n;
  x->VN   = n/8;
  if (n%8) x->VN++;
  x->data = (char *) malloc(sizeof(char) * x->VN);
  if (!x->data) return 0;
  BMapFill(x,0);
  return x;
}

void   BMapDestroy(BMap *b) {
  if (b) {
    if (b->data) {
      free(b->data);
      b->data = 0;
    }
    free(b);
  }
}

void   BMapFill(BMap *b, int value) {
  memset(b->data, value?0xff:0, b->VN);
}

void   BMapCopy(BMap *dest, BMap *src) {
  int n;
  BMapFill(dest, 0);
  n = dest->VN;
  if (n > src->VN) n = src->VN;
  memcpy(dest->data,src->data,sizeof(char) * src->VN);
}

static char bmap_set[8]   = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };
static char bmap_reset[8] = { 0xfe, 0xfd, 0xfb, 0xf7, 0xef, 0xdf, 0xbf, 0x7f };

int    BMapGet(BMap *b, int n) {
  int thebyte, thebit, value;

  thebyte = n >> 3;
  thebit  = n & 0x07;

  value = b->data[thebyte] & bmap_set[thebit];
  if (value) value=1;
  return value;
}

void BMapSet(BMap *b, int n, int value) {
  int thebyte, thebit;

  thebyte = n >> 3;
  thebit  = n & 0x07;

  if (value)
    b->data[thebyte] |= bmap_set[thebit];
  else
    b->data[thebyte] &= bmap_reset[thebit];
}

void   BMapToggle(BMap *b, int n) {
  int thebyte, thebit;

  thebyte = n >> 3;
  thebit  = n & 0x07;

  b->data[thebyte] ^= bmap_set[thebit];
}

/* IntSet */
/* a descricao algoritmica desta estrutura esta' na
   secao 3.6 da minha tese -- Felipe Bergo */

/* cria conjunto de inteiros no dominio [0,N) */
IntSet * IntSetNew(int N, int propsize) {
  IntSet *S;

  S = (IntSet *) malloc(sizeof(IntSet));
  if (!S) return 0;

  S->n     = N;
  S->count = 0;
  S->Sv    = BMapNew(N);
  if (!S->Sv) {
    free(S);
    return 0;
  }

  /* tamanho inicial de Sl: 64 elementos, cresce de 256 em 256 */
  S->slsize = 64;
  S->slgrow = 256;
  S->Sl = (int *) malloc(S->slsize * sizeof(int));
  if (!S->Sl) {
    free(S);
    return 0;
  }

  S->propsize = propsize;
  if (propsize) {
    S->Sprop = malloc(propsize * S->slsize);
    if (!S->Sprop) {
      free(S->Sl);
      free(S);
      return 0;
    }
  } else {
    S->Sprop = 0;
  }

  return S;
}

/* destroi conjunto */
void IntSetDestroy(IntSet *S) {
  if (S) {
    if (S->Sl)
      free(S->Sl);
    if (S->Sv)
      BMapDestroy(S->Sv);
    if (S->Sprop)
      free(S->Sprop);
    free(S);
  }
}

/* cria uma copia de S */
IntSet * IntSetNewCopy(IntSet *S) {
  IntSet *T;

  T = IntSetNew(S->n, S->propsize);
  if (!T) return 0;

  T->count    = S->count;
  T->slsize   = S->slsize;
  T->slgrow   = S->slgrow;

  T->Sl = (int *) realloc(T->Sl, T->slsize * sizeof(int));
  memcpy(T->Sl, S->Sl, T->slsize * sizeof(int));

  if (T->propsize)
    memcpy(T->Sprop, S->Sprop, T->propsize * T->slsize);

  BMapCopy(T->Sv, S->Sv);
  return(T);
}

/* operacoes internas (nao estao declaradas no .h para
   evitar que usem errado) */

void IntSetGrow(IntSet *S) {
  S->slsize += S->slgrow;
  S->Sl = (int *) realloc(S->Sl, sizeof(int) * S->slsize);
  if (S->propsize)
    S->Sprop = realloc(S->Sprop, S->propsize * S->slsize);
}

void IntSetAddUniqueElementToSet(IntSet *S, int elem, void *elemprop) {
  _fast_BMapSet1(S->Sv, elem);
  if (S->count == S->slsize)
    IntSetGrow(S);

  if (elemprop!=0 && S->Sprop!=0)
    memcpy( SPROP_PTR(S,S->count), elemprop, S->propsize);

  S->Sl[S->count++] = elem;
}

/* remove um fulano do meio de S->Sl */
void IntSetR1(IntSet *S, int index) {
  S->Sl[index] = S->Sl[--(S->count)];

  if (S->propsize && S->Sprop && index != S->count)
    memcpy( SPROP_PTR(S,index), SPROP_PTR(S,S->count), S->propsize );
}

/* operacoes publicas */

/* retorna diferente de zero se elem pertence a S */
int  IntSetBelongs(IntSet *S, int elem) {
  return(_fast_BMapGet(S->Sv,elem));
}

#define _fast_Belongs(S,elem) (_fast_BMapGet(S->Sv,elem))

/* faz S := S uniao T */
void IntSetUnion(IntSet *S, IntSet *T) {
  int i;
  if (S->propsize != 0 && S->propsize == T->propsize) {

    /* com copia de propriedades */
    for(i=0;i<T->count;i++)
      if (!_fast_Belongs(S,T->Sl[i]))
	IntSetAddUniqueElementToSet(S, T->Sl[i], SPROP_PTR(T,i));

  } else {

    /* sem copia de propriedades */
    for(i=0;i<T->count;i++)
      if (!_fast_Belongs(S,T->Sl[i]))
	IntSetAddUniqueElementToSet(S, T->Sl[i],0);

  }
}

/* faz S := S uniao { elem } */
void IntSetUnion1(IntSet *S, int elem, void *elemprop) {
  if (!_fast_Belongs(S,elem))
    IntSetAddUniqueElementToSet(S, elem, elemprop);
}

/* faz S := S \ T */
void IntSetMinus(IntSet *S, IntSet *T) {
  int i;

  for(i=0;i<T->count;i++)
    if (_fast_Belongs(S,T->Sl[i]))
      _fast_BMapSet0(S->Sv,T->Sl[i]);

  for(i=0;i<S->count;i++)
    if (!_fast_BMapGet(S->Sv,S->Sl[i]))
      IntSetR1(S,i--);
}

/* faz S := S intersecao T */
void IntSetIntersection(IntSet *S, IntSet *T) {
  int i;

  for(i=0;i<T->count;i++)
    if (_fast_Belongs(S,T->Sl[i]))
      _fast_BMapSet0(S->Sv,T->Sl[i]);

  for(i=0;i<S->count;i++) {
    _fast_BMapToggle(S->Sv,S->Sl[i]);
    if (!_fast_BMapGet(S->Sv,S->Sl[i]))
      IntSetR1(S,i--);
  }
}

/* libera memoria, se possivel. Para ser usado, por exemplo,
   apos um IntSetMinus ou IntSetIntersection que reduza muito
   o tamanho de S. Encolhe o Sl e Sprop se a area desperdicada
   for maior que slgrow */
void IntSetShrink(IntSet *S) {
  if (S->slsize - S->count > S->slgrow) {
    S->slsize = S->count;
    S->Sl = (int *) realloc(S->Sl, sizeof(int) * S->slsize);

    if (S->propsize!=0 && S->Sprop!=0)
      S->Sprop = realloc(S->Sprop, S->propsize * S->slsize);
  }
}

/* diferente de zero se conjunto estiver vazio */
int IntSetEmpty(IntSet *S) {
  return(S->count == 0);
}

/* remove um elemento qualquer; retorna elem. Se elemprop!=0 e
   S tiver propsize!=0, copia as propriedades do elemento removido
   para elemprop */
int IntSetRemoveAny(IntSet *S, void *elemprop) {
  _fast_BMapSet0(S->Sv,S->Sl[--S->count]);
  if (elemprop!=0 && S->propsize!=0)
    memcpy(elemprop, SPROP_PTR(S,S->count), S->propsize);
  return(S->Sl[S->count]);
}

/* remove um elemento elem de S, copia suas propriedades para elemprop
   se elemprop!=0 e propsize!=0 */
void IntSetRemoveElement(IntSet *S, int elem, void *elemprop) {
  int i;
  if (_fast_Belongs(S,elem)) {
    i=0;
    while(S->Sl[i]!=elem) ++i;
    if (elemprop!=0 && S->propsize!=0)
      memcpy(elemprop, SPROP_PTR(S,i), S->propsize);
    IntSetR1(S,i);
    _fast_BMapSet0(S->Sv,elem);
  }
}

/* S:= conjunto vazio */
void IntSetRemoveAll(IntSet *S) {
  S->count = 0;
  BMapFill(S->Sv,0);
}

/* tentar copiar propriedades de elem para elemprop, retorna
   0 em caso de falha, !=0 em caso de sucesso.
   falhas possiveis: propsize=0, elem nao esta' em S */
int  IntSetGetElemProp(IntSet *S, int elem, void *elemprop) {
  int i;
  if (_fast_Belongs(S,elem)) {
    i=0;
    while(S->Sl[i]!=elem) ++i;
    if (elemprop!=0 && S->propsize!=0) {
      memcpy(elemprop, SPROP_PTR(S,i), S->propsize);
      return 1;
    }
  }
  return 0;
}

/* tenta alterar propriedade de elem para elemprop,
   retorna 0 em caso de falha, !=0 em caso de sucesso.
   falhas possiveis: propsize=0, elem nao esta' em S */
int IntSetSetElemProp(IntSet *S, int elem, void *elemprop) {
  int i;
  if (_fast_Belongs(S,elem)) {
    i=0;
    while(S->Sl[i]!=elem) ++i;
    if (elemprop!=0 && S->propsize!=0) {
      memcpy(SPROP_PTR(S,i), elemprop, S->propsize);
      return 1;
    }
  }
  return 0;
}
