#ifndef _CTIME_H_
#define _CTIME_H_

#include "common.h"

// to measure time
//#ifndef _WIN32 
//UNIX
#include <sys/time.h> 
#define TIMING_START(M) timer start, now, now1;\
  gettimeofday(&start,NULL);\
  now = start;\
  fprintf(ftime,"================================================\nStarting measuring %s (in milliseconds)\n\n",M);
#define TIMING_SECTION(M) gettimeofday(&now1,NULL);\
  fprintf(ftime,"%f:\tSECTION %s\n",(now1.tv_sec-now.tv_sec)*1000.0 + (now1.tv_usec-now.tv_usec)*0.001,M);\
  now=now1;
#define TIMING_END(M) gettimeofday(&now1,NULL);\
  fprintf(ftime,"%f:\tSECTION %s\n",(now1.tv_sec-now.tv_sec)*1000.0 + (now1.tv_usec-now.tv_usec)*0.001,M);\
  fprintf(ftime,"\nTotal time: %f\n================================================\n",\
      	 (now1.tv_sec-start.tv_sec)*1000.0 + (now1.tv_usec-start.tv_usec)*0.001);

timer *Tic(); /* It marks the initial time */
timer *Toc(); /* It marks the final time */
float CTime(timer *tic, timer *toc); /* It computes the time difference */

#endif
