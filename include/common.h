#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <math.h>
#include <string.h>
#include <limits.h>
#include <float.h>
#include <time.h>


/* Error messages */

#define MSG1  "Cannot allocate memory space"
#define MSG2  "Cannot open file"
#define MSG3  "Invalid option"

/* Common data types to all programs */

#ifndef __cplusplus
typedef enum boolean {false,true} bool;
#endif
typedef unsigned short ushort;
typedef unsigned int uint;

//#ifndef __cplusplus
 //#ifndef _WIN32  
   //#ifndef __cplusplus
      //typedef enum boolean {false,true} bool;
   //#endif
   //#else
   //typedef unsigned short ushort;
   //typedef unsigned int uint;
//#endif
//#endif

typedef struct timeval timer;
typedef unsigned char uchar;

typedef int* ap_int;
typedef double* ap_double;

  // Real type values.
  // Can be changed to float, double.
typedef float real;
#define REAL_MAX FLT_MAX //DBL_MAX
#define REAL_MIN FLT_MIN //DBL_MIN

/* Common definitions */


#define PI          3.1415926536
#define INTERIOR    0
#define EXTERIOR    1
#define BOTH        2
#define WHITE       0
#define GRAY        1
#define BLACK       2
#define NIL        -1
#define INCREASING  1
#define DECREASING  0
#define Epsilon     1E-05

/* Common operations */

#ifndef MAX
#define MAX(x,y) (((x) > (y))?(x):(y))
#endif

#ifndef MIN
#define MIN(x,y) (((x) < (y))?(x):(y))
#endif

#define ROUND(x) ((x < 0)?(int)(x-0.5):(int)(x+0.5))

#define SIGN(x) ((x >= 0)?1:-1)

char   *AllocCharArray(int n);  /* It allocates 1D array of n characters */
uchar  *AllocUCharArray(int n);  /* It allocates 1D array of n characters */
ushort *AllocUShortArray(int n);  /* It allocates 1D array of n ushorts */
uint   *AllocUIntArray(int n); /* It allocates 1D array of n uints */
int    *AllocIntArray(int n);   /* It allocates 1D array of n integers */
float  *AllocFloatArray(int n); /* It allocates 1D array of n floats */
double *AllocDoubleArray(int n);/* It allocates 1D array of n doubles */
real   *AllocRealArray(int n); /* It allocates 1D array of n reals */

void Error(char *msg,char *func); /* It prints error message and exits
                                     the program. */
void Warning(char *msg,char *func); /* It prints warning message and
                                       leaves the routine. */
void Change(int *a, int *b); /* It changes content between a and b */

void FChange(float *a, float *b); /* It changes content between floats a and b */

int NCFgets(char *s, int m, FILE *f); /* It skips # comments */

/**
 * Gera um número inteiro aleatório no intervalo [low,high].
http://www.ime.usp.br/~pf/algoritmos/aulas/random.html
 **/
int RandomInteger (int low, int high);

inline real sqrtreal(real x);

int SafeMod(int a, int n);

/// Algorithm from http://aggregate.org/MAGIC/#Is%20Power%20of%202
int IsPowerOf2(int x);
#endif
