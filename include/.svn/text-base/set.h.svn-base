#ifndef _SET_H_
#define _SET_H_

#include "common.h"

typedef struct _set {
  int elem;
  struct _set *next;
} Set;

void InsertSet(Set **S, int elem);
int  RemoveSet(Set **S);
void RemoveSetElem(Set **S, int elem);
void DestroySet(Set **S);
Set *CloneSet(Set *S);
bool IsInSet(Set *S, int elem);
int  MinimumSetValue(Set *S);
int  MaximumSetValue(Set *S);
void Convert2DisjointSets(Set **S1,
			  Set **S2);
int  GetSetSize(Set *S);
// Merge sets. The next field of the last element of set S points to the first element of set T. T Does not change.
void MergeSets(Set **S, Set **T);

/* vetor de booleanos, usa um bit por booleano
   tamanho = ceil(n / 8) */

typedef struct _bmap {
  char *data;
  int N, VN;
} BMap;

BMap * BMapNew(int n);
void   BMapDestroy(BMap *b);
void   BMapFill(BMap *b, int value);
int    BMapGet(BMap *b, int n);
void   BMapSet(BMap *b, int n, int value);
void   BMapToggle(BMap *b, int n);
void   BMapCopy(BMap *dest, BMap *src);

/* versoes inline de BMapGet, BMapSet e BMapToggle */
/* se estiver apenas setando ou resetando o bit incondicionalmente,
   use Set0 / Set1 */

#define _fast_BMapGet(b,n) ((b->data[n>>3]&(1<<(n&0x07)))!=0)
#define _fast_BMapSet(b,n,v) if (v) b->data[n>>3]|=(1<<(n&0x07)); else b->data[n>>3]&=((~0)^(1<<(n&0x07)));

#define _fast_BMapSet0(b,n) b->data[n>>3]&=((~0)^(1<<(n&0x07)));
#define _fast_BMapSet1(b,n) b->data[n>>3]|=(1<<(n&0x07));
#define _fast_BMapToggle(b,n) b->data[n>>3]^=(1<<(n&0x07));

/* estrutura de conjunto para a DIFT */

/* Sprop, propsize sao usados para manter informacoes de
   semente (label, custo, etc.), Sprop so' e' criada/usada
   se propsize!=0 em IntSetNew */

/* se nao estiver usando Sprop, basta passar propsize=0 e
   passar NULL ou 0 em todos os parametros void *elemprop */

typedef struct _intset {
  int n;
  int count;
  int slsize, slgrow;
  BMap  *Sv;
  int   *Sl;

  int    propsize;
  void  *Sprop; /* propriedades associadas a cada elemento */
} IntSet;

/* ha' descricoes de todas as operacoes em set.c, logo acima
   da declaracao de cada funcao */

IntSet * IntSetNew(int N, int propsize);
void     IntSetDestroy(IntSet *S);
IntSet * IntSetNewCopy(IntSet *S);
void     IntSetShrink(IntSet *S);

int      IntSetBelongs(IntSet *S, int elem);
void     IntSetUnion(IntSet *S, IntSet *T);
void     IntSetUnion1(IntSet *S, int elem, void *elemprop);
void     IntSetMinus(IntSet *S, IntSet *T);
void     IntSetIntersection(IntSet *S, IntSet *T);
int      IntSetEmpty(IntSet *S);
int      IntSetRemoveAny(IntSet *S, void *elemprop);
void     IntSetRemoveElement(IntSet *S, int elem, void *elemprop);
void     IntSetRemoveAll(IntSet *S);

int      IntSetGetElemProp(IntSet *S, int elem, void *elemprop);
int      IntSetSetElemProp(IntSet *S, int elem, void *elemprop);

/* obtem ponteiro para o bloco de propriedades do n-esimo elemento em S */
#define SPROP_PTR(S,n) ((void *)(((char *)((S)->Sprop))+((n)*(S)->propsize)))

#endif
