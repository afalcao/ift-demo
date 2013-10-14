/*
    Copyright (C) <2010> <Alexandre Xavier FalcÃ£o and Thiago Vallin Spina>

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

#include "ift.h"

/* Paper related to this program:

@conference{Falcao01,
 author    = "A.X. Falc{\~{a}}o and B. S. da Cunha and R. A. Lotufo",
 booktitle = "SPIE on Medical Imaging",
 title     = "Design of connected operators using the image foresting transform",
 volume    = "4322",
 month     = "Feb",
 pages     = "468-479",
 year      = "2001"
}

*/

int main(int argc, char **argv)
{
  Image  *img[8];
  Set *S=NULL;
  AdjRel *A=NULL;
  int i;
  timer    *t1=NULL,*t2=NULL;
  char outfile[100];
  char *file_noext;

  /*------- -------------------------------------------------*/
#ifndef _WIN32
  void *trash = malloc(1);
  struct mallinfo info;
  int MemDinInicial, MemDinFinal;
  free(trash);
  info = mallinfo();
  MemDinInicial = info.uordblks;
#endif

  /*--------------------------------------------------------*/

  if (argc != 2) {
    fprintf(stderr,"usage: %s <orig.pgm> \n",argv[0]);
    fprintf(stderr,"orig.pgm: binary image to which morphological filters will be applied\n");
    exit(-1);
  }


  img[0]= ReadImage(argv[1]);

  file_noext = strtok(argv[1],".");

  img[1]= CloseBasins(img[0]);
  sprintf(outfile,"%s_closebasins.pgm",file_noext);
  WriteImage(img[1],outfile);

  img[2]= OpenDomes(img[0]);
  sprintf(outfile,"%s_opendomes.pgm",file_noext);
  WriteImage(img[2],outfile);

  A = Circular(10.0);

  t1 = Tic();

  img[3]= CloseRec(img[0],A);

  t2 = Toc();

  fprintf(stdout,"Closing by reconstruction in %f ms\n",CTime(t1,t2));
  sprintf(outfile,"%s_closerec.pgm",file_noext);
  WriteImage(img[3],outfile);

  img[4]= OpenRec(img[0],A);
  sprintf(outfile,"%s_openrec.pgm",file_noext);
  WriteImage(img[4],outfile);

  DestroyAdjRel(&A);

  for (i=1; i < 5; i++)
    DestroyImage(&img[i]);

  if (MaximumValue(img[0])==1){ // Apply fast binary filters

    img[1] = AddFrame(img[0],10,0); // to guarantee space for dilation
    sprintf(outfile,"%s_shape.pgm",file_noext);
    WriteImage(img[1],outfile);

    img[2]=FastDilate(img[1],&S,10.0);
    sprintf(outfile,"%s_fastdilation.pgm",file_noext);
    WriteImage(img[2],outfile);
    DestroySet(&S);

    img[3]=FastErode(img[1],&S,10.0);
    sprintf(outfile,"%s_fasterosion.pgm",file_noext);
    WriteImage(img[3],outfile);
    DestroySet(&S);

    img[4]=FastClose(img[1],10.0);
    sprintf(outfile,"%s_fastclose.pgm",file_noext);
    WriteImage(img[4],outfile);

    img[5]= FastOpen(img[1],10.0);
    sprintf(outfile,"%s_fastopen.pgm",file_noext);
    WriteImage(img[5],outfile);

    t1 = Tic();

    img[6]=FastCloseRec(img[1],10.0);

    t2 = Toc();

    fprintf(stdout,"Fast closing by reconstruction in %f ms\n",CTime(t1,t2));
    sprintf(outfile,"%s_fastdcloserec.pgm",file_noext);
    WriteImage(img[6],outfile);

    img[7]=FastOpenRec(img[1],10.0);
    sprintf(outfile,"%s_fastopenrec.pgm",file_noext);
    WriteImage(img[7],outfile);

    for (i=1; i < 8; i++)
      DestroyImage(&img[i]);
  }

  DestroyImage(&img[0]);

  /* ---------------------------------------------------------- */

#ifndef _WIN32
  info = mallinfo();
  MemDinFinal = info.uordblks;
  if (MemDinInicial!=MemDinFinal)
    printf("\n\nDinamic memory was not completely deallocated (%d, %d)\n",
	   MemDinInicial,MemDinFinal);
#endif

  return(0);
}
